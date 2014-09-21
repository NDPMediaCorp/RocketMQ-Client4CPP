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

#include "ConsumerStatManage.h"
#include "MessageQueueLock.h"
#include "MessageListener.h"
#include "ThreadPool.h"
#include "TimerThread.h"

class DefaultMQPushConsumerImpl;
class MessageListenerOrderly;
class DefaultMQPushConsumer;
class ConsumeMessageOrderlyService;

class ConsumeOrderlyRequest: public ThreadPoolWork
{
public:
	ConsumeOrderlyRequest(ProcessQueue* pProcessQueue,
						  MessageQueue& messageQueue,
						  ConsumeMessageOrderlyService* pService);
	~ConsumeOrderlyRequest();

	virtual void Do();

	ProcessQueue* getProcessQueue()
	{
		return m_pProcessQueue;
	}
	
	MessageQueue& getMessageQueue()
	{
		return m_messageQueue;
	}

private:
	ProcessQueue* m_pProcessQueue;
	MessageQueue& m_messageQueue;
	ConsumeMessageOrderlyService* m_pService;
};


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
	void tryLockLaterAndReconsume(MessageQueue& messageQueue,
									ProcessQueue* pProcessQueue,
									long long delayMills);
	bool processConsumeResult(std::list<MessageExt*>& msgs,
		ConsumeOrderlyStatus status,
		ConsumeOrderlyContext& context,
		ConsumeOrderlyRequest& consumeRequest);

	ConsumerStat& getConsumerStat();
	/**
	* 在Consumer本地定时线程中定时重试
	*/
	void submitConsumeRequestLater(ProcessQueue* pProcessQueue,
									MessageQueue& messageQueue,
									long long suspendTimeMillis);

	void submitConsumeRequest(std::list<MessageExt*>& msgs,
								ProcessQueue* pProcessQueue,
								MessageQueue& messageQueue,
								bool dispathToConsume);

	void updateCorePoolSize(int corePoolSize);
	MessageQueueLock& getMessageQueueLock();
	std::string& getConsumerGroup();
	MessageListenerOrderly* getMessageListener();
	DefaultMQPushConsumerImpl* getDefaultMQPushConsumerImpl();
	DefaultMQPushConsumer* getDefaultMQPushConsumer();

public:
	static long s_MaxTimeConsumeContinuously;

private:

	volatile bool m_stoped;
	DefaultMQPushConsumerImpl* m_pDefaultMQPushConsumerImpl;
	DefaultMQPushConsumer* m_pDefaultMQPushConsumer;
	MessageListenerOrderly* m_pMessageListener;
	std::string m_consumerGroup;
	MessageQueueLock m_messageQueueLock;

	kpr::ThreadPool* m_pConsumeExecutor;
	kpr::TimerThread_var m_scheduledExecutorService;
};

#endif
