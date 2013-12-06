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
#if!defined __SENDMESSAGEHOOK_H__
#define __SENDMESSAGEHOOK_H__

#include <string>
#include "Message.h"
#include "MQClientException.h"
#include "RocketMQClient.h"

class ROCKETMQCLIENT_API SendMessageContext
{
public:
	std::string producerGroup;
	Message msg;
	MessageQueue mq;
	std::string brokerAddr;
	CommunicationMode communicationMode;
	SendResult sendResult;
	MQException* pException;
	void* pArg;
};

class ROCKETMQCLIENT_API SendMessageHook
{
public:
	virtual ~SendMessageHook() {}
	virtual std::string hookName()=0;
	virtual void sendMessageBefore(const SendMessageContext& context)=0;
	virtual void sendMessageAfter(const SendMessageContext& context)=0;
};

#endif
