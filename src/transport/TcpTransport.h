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
#if!defined __TCPTRANSPORT_H__
#define __TCPTRANSPORT_H__

#include <map>
#include <string>
#include <list>
#include "Mutex.h"
#include "SocketUtil.h"

const int CLIENT_STATE_UNINIT = 0;
const int CLIENT_STATE_INITED = 1;
const int CLIENT_STATE_DISCONNECT = 2;
const int CLIENT_STATE_CONNECTED = 3;

const int CLIENT_ERROR_SUCCESS=0;
const int CLIENT_ERROR_INIT=1;
const int CLIENT_ERROR_INVALID_URL=2;
const int CLIENT_ERROR_CONNECT=3;
const int CLIENT_ERROR_OOM=4;

class TcpTransport
{
public:
	TcpTransport(std::map<std::string, std::string>& config);
	~TcpTransport();

	int Connect(const std::string &strServerURL);
	bool IsConnected();
	void Close();

	int SendData(const char* pBuffer, size_t len, int nTimeOut = -1);
	ssize_t RecvData(std::list<std::string*>& outDataList);
	void Run();
	SOCKET GetSocket();
	std::string& GetServerURL();

private:
	int SendOneMsg(const char* pBuffer, size_t len, int nTimeout);
	ssize_t RecvMsg();
	void ProcessData(std::list<std::string*>& outDataList);
	bool ResizeBuf(uint32_t nNewSize);
	void TryShrink(uint32_t nMsgLen);
	static uint32_t GetMsgSize(const char * pBuf);

private:
	int m_sfd;
	int m_state;
	size_t m_recvBufSize;
	size_t m_recvBufUsed;
	uint32_t m_shrinkMax;
	int m_shrinkCheckCnt;
	kpr::Mutex m_sendLock;
	kpr::Mutex m_recvLock;
	std::string m_serverURL;
	char * m_pRecvBuf;
	bool enableSSL;
    SSL_CTX* sslContext;
    SSL* ssl;
};

#endif
