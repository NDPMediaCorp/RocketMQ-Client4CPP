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

#include "ConsumeMessageOrderlyService.h"

#include <list>
#include <string>

#include "DefaultMQPushConsumerImpl.h"
#include "MessageListener.h"
#include "MessageQueue.h"
#include "RebalanceImpl.h"
#include "DefaultMQPushConsumer.h"

ConsumeMessageOrderlyService::ConsumeMessageOrderlyService(DefaultMQPushConsumerImpl* pDefaultMQPushConsumerImpl,
	MessageListenerOrderly* pMessageListener)
{
	m_pDefaultMQPushConsumerImpl = pDefaultMQPushConsumerImpl;
	m_pMessageListener = pMessageListener;
	m_pDefaultMQPushConsumer = m_pDefaultMQPushConsumerImpl->getDefaultMQPushConsumer();
	m_consumerGroup = m_pDefaultMQPushConsumer->getConsumerGroup();
}

void ConsumeMessageOrderlyService::start()
{
}

void ConsumeMessageOrderlyService::shutdown()
{

}

void ConsumeMessageOrderlyService::unlockAllMQ()
{
	m_pDefaultMQPushConsumerImpl->getRebalanceImpl()->unlockAll(false);
}

void ConsumeMessageOrderlyService::lockMQPeriodically()
{
	if (!m_stoped)
	{
		m_pDefaultMQPushConsumerImpl->getRebalanceImpl()->lockAll();
	}
}

bool ConsumeMessageOrderlyService::lockOneMQ(MessageQueue& mq)
{
	if (!m_stoped)
	{
		return m_pDefaultMQPushConsumerImpl->getRebalanceImpl()->lock(mq);
	}

	return false;
}

void ConsumeMessageOrderlyService::tryLockLaterAndReconsume(MessageQueue& mq,
															ProcessQueue& processQueue,
															long long delayMills)
{
}

ConsumerStat ConsumeMessageOrderlyService::getConsumerStat()
{
	return m_pDefaultMQPushConsumerImpl->getConsumerStatManager()->getConsumertat();
}

void ConsumeMessageOrderlyService::submitConsumeRequestLater(ProcessQueue& processQueue,
																MessageQueue& messageQueue,
																long long suspendTimeMillis)
{

}

void ConsumeMessageOrderlyService::submitConsumeRequest(std::list<MessageExt*>& msgs,
														ProcessQueue& processQueue,
														MessageQueue& messageQueue,
														bool dispathToConsume)
{
}

void ConsumeMessageOrderlyService::updateCorePoolSize(int corePoolSize)
{
}
