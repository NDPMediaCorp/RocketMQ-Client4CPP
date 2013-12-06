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
#if!defined __SENDRESULT_H__
#define __SENDRESULT_H__

#include "MessageQueue.h"
#include "RocketMQClient.h"

/**
* 这4种状态都表示消息已经成功到达Master
*
*/
enum SendStatus
{
	// 消息发送成功
	SEND_OK,
	// 消息发送成功，但是服务器刷盘超时，消息已经进入服务器队列，只有此时服务器宕机，消息才会丢失
	FLUSH_DISK_TIMEOUT,
	// 消息发送成功，但是服务器同步到Slave时超时，消息已经进入服务器队列，只有此时服务器宕机，消息才会丢失
	FLUSH_SLAVE_TIMEOUT,
	// 消息发送成功，但是此时slave不可用，消息已经进入服务器队列，只有此时服务器宕机，消息才会丢失
	SLAVE_NOT_AVAILABLE
};

/**
* 发送消息结果
*
*/
class ROCKETMQCLIENT_API SendResult
{
public:
	SendResult();
	SendResult(const SendStatus& sendStatus,
		const std::string&  msgId,
		MessageQueue& messageQueue,
		long long queueOffset,
		std::string&  projectGroupPrefix);

	const std::string&  getMsgId();
	void setMsgId(const std::string&  msgId);
	SendStatus getSendStatus();
	void setSendStatus(const SendStatus& sendStatus);
	MessageQueue& getMessageQueue();
	void setMessageQueue(MessageQueue& messageQueue);
	long long getQueueOffset();
	void setQueueOffset(long long queueOffset);

private:
	SendStatus m_sendStatus;
	std::string m_msgId;
	MessageQueue m_messageQueue;
	long long m_queueOffset;
};

enum LocalTransactionState
{
	COMMIT_MESSAGE,// 提交事务
	ROLLBACK_MESSAGE,// 回滚事务
	UNKNOW,
};

/**
* 发送事务消息返回结果
*
*/
class ROCKETMQCLIENT_API TransactionSendResult : public SendResult
{
public:
	TransactionSendResult();
	LocalTransactionState getLocalTransactionState();
	void setLocalTransactionState(LocalTransactionState localTransactionState);

private:
	LocalTransactionState m_localTransactionState;
};

#endif
