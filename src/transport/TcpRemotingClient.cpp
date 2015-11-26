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

#include "TcpRemotingClient.h"
#include "TcpTransport.h"
#include "ThreadPool.h"
#include "ScopedLock.h"
#include "KPRUtil.h"
#include "ResponseFuture.h"
#include "SocketUtil.h"


ProcessDataWork::ProcessDataWork(TcpRemotingClient* pClient,std::string* pData)
	:m_pClient(pClient),m_pData(pData)
{

}

ProcessDataWork::~ProcessDataWork()
{

}

void ProcessDataWork::Do()
{
	try
	{
		m_pClient->ProcessData(m_pData);
	}
	catch (...)
	{
	}
}

TcpRemotingClient::TcpRemotingClient(const RemoteClientConfig& config)
	:m_config(config),m_stop (false)
{
	m_pThreadPool = new kpr::ThreadPool(10,5,20);
	m_EventThread = new EventThread(*this);
	m_maxFd=0;
	FD_ZERO (&m_rset);
	SocketInit();
}

TcpRemotingClient::~TcpRemotingClient()
{
	SocketUninit();
}

void TcpRemotingClient::start()
{
	m_EventThread->Start();
}

void TcpRemotingClient::shutdown()
{
	m_stop=true;
	m_pThreadPool->Destroy();
	m_EventThread->Join();
}

void TcpRemotingClient::updateNameServerAddressList(const std::list<std::string>& addrs)
{
	m_namesrvAddrList = addrs;
}

std::list<std::string> TcpRemotingClient::getNameServerAddressList()
{
	return m_namesrvAddrList;
}

RemotingCommand* TcpRemotingClient::invokeSync(const std::string& addr,
		RemotingCommand* request,
		int timeoutMillis)
{
	TcpTransport* tts = GetAndCreateTransport(addr);
	if (tts != NULL && tts->IsConnected())
	{
		return invokeSyncImpl(tts, request, timeoutMillis);
	}
	else
	{
		RemoveTTS(tts);

		return NULL;
	}
}

int TcpRemotingClient::invokeAsync(const std::string& addr,
								   RemotingCommand* request,
								   int timeoutMillis,
								   InvokeCallback* pInvokeCallback)
{
	TcpTransport* tts = GetAndCreateTransport(addr);
	if (tts != NULL && tts->IsConnected())
	{
		return invokeAsyncImpl(tts, request, timeoutMillis, pInvokeCallback);
	}
	else
	{
		RemoveTTS(tts);

		return -1;
	}
}

int TcpRemotingClient::invokeOneway(const std::string& addr,
									RemotingCommand* request,
									int timeoutMillis)
{
	TcpTransport* tts = GetAndCreateTransport(addr);
	if (tts != NULL && tts->IsConnected())
	{
		return invokeOnewayImpl(tts, request, timeoutMillis);
	}
	else
	{
		RemoveTTS(tts);

		return -1;
	}
}

void TcpRemotingClient::HandleSocketEvent(fd_set rset)
{
	std::list<std::string*> data;
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_tcpTransportTableMutex);
		std::map<std::string ,TcpTransport*>::iterator it = m_tcpTransportTable.begin();

		for (; it!=m_tcpTransportTable.end(); it++)
		{
			TcpTransport* tts = it->second;
			if (FD_ISSET (tts->GetSocket(), &rset))
			{
				tts->RecvData(data);
			}
		}
	}

	std::list<std::string*>::iterator it = data.begin();
	for (; it!=data.end(); it++)
	{
		ProcessDataWork* work = new ProcessDataWork(this,(*it));
		m_pThreadPool->AddWork(work);
	}
}

void TcpRemotingClient::UpdateEvent()
{
	kpr::ScopedLock<kpr::Mutex> lock(m_tcpTransportTableMutex);
	std::map<std::string ,TcpTransport*>::iterator it = m_tcpTransportTable.begin();
	m_maxFd=0;
	FD_ZERO (&m_rset);

	for (; it!=m_tcpTransportTable.end(); it++)
	{
		TcpTransport* tts = it->second;
		FD_SET (tts->GetSocket(), &m_rset);
		if (tts->GetSocket() > m_maxFd)
		{
			m_maxFd = tts->GetSocket();
		}
	}
}

void TcpRemotingClient::Run()
{
	fd_set rset, xset;
	unsigned long long beginTime = GetCurrentTimeMillis();

	do
	{
		try
		{
			FD_ZERO (&rset);
			FD_ZERO (&xset);
			{
				kpr::ScopedLock<kpr::Mutex> lock(m_tcpTransportTableMutex);

				rset = m_rset;
				xset = m_rset;
			}

			struct timeval tv = {1, 0};
			int r = select(m_maxFd+1, &rset, NULL, &xset, &tv);
			int err = NET_ERROR;

			if (r == -1 && err == WSAEBADF)
			{
				// worker thread already closed some fd
				// let's loop and build fd set again
				continue;
			}

			if (r > 0)
			{
				HandleSocketEvent (rset);
			}

			HandleTimerEvent(GetCurrentTimeMillis()-beginTime );
		}
		catch (...)
		{
			
		}
	}
	while (!m_stop);
}

TcpTransport* TcpRemotingClient::GetAndCreateTransport( const std::string& addr )
{
	TcpTransport* tts;

	{
		kpr::ScopedLock<kpr::Mutex> lock(m_tcpTransportTableMutex);
		std::map<std::string ,TcpTransport*>::iterator it = m_tcpTransportTable.find(addr);
		if (it!=m_tcpTransportTable.end())
		{
			return it->second;
		}

		std::map<std::string ,std::string> config;
		config["tcp.transport.enableSSL"] = "true";
		tts = new TcpTransport(config);
		if (tts->Connect(addr)!=CLIENT_ERROR_SUCCESS)
		{
			delete tts;
			return NULL;
		}

		m_tcpTransportTable[addr]=tts;
	}

	UpdateEvent();

	return tts;
}

void TcpRemotingClient::HandleTimerEvent(unsigned long long tm)
{
	//TODO 定时器
}

void TcpRemotingClient::ProcessData( std::string* pData )
{
	const char* data = pData->data();
	int len = pData->size();

	RemotingCommand* cmd = RemotingCommand::CreateRemotingCommand(data,len);

	int code;
	if (cmd->isResponseType())
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_responseTableMutex);
		std::map<int,ResponseFuture*>::iterator it = m_responseTable.find(cmd->getOpaque());
		if (it!=m_responseTable.end())
		{
			code=it->second->getRequestCode();
		}
		else
		{
			//TODO 没找到，丢弃,需要记录日志
			delete pData;
			delete cmd;
			return;
		}
	}
	else
	{
		code = cmd->getCode();
	}

	cmd->MakeCustomHeader(code,data,len);
	processMessageReceived(cmd);

	delete pData;
}

RemotingCommand* TcpRemotingClient::invokeSyncImpl( TcpTransport* pTts,
		RemotingCommand* request,
		int timeoutMillis )
{
	ResponseFuture* responseFuture = new ResponseFuture(request->getCode(),
														request->getOpaque(), 
														timeoutMillis,
														NULL,
														true);
	
	{
		responseFuture->IncRef();
		kpr::ScopedLock<kpr::Mutex> lock(m_responseTableMutex);
		m_responseTable.insert(std::pair<int,ResponseFuture*>(request->getOpaque(), responseFuture));
	}

	int ret = SendCmd(pTts,request,timeoutMillis);
	if (ret==0)
	{
		responseFuture->setSendRequestOK(true);
	}
	else
	{
		//TODO close socket?
		{
			kpr::ScopedLock<kpr::Mutex> lock(m_responseTableMutex);
			std::map<int,ResponseFuture*>::iterator it = m_responseTable.find(request->getOpaque());
			if (it != m_responseTable.end())
			{
				it->second->DecRef();
				m_responseTable.erase(it);
			}
		}

		responseFuture->DecRef();

		return NULL;
	}

	RemotingCommand* responseCommand = responseFuture->waitResponse(timeoutMillis);
	if (responseCommand ==NULL)
	{
		// 发送请求成功，读取应答超时
		if (ret == 0)
		{
			
		}
		else// 发送请求失败
		{
		}
	}

	// 如果发现，说明超时，所以可以在这里删除
	// 如果没有发现，说明已经在处理了，那么就在处理的地方删除
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_responseTableMutex);
		std::map<int,ResponseFuture*>::iterator it = m_responseTable.find(request->getOpaque());
		if (it != m_responseTable.end())
		{
			it->second->DecRef();
			m_responseTable.erase(it);
		}
	}

	responseFuture->DecRef();

	return responseCommand;
}

int TcpRemotingClient::invokeAsyncImpl( TcpTransport* pTts,
										RemotingCommand* request,
										int timeoutMillis,
										InvokeCallback* pInvokeCallback )
{
	ResponseFuture* responseFuture = new ResponseFuture(request->getCode(),
														request->getOpaque(),
														timeoutMillis,
														pInvokeCallback,
														true);
	
	{
		responseFuture->IncRef();
		kpr::ScopedLock<kpr::Mutex> lock(m_responseTableMutex);
		m_responseTable.insert(std::pair<int,ResponseFuture*>(request->getOpaque(), responseFuture));
	}

	int ret = SendCmd(pTts,request,timeoutMillis);
	if (ret==0)
	{
		responseFuture->setSendRequestOK(true);
	}
	else
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_responseTableMutex);
		std::map<int,ResponseFuture*>::iterator it = m_responseTable.find(request->getOpaque());
		if (it != m_responseTable.end())
		{
			it->second->DecRef();
			m_responseTable.erase(it);
		}
	}

	responseFuture->DecRef();

	return ret;
}

int TcpRemotingClient::invokeOnewayImpl( TcpTransport* pTts,
		RemotingCommand* request,
		int timeoutMillis )
{
	request->markOnewayRPC();
	SendCmd(pTts,request,timeoutMillis);

	return 0;
}

void TcpRemotingClient::processMessageReceived(RemotingCommand* pCmd)
{
	switch (pCmd->getType())
	{
	case REQUEST_COMMAND:
		processRequestCommand(pCmd);
		break;
	case RESPONSE_COMMAND:
		processResponseCommand(pCmd);
		break;
	default:
		break;
	}
}

void TcpRemotingClient::processRequestCommand(RemotingCommand* pCmd)
{

}

void TcpRemotingClient::processResponseCommand(RemotingCommand* pCmd)
{
	ResponseFuture* res = NULL;
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_responseTableMutex);
		std::map<int,ResponseFuture*>::iterator it = m_responseTable.find(pCmd->getOpaque());
		if (it!=m_responseTable.end())
		{
			res = it->second;
			m_responseTable.erase(it);
		}
	}

	// 同步调用需要通知等待者，异步调用需要调用回调函数
	if (res)
	{
		res->putResponse(pCmd);
		res->executeInvokeCallback();
	}
	else
	{
		Logger::get_logger()->warn("Unable to find request[opaque = {}]", pCmd->getOpaque());
		delete pCmd;
	}
}

int TcpRemotingClient::SendCmd( TcpTransport* pTts,RemotingCommand* msg,int timeoutMillis )
{
	int ret = pTts->SendData(msg->GetHead(),msg->GetHeadLen(),timeoutMillis);
	if (ret==0&&msg->GetBody())
	{
		ret = pTts->SendData(msg->GetBody(),msg->GetBodyLen(),timeoutMillis);
	}

	return ret;
}

void TcpRemotingClient::registerProcessor( int requestCode, TcpRequestProcessor* pProcessor )
{
	m_processorTable[requestCode]=pProcessor;
}

void TcpRemotingClient::RemoveTTS( TcpTransport* pTts )
{
	if (pTts)
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_tcpTransportTableMutex);

		std::map<std::string ,TcpTransport*>::iterator it = m_tcpTransportTable.find(pTts->GetServerURL());
		m_tcpTransportTable.erase(it);

		delete pTts;
	}
}
