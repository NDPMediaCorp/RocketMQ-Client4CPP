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

#include "PullMessageService.h"
#include <list>
#include "MQClientFactory.h"
#include "MQConsumerInner.h"
#include "PullRequest.h"
#include "DefaultMQPushConsumerImpl.h"
#include "ScopedLock.h"


PullMessageService::PullMessageService(MQClientFactory* pMQClientFactory)
	:ServiceThread("PullMessageService"),
	 m_pMQClientFactory(pMQClientFactory)
{
	m_TimeThread = new kpr::TimerThread("PullMessageService-timer",1000);
}


PullMessageService::~PullMessageService()
{

}

/**
* 只定时一次
*/
void PullMessageService::executePullRequestLater(PullRequest* pPullRequest, long timeDelay)
{
	MyTimeHandler* handler = new MyTimeHandler(this,pPullRequest);
	
	m_TimeThread->RegisterTimer(0,timeDelay,handler,false);
}


/**
* 立刻执行PullRequest
*/
void PullMessageService::executePullRequestImmediately(PullRequest* pPullRequest)
{
	try
	{
		{
			kpr::ScopedLock<kpr::Mutex> lock(m_lock);
			m_pullRequestQueue.push_back(pPullRequest);
		}

		wakeup();
	}
	catch (...)
	{
	}
}

void PullMessageService::Run()
{
	while (!m_stoped)
	{
		try
		{
			bool wait = false;

			{
				kpr::ScopedLock<kpr::Mutex> lock(m_lock);
				if (m_pullRequestQueue.empty())
				{
					wait = true;
				}
			}
			
			if (wait)
			{
				waitForRunning(5000);
			}

			PullRequest* pullRequest=NULL;
			{
				kpr::ScopedLock<kpr::Mutex> lock(m_lock);
				if (!m_pullRequestQueue.empty())
				{
					pullRequest = m_pullRequestQueue.front();
					m_pullRequestQueue.pop_front();
				}
			}

			if (pullRequest != NULL)
			{
				pullMessage(pullRequest);
			}
		}
		catch (...)
		{

		}
	}

	m_TimeThread->Close();
	m_TimeThread->Join();
}

std::string PullMessageService::getServiceName()
{
	return "PullMessageService";
}


void PullMessageService::pullMessage(PullRequest* pPullRequest)
{
	MQConsumerInner* consumer = m_pMQClientFactory->selectConsumer(pPullRequest->getConsumerGroup());
	if (consumer != NULL)
	{
		DefaultMQPushConsumerImpl* impl = (DefaultMQPushConsumerImpl*) consumer;
		impl->pullMessage(pPullRequest);
	}
	else
	{
		Logger::get_logger()->error("Unable to find consumer instance from MQClientFactory");
	}
}
