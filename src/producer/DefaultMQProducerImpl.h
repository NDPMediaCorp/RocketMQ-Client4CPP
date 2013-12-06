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

#if!defined __DEFAULTMQPRODUCERIMPL_H__
#define __DEFAULTMQPRODUCERIMPL_H__

#include <list>
#include <vector>
#include "MQProducerInner.h"
#include "QueryResult.h"
#include "ServiceState.h"
#include "CommunicationMode.h"
#include "SendResult.h"
#include "MQClientException.h"

class DefaultMQProducer;
class SendMessageHook;
class SendMessageContext;
class MessageQueue;
class MessageExt;
class SendCallback;
class MessageQueueSelector;
class MQClientFactory;
class MQClientException;
class RemotingException;
class MQBrokerException;
class InterruptedException;
class LocalTransactionExecuter;

/**
* 生产者默认实现
*
*/
class DefaultMQProducerImpl : public MQProducerInner
{
public:
	DefaultMQProducerImpl(DefaultMQProducer* pDefaultMQProducer);
	void initTransactionEnv();
	void destroyTransactionEnv();

	bool hasHook();
	void registerHook(SendMessageHook* pHook);
	void executeHookBefore(const SendMessageContext& context);
	void executeHookAfter(const SendMessageContext& context);

	void start();
	void start(bool startFactory);
	void shutdown();
	void shutdown(bool shutdownFactory);

	//父类接口实现
	std::set<std::string> getPublishTopicList();
	bool isPublishTopicNeedUpdate(const std::string& topic);

	void checkTransactionState(const std::string& addr,
							   const MessageExt& msg,
							   const CheckTransactionStateRequestHeader& checkRequestHeader);

	void updateTopicPublishInfo(const std::string& topic, TopicPublishInfo& info);
	virtual TransactionCheckListener* checkListener();
	//父类接口实现 end

	void createTopic(const std::string& key, const std::string& newTopic, int queueNum);

	std::vector<MessageQueue>* fetchPublishMessageQueues(const std::string& topic);

	long long searchOffset(const MessageQueue& mq, long long timestamp);
	long long maxOffset(const MessageQueue& mq);
	long long minOffset(const MessageQueue& mq);

	long long earliestMsgStoreTime(const MessageQueue& mq);

	MessageExt viewMessage(const std::string& msgId);
	QueryResult queryMessage(const std::string& topic,
							 const std::string& key,
							 int maxNum,
							 long long begin,
							 long long end);

	/**
	* DEFAULT ASYNC -------------------------------------------------------
	*/
	void send(Message& msg, SendCallback* sendCallback);

	/**
	* DEFAULT ONEWAY -------------------------------------------------------
	*/
	void sendOneway(Message& msg);

	/**
	* KERNEL SYNC -------------------------------------------------------
	*/
	SendResult send(Message& msg, MessageQueue& mq);

	/**
	* KERNEL ASYNC -------------------------------------------------------
	*/
	void send(Message& msg, MessageQueue& mq, SendCallback* sendCallback);

	/**
	* KERNEL ONEWAY -------------------------------------------------------
	*/
	void sendOneway(Message& msg, MessageQueue& mq);

	/**
	* SELECT SYNC -------------------------------------------------------
	*/
	SendResult send(Message& msg, MessageQueueSelector* selector, void* arg);

	/**
	* SELECT ASYNC -------------------------------------------------------
	*/
	void send(Message& msg, MessageQueueSelector* selector, void* arg, SendCallback* sendCallback);

	/**
	* SELECT ONEWAY -------------------------------------------------------
	*/
	void sendOneway(Message& msg, MessageQueueSelector* selector, void* arg);

	TransactionSendResult sendMessageInTransaction(Message& msg, LocalTransactionExecuter* tranExecuter, void* arg);

	/**
	* DEFAULT SYNC -------------------------------------------------------
	*/
	SendResult send(Message& msg);

	std::map<std::string, TopicPublishInfo> getTopicPublishInfoTable();

	MQClientFactory* getmQClientFactory();

	int getZipCompressLevel();
	void setZipCompressLevel(int zipCompressLevel);

private:
	SendResult sendSelectImpl(Message& msg,
							  MessageQueueSelector* selector,
							  void* pArg,
							  CommunicationMode communicationMode,
							  SendCallback* sendCallback);

	SendResult sendDefaultImpl(Message& msg,
							   CommunicationMode communicationMode,
							   SendCallback* pSendCallback);

	SendResult sendKernelImpl(Message& msg,
							  const MessageQueue& mq,
							  CommunicationMode communicationMode,
							  SendCallback* pSendCallback);

	void endTransaction(SendResult sendResult,
						LocalTransactionState localTransactionState,
						MQClientException localException);

	void makeSureStateOK();
	void checkConfig();

	/**
	* 尝试寻找Topic路由信息，如果没有则到Name Server上找，再没有，则取默认Topic
	*/
	TopicPublishInfo tryToFindTopicPublishInfo(const std::string& topic) ;

	bool tryToCompressMessage(Message& msg);

protected:
	//TODO 事务相关队列 及 检测线程

private:
	int m_zipCompressLevel;// 消息压缩level，默认5

	DefaultMQProducer* m_pDefaultMQProducer;
	std::map<std::string, TopicPublishInfo> m_topicPublishInfoTable;
	ServiceState m_serviceState;
	MQClientFactory* m_pMQClientFactory;
	std::list<SendMessageHook*> m_hookList;//发送每条消息会回调
};

#endif
