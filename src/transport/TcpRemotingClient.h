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

#if!defined __TCPREMOTINGCLIENT_H__
#define __TCPREMOTINGCLIENT_H__

#include <map>
#include <string>
#include <list>
#include "SocketUtil.h"
#include "RemotingCommand.h"
#include "Thread.h"
#include "ThreadPool.h"
#include "ThreadPoolWork.h"
#include "RemoteClientConfig.h"

class TcpTransport;
class InvokeCallback;
class TcpRemotingClient;
class ResponseFuture;
class TcpRequestProcessor;

class ProcessDataWork : public ThreadPoolWork
{
public:
	ProcessDataWork(TcpRemotingClient* pClient,std::string* pData);
	virtual ~ProcessDataWork();
	virtual void Do();

private:
	TcpRemotingClient* m_pClient;
	std::string* m_pData;
};

/**
* 远程通信Client
*
*/
class TcpRemotingClient
{
public:
	TcpRemotingClient(const RemoteClientConfig& config);
	virtual ~TcpRemotingClient();
	virtual void start();
	virtual void shutdown();

	//TODO 可能不需要这个两个函数
	void updateNameServerAddressList(const std::list<std::string>& addrs);
	std::list<std::string> getNameServerAddressList();

	RemotingCommand* invokeSync(const std::string& addr,
								RemotingCommand* request,
								int timeoutMillis) ;

	int invokeAsync(const std::string& addr,
					RemotingCommand* request,
					int timeoutMillis,
					InvokeCallback* invokeCallback);

	int invokeOneway(const std::string& addr,
					 RemotingCommand* request,
					 int timeoutMillis);

	void registerProcessor(int requestCode, TcpRequestProcessor* pProcessor);

	class EventThread : public kpr::Thread
	{
	public:
		EventThread(TcpRemotingClient& client)
			:Thread("EventThread"),m_client(client)
		{
		}

		void Run()
		{
			m_client.Run();
		}

	private :
		TcpRemotingClient& m_client;
	};

	friend class EventThread;
	friend class ProcessDataWork;

private:
	int SendCmd(TcpTransport* pTts,RemotingCommand* msg,int timeoutMillis);
	void RemoveTTS(TcpTransport* pTts);
	void ProcessData(std::string* pData);
	void HandleSocketEvent(fd_set wset);
	void HandleTimerEvent(unsigned long long tm);
	void UpdateEvent();
	void Run();
	void processMessageReceived(RemotingCommand* pCmd);
	void processRequestCommand(RemotingCommand* pCmd);
	void processResponseCommand(RemotingCommand* pCmd);
	TcpTransport* GetAndCreateTransport(const std::string& addr);

	RemotingCommand* invokeSyncImpl(TcpTransport* pTts,
									RemotingCommand* request,
									int timeoutMillis) ;

	int invokeAsyncImpl(TcpTransport* pTts,
						RemotingCommand* request,
						int timeoutMillis,
						InvokeCallback* pInvokeCallback);

	int invokeOnewayImpl(TcpTransport* pTts,
						 RemotingCommand* request,
						 int timeoutMillis);

private:
	bool m_stop;
	fd_set m_rset;
	SOCKET m_maxFd;
	kpr::Mutex m_tcpTransportTableMutex;
	kpr::Mutex m_responseTableMutex;

	std::map<int,ResponseFuture*> m_responseTable;
	std::map<std::string ,TcpTransport*> m_tcpTransportTable;
	std::list<std::string> m_namesrvAddrList;
	kpr::ThreadPool* m_pThreadPool;
	RemoteClientConfig m_config;	
	TcpRequestProcessor* m_pDefaultRequestProcessor;// 默认请求代码处理器	
	std::map<int, TcpRequestProcessor*> m_processorTable;// 注册的各个RPC处理器
	kpr::Thread_var m_EventThread;
};

#endif
