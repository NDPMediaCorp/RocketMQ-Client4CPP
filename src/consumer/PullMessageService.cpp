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


PullMessageService::PullMessageService(MQClientFactory* pMQClientFactory)
	:ServiceThread("PullMessageService"),
	 m_pMQClientFactory(pMQClientFactory)
{
}


PullMessageService::~PullMessageService()
{

}

/**
* 只定时一次
*/
void PullMessageService::executePullRequestLater(PullRequest* pPullRequest, long timeDelay)
{

}


/**
* 立刻执行PullRequest
*/
void PullMessageService::executePullRequestImmediately(PullRequest* pPullRequest)
{
	try
	{
		m_pullRequestQueue.push_back(pPullRequest);
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
			if (m_pullRequestQueue.empty())
			{
				Sleep(5000);
				//Wait();
			}

			if (!m_pullRequestQueue.empty())
			{
				PullRequest* pullRequest = m_pullRequestQueue.front();
				if (pullRequest != NULL)
				{
					pullMessage(pullRequest);
				}

				m_pullRequestQueue.pop_front();
			}
		}
		catch (...)
		{

		}
	}
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
		//TODO
	}
}

