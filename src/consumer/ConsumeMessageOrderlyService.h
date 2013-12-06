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

#if!defined __CONSUMEMESSAGEORDERLYSERVICE_H__
#define __CONSUMEMESSAGEORDERLYSERVICE_H__

#include "ConsumeMessageService.h"

#include <list>
#include <string>

#include "MessageQueueLock.h"
#include "ConsumerStatManage.h"

class DefaultMQPushConsumerImpl;
class MessageListenerOrderly;
class DefaultMQPushConsumer;

/**
* 顺序消费消息服务
* 
*/
class ConsumeMessageOrderlyService : public ConsumeMessageService
{
public:
	ConsumeMessageOrderlyService(DefaultMQPushConsumerImpl* pDefaultMQPushConsumerImpl,
		MessageListenerOrderly* pMessageListener);

	void start();
	void shutdown();
	void unlockAllMQ();
	void lockMQPeriodically();
	bool lockOneMQ(MessageQueue& mq);
	void tryLockLaterAndReconsume(MessageQueue& mq, 
									ProcessQueue& processQueue,
									long long delayMills);

	ConsumerStat getConsumerStat();
	/**
	* 在Consumer本地定时线程中定时重试
	*/
	void submitConsumeRequestLater(ProcessQueue& processQueue,
									MessageQueue& messageQueue,
									long long suspendTimeMillis);

	void submitConsumeRequest(std::list<MessageExt*>& msgs,
								ProcessQueue& processQueue,
								MessageQueue& messageQueue,
								bool dispathToConsume);

	void updateCorePoolSize(int corePoolSize);

private:
	static long s_MaxTimeConsumeContinuously;

	volatile bool m_stoped;
	DefaultMQPushConsumerImpl* m_pDefaultMQPushConsumerImpl;
	DefaultMQPushConsumer* m_pDefaultMQPushConsumer;
	MessageListenerOrderly* m_pMessageListener;
	std::string m_consumerGroup;
	MessageQueueLock m_messageQueueLock;
};

#endif
