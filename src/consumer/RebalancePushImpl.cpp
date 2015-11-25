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

#include "RebalancePushImpl.h"

RebalancePushImpl::RebalancePushImpl(DefaultMQPushConsumerImpl* pDefaultMQPushConsumerImpl)
	:RebalanceImpl("",BROADCASTING,NULL,NULL),
	m_pDefaultMQPushConsumerImpl(pDefaultMQPushConsumerImpl)
{
}

RebalancePushImpl::RebalancePushImpl(const std::string& consumerGroup,
	MessageModel messageModel,
	AllocateMessageQueueStrategy* pAllocateMessageQueueStrategy,
	MQClientFactory* pMQClientFactory,
	DefaultMQPushConsumerImpl* pDefaultMQPushConsumerImpl)
	:RebalanceImpl(consumerGroup, messageModel, pAllocateMessageQueueStrategy, pMQClientFactory),
	m_pDefaultMQPushConsumerImpl(pDefaultMQPushConsumerImpl)
{
}

void RebalancePushImpl::dispatchPullRequest(std::list<PullRequest*>& pullRequestList)
{
	std::list<PullRequest*>::iterator it = pullRequestList.begin();
	// 派发PullRequest
	for (;it!=pullRequestList.end();it++)
	{
		m_pDefaultMQPushConsumerImpl->executePullRequestImmediately(*it);
	}
}

long long RebalancePushImpl::computePullFromWhere(MessageQueue& mq)
{
	long long result = -1;
	ConsumeFromWhere consumeFromWhere =
		m_pDefaultMQPushConsumerImpl->getDefaultMQPushConsumer()->getConsumeFromWhere();
	OffsetStore* offsetStore = m_pDefaultMQPushConsumerImpl->getOffsetStore();

	switch (consumeFromWhere)
	{
	case CONSUME_FROM_LAST_OFFSET:
		{
			long long lastOffset = offsetStore->readOffset(mq, READ_FROM_STORE);
			if (lastOffset >= 0)
			{
				result = lastOffset;
			}
			// 当前订阅组在服务器没有对应的Offset
			// 说明是第一次启动
			else if (-1 == lastOffset)
			{
				// 如果是重试队列，需要从0开始
				if (strncmp(MixAll::RETRY_GROUP_TOPIC_PREFIX.c_str(),mq.getTopic().c_str(),MixAll::RETRY_GROUP_TOPIC_PREFIX.size())==0)
				{
					result = 0L;
				}
				// 正常队列则从末尾开始
				else
				{
					result = LLONG_MAX;
				}
			}
			// 发生其他错误
			else
			{
				result = -1;
			}
			break;
		}
	case CONSUME_FROM_LAST_OFFSET_AND_FROM_MIN_WHEN_BOOT_FIRST:
		{
			long long lastOffset = offsetStore->readOffset(mq, READ_FROM_STORE);
			if (lastOffset >= 0)
			{
				result = lastOffset;
			}
			// 当前订阅组在服务器没有对应的Offset
			// 说明是第一次启动
			else if (-1 == lastOffset)
			{
				result = 0L;
			}
			// 发生其他错误
			else
			{
				result = -1;
			}
			break;
		}
	case CONSUME_FROM_MAX_OFFSET:
		result = LLONG_MAX;
		break;
	case CONSUME_FROM_MIN_OFFSET:
		result = 0L;
		break;
	default:
		break;
	}

	return result;
}

void RebalancePushImpl::messageQueueChanged(const std::string& topic,
	std::set<MessageQueue>& mqAll, 
	std::set<MessageQueue>& mqDivided)
{
}


void RebalancePushImpl::removeUnnecessaryMessageQueue(MessageQueue& mq, ProcessQueue& pq)
{
	m_pDefaultMQPushConsumerImpl->getOffsetStore()->persist(mq);
	m_pDefaultMQPushConsumerImpl->getOffsetStore()->removeOffset(mq);
	if (m_pDefaultMQPushConsumerImpl->isConsumeOrderly())
	{
		unlock(mq, true);
	}
}

