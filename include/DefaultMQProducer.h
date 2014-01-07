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

#if!defined __DEFAULTMQPRODUCER_H__
#define __DEFAULTMQPRODUCER_H__

#include <string>
#include <list>
#include <vector>
#include <ClientConfig.h>
#include <MQProducer.h>
#include <SendResult.h>
#include "RocketMQClient.h"

class MessageQueue;
class MessageQueueSelector;
class MQClientException;
class Message;
class DefaultMQProducerImpl;

/**
* 消息生产者，适合使用spring初始化
*
*/
class ROCKETMQCLIENT_API DefaultMQProducer : public ClientConfig ,public MQProducer
{
public:
	DefaultMQProducer();
	DefaultMQProducer(const std::string& producerGroup);

	//begin MQProducer
	void start();
	void shutdown();

	std::vector<MessageQueue>* fetchPublishMessageQueues(const std::string& topic);

	SendResult send(Message& msg);
	SendResult send(Message& msg, MessageQueue& mq);
	SendResult send(Message& msg, MessageQueueSelector* selector, void* arg);
	void send(Message& msg, SendCallback* pSendCallback);
	void send(Message& msg, MessageQueue& mq, SendCallback* pSendCallback);
	void send(Message& msg, MessageQueueSelector* selector, void* arg, SendCallback* pSendCallback);
	void sendOneway(Message& msg);
	void sendOneway(Message& msg, MessageQueue& mq);
	void sendOneway(Message& msg, MessageQueueSelector* selector, void* arg);

	TransactionSendResult sendMessageInTransaction(Message& msg,
													LocalTransactionExecuter* pTranExecuter,
													void* arg);
	//end MQProducer

	void createTopic(const std::string& key, const std::string& newTopic, int queueNum) ;
	long long searchOffset(const MessageQueue& mq, long long timestamp);
	long long maxOffset(const MessageQueue& mq);
	long long minOffset(const MessageQueue& mq);
	long long earliestMsgStoreTime(const MessageQueue& mq);
	MessageExt viewMessage(const std::string& msgId) ;
	QueryResult queryMessage(const std::string& topic,
							 const std::string& key,
							 int maxNum,
							 long long begin,
							 long long end);

	std::string getProducerGroup();
	void setProducerGroup(const std::string& producerGroup);

	std::string getCreateTopicKey();
	void setCreateTopicKey(const std::string& createTopicKey);

	int getSendMsgTimeout();
	void setSendMsgTimeout(int sendMsgTimeout) ;

	int getCompressMsgBodyOverHowmuch();
	void setCompressMsgBodyOverHowmuch(int compressMsgBodyOverHowmuch);

	int getCompressLevel();
	void setCompressLevel(int compressLevel);

	bool isRetryAnotherBrokerWhenNotStoreOK();
	void setRetryAnotherBrokerWhenNotStoreOK(bool retryAnotherBrokerWhenNotStoreOK);

	int getMaxMessageSize();
	void setMaxMessageSize(int maxMessageSize);

	int getDefaultTopicQueueNums();
	void setDefaultTopicQueueNums(int defaultTopicQueueNums);

	DefaultMQProducerImpl* getDefaultMQProducerImpl();

protected:
	DefaultMQProducerImpl* m_pDefaultMQProducerImpl;

private:
	int m_defaultTopicQueueNums;///< 发送消息，自动创建Topic时，默认队列数
	int m_sendMsgTimeout;///< 发送消息超时，不建议修改
	int m_compressMsgBodyOverHowmuch;///< Message Body大小超过阀值，则压缩
	int m_maxMessageSize;///< 最大消息大小，默认512K

	std::string m_producerGroup;///< 一般发送同样消息的Producer，归为同一个Group，应用必须设置，并保证命名唯一
	std::string m_createTopicKey;///< 支持在发送消息时，如果Topic不存在，自动创建Topic，但是要指定Key
	
	// 消息已经成功写入Master，但是刷盘超时或者同步到Slave失败，则尝试重试另一个Broker，不建议修改默认值<br>
	//顺序消息无效	
	bool m_retryAnotherBrokerWhenNotStoreOK;
	int m_compressLevel;
};

#endif
