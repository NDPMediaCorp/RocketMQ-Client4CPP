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
#include "ConsumeMessageConcurrentlyService.h"

#include "DefaultMQPushConsumerImpl.h"
#include "MessageListener.h"
#include "MessageQueue.h"
#include "RebalanceImpl.h"
#include "DefaultMQPushConsumer.h"

ConsumeMessageConcurrentlyService::ConsumeMessageConcurrentlyService(
	DefaultMQPushConsumerImpl* pDefaultMQPushConsumerImpl,
	MessageListenerConcurrently* pMessageListener)
{
	m_pDefaultMQPushConsumerImpl = pDefaultMQPushConsumerImpl;
	m_pMessageListener = pMessageListener;
	m_pDefaultMQPushConsumer = m_pDefaultMQPushConsumerImpl->getDefaultMQPushConsumer();
	m_consumerGroup = m_pDefaultMQPushConsumer->getConsumerGroup();
}

void ConsumeMessageConcurrentlyService::start()
{
}

void ConsumeMessageConcurrentlyService::shutdown()
{
}

ConsumerStat ConsumeMessageConcurrentlyService::getConsumerStat()
{
	return m_pDefaultMQPushConsumerImpl->getConsumerStatManager()->getConsumertat();
}

bool ConsumeMessageConcurrentlyService::sendMessageBack(MessageExt& msg,
	ConsumeConcurrentlyContext& context)
{
	// 如果用户没有设置，服务器会根据重试次数自动叠加延时时间

	try
	{
		m_pDefaultMQPushConsumerImpl->sendMessageBack(msg, context.delayLevelWhenNextConsume);
		return true;
	}
	catch (...)
	{

	}

	return false;
}

void ConsumeMessageConcurrentlyService::submitConsumeRequestLater(std::list<MessageExt>& msgs,
																	ProcessQueue& processQueue,
																	MessageQueue& messageQueue)
{

}

void ConsumeMessageConcurrentlyService::submitConsumeRequest(std::list<MessageExt*>& msgs,
																ProcessQueue& processQueue,
																MessageQueue& messageQueue,
																bool dispathToConsume)
{
}

void ConsumeMessageConcurrentlyService::updateCorePoolSize(int corePoolSize)
{
}
