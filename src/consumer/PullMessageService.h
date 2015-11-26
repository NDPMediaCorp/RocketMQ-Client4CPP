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
#if!defined __PULLMESSAGESERVICE_H__
#define __PULLMESSAGESERVICE_H__

#include "ServiceThread.h"
#include <list>
#include "TimerThread.h"
#include "Logger.h"

class MQClientFactory;
class MQConsumerInner;
class PullRequest;

/**
* 长轮询拉消息服务，单线程异步拉取
*
*/
class PullMessageService : public ServiceThread
{
public:
	PullMessageService(MQClientFactory* pMQClientFactory);
	~PullMessageService();

	/**
	* 只定时一次
	*/
	void executePullRequestLater(PullRequest* pPullRequest, long timeDelay);

	/**
	* 立刻执行PullRequest
	*/
	void executePullRequestImmediately(PullRequest* pPullRequest);

	

	std::string getServiceName();

	void Run();

	class MyTimeHandler :public kpr::TimerHandler
	{
	public:
		MyTimeHandler(PullMessageService* pService, PullRequest* pPullRequest)
			:m_pService(pService),m_pPullRequest(pPullRequest)
		{

		}

		void OnTimeOut(unsigned int timerID)
		{
			m_pService->executePullRequestImmediately(m_pPullRequest);
			delete this;
		}

	private:
		PullMessageService* m_pService;
		PullRequest* m_pPullRequest;
	};
private:
	void pullMessage(PullRequest* pPullRequest);

private:
	std::list<PullRequest*> m_pullRequestQueue;
	kpr::Mutex m_lock;
	MQClientFactory* m_pMQClientFactory;
	kpr::TimerThread_var m_TimeThread;
};

#endif
