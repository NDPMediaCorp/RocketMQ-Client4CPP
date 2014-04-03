/**
* Copyright (C) 2013 kangliqiang ,kangliq@163.com
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include "TcpTransport.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <assert.h>

#include "SocketUtil.h"
#include "ScopedLock.h"

const int DEFAULT_SHRINK_COUNT = 32;
const int DEFAULT_RECV_BUFFER_SIZE = 1024 * 16;

TcpTransport::TcpTransport(std::map<std::string, std::string>& config)
	:m_sfd(0),
	 m_state(CLIENT_STATE_UNINIT),
	 m_pRecvBuf(NULL),
	 m_recvBufSize(DEFAULT_RECV_BUFFER_SIZE),
	 m_recvBufUsed(0),
	 m_shrinkMax(DEFAULT_RECV_BUFFER_SIZE)
	 ,m_shrinkCheckCnt(DEFAULT_SHRINK_COUNT)
{
	std::map<std::string, std::string>::iterator it = config.find("tcp.transport.recvBufferSize");
	if (it != config.end())
	{
		m_recvBufSize = atoi(it->second.c_str());
	}

	it = config.find("tcp.transport.shrinkCheckMax");
	if (it != config.end())
	{
		m_shrinkCheckCnt = atoi(it->second.c_str());
	}

	if (SocketInit() != 0)
	{
		m_state = CLIENT_STATE_UNINIT;
	}

	m_pRecvBuf = (char*)malloc(m_recvBufSize);

	m_state = (NULL == m_pRecvBuf) ? CLIENT_STATE_UNINIT : CLIENT_STATE_INITED;
}

TcpTransport::~TcpTransport()
{
	Close();

	if (m_pRecvBuf)
	{
		free(m_pRecvBuf);
	}

	if (m_sfd != INVALID_SOCKET)
	{
		shutdown(m_sfd,SD_BOTH);
		closesocket(m_sfd);
		m_sfd = INVALID_SOCKET;
	}

	SocketUninit();
}

int TcpTransport::Connect(const std::string &strServerURL)
{
	if (m_state == CLIENT_STATE_UNINIT)
	{
		return CLIENT_ERROR_INIT;
	}

	if (IsConnected())
	{
		if (strServerURL.compare(m_serverURL) == 0)
		{
			return CLIENT_ERROR_SUCCESS;
		}
		else
		{
			Close();
		}
	}

	short port;
	std::string strAddr;

	if (!SplitURL(strServerURL, strAddr, port))
	{
		return CLIENT_ERROR_INVALID_URL;
	}

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	sa.sin_addr.s_addr = inet_addr(strAddr.c_str());
	m_sfd = (int)socket(AF_INET, SOCK_STREAM, 0);

	if (MakeSocketNonblocking(m_sfd) == -1)
	{
		return CLIENT_ERROR_CONNECT;
	}

	if (SetTcpNoDelay(m_sfd) == -1)
	{
		closesocket(m_sfd);
		return CLIENT_ERROR_CONNECT;
	}
	
	if (connect(m_sfd,(struct sockaddr*)&sa, sizeof(sockaddr)) == -1)
	{
		int err = NET_ERROR;
		if (err == WSAEWOULDBLOCK || err == WSAEINPROGRESS)
		{
			fd_set wfd;
			fd_set exceptfds;

			FD_ZERO(&wfd);
			FD_ZERO(&exceptfds);
			FD_SET(m_sfd,&wfd);
			FD_SET(m_sfd, &exceptfds);

			struct timeval tv = {5, 0};

			if (select(FD_SETSIZE, NULL, &wfd, &exceptfds, &tv) == -1)
			{
				closesocket(m_sfd);
				return CLIENT_ERROR_CONNECT;
			}

			if (!FD_ISSET(m_sfd,&wfd))
			{
				closesocket(m_sfd);
				return CLIENT_ERROR_CONNECT;
			}

#ifndef WIN32

			int opterr = 0;
			socklen_t errlen = sizeof(opterr);
			if (getsockopt(m_sfd,SOL_SOCKET, SO_ERROR, &opterr, &errlen) == -1)
			{
				closesocket(m_sfd);
				return CLIENT_ERROR_CONNECT;
			}

			if (opterr)
			{
				closesocket(m_sfd);
				return CLIENT_ERROR_CONNECT;
			}
#endif
		}
		else
		{
			return CLIENT_ERROR_CONNECT;
		}
	}

	m_serverURL = strServerURL;
	m_state = CLIENT_STATE_CONNECTED;

	m_recvBufUsed = 0;

	return CLIENT_ERROR_SUCCESS;
}

bool TcpTransport::IsConnected()
{
	return m_state == CLIENT_STATE_CONNECTED;
}

void TcpTransport::Close()
{
	if (m_state == CLIENT_STATE_CONNECTED)
	{
		m_state = CLIENT_STATE_DISCONNECT;
	}
}

int TcpTransport::SendData(const char* pBuffer, int len,int timeOut)
{
	kpr::ScopedLock<kpr::Mutex> lock(m_sendLock);
	return SendOneMsg(pBuffer,len,timeOut);
}

int TcpTransport::SendOneMsg(const char* pBuffer, int len, int nTimeOut)
{
	int pos = 0;

	while (len > 0 && m_state == CLIENT_STATE_CONNECTED)
	{
		int ret = send(m_sfd, pBuffer + pos, len, 0);
		if (ret > 0)
		{
			len -= ret;
			pos += ret;
		}
		else if (ret == 0)
		{
			Close();
			break;
		}
		else
		{
			int err = NET_ERROR;
			if (err == WSAEWOULDBLOCK||err==EAGAIN)
			{
				fd_set wfd;
				FD_ZERO(&wfd);
				FD_SET(m_sfd, &wfd);

				struct timeval tv = {2, 0};
				struct timeval* tvp = NULL;

				if (nTimeOut >0)
				{
					tv.tv_sec = nTimeOut / 1000;
					tv.tv_usec = (nTimeOut % 1000) * 1000;
				}

				tvp = &tv;

				if (select(m_sfd+1, NULL, &wfd, NULL, tvp) == -1)
				{
					Close();
					break;
				}

				if (nTimeOut > 0 && !FD_ISSET(m_sfd,&wfd))
				{
					Close();
					break;
				}
			}
			else
			{
				Close();
				break;
			}
		}
	}

	return (len == 0) ? 0 : -1;
}

int TcpTransport::RecvMsg()
{
	int ret = recv(m_sfd, m_pRecvBuf + m_recvBufUsed, m_recvBufSize - m_recvBufUsed, 0);

	if (ret > 0)
	{
		m_recvBufUsed += ret;
	}
	else if (ret == 0)
	{
		Close();
	}
	else if (ret == -1)
	{
		int err = NET_ERROR;
		if (err != WSAEWOULDBLOCK && err != EAGAIN)
		{
			Close();
		}
	}

	return ret ;
}

bool TcpTransport::ResizeBuf(int nNewSize)
{
	char * newbuf = (char*)realloc(m_pRecvBuf,nNewSize);
	if (!newbuf)
	{
		return false;
	}

	m_pRecvBuf = newbuf;
	m_recvBufSize = nNewSize;

	return true;
}

void TcpTransport::TryShrink(int MsgLen)
{
	m_shrinkMax = MsgLen > m_shrinkMax ? MsgLen : m_shrinkMax;
	if (m_shrinkCheckCnt == 0)
	{
		m_shrinkCheckCnt = DEFAULT_SHRINK_COUNT;
		if (m_recvBufSize > m_shrinkMax)
		{
			ResizeBuf(m_shrinkMax);
		}
	}
	else
	{
		m_shrinkCheckCnt--;
	}
}

int TcpTransport::GetMsgSize(const char * pBuf)
{
	int len = 0;
	memcpy(&len, pBuf, sizeof(int));

	//由于长度值不包含自身，所以需要+4
	return ntohl(len)+4;
}

int TcpTransport::RecvData(std::list<std::string*>& outDataList)
{
	int ret = RecvMsg();
	ProcessData(outDataList);
	return ret;
}

void TcpTransport::ProcessData(std::list<std::string*>& outDataList)
{
	while (m_recvBufUsed > int(sizeof(int)))
	{
		int msgLen = 0;
		msgLen = GetMsgSize(m_pRecvBuf);
		if (msgLen > m_recvBufSize)
		{
			if (ResizeBuf(msgLen))
			{
				m_shrinkCheckCnt = DEFAULT_SHRINK_COUNT;
			}
			break;
		}
		else
		{
			TryShrink(msgLen);
		}

		if (m_recvBufUsed >= msgLen)
		{
			std::string* data = new std::string;
			data->assign(m_pRecvBuf,msgLen);
			outDataList.push_back(data);
			m_recvBufUsed -= msgLen;

			memmove(m_pRecvBuf, m_pRecvBuf + msgLen, m_recvBufUsed);
		}
		else
		{
			break;
		}
	}
}

SOCKET TcpTransport::GetSocket()
{
	return m_sfd;
}
