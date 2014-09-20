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

#include "LockBatchBody.h"

LockBatchRequestBody::LockBatchRequestBody()
{
}

LockBatchRequestBody::~LockBatchRequestBody()
{
}

void LockBatchRequestBody::Encode(std::string& outData)
{

}

std::string LockBatchRequestBody::getConsumerGroup()
{
	return m_consumerGroup;
}

void LockBatchRequestBody::setConsumerGroup(const std::string& consumerGroup)
{
	m_consumerGroup = consumerGroup;
}

std::string LockBatchRequestBody::getClientId()
{
	return m_clientId;
}

void LockBatchRequestBody::setClientId(const std::string& clientId)
{
	m_clientId = clientId;
}

std::set<MessageQueue>& LockBatchRequestBody::getMqSet()
{
	return m_mqSet;
}

void LockBatchRequestBody::setMqSet(const std::set<MessageQueue>& mqSet)
{
	m_mqSet = mqSet;
}

LockBatchResponseBody::LockBatchResponseBody()
{
}

LockBatchResponseBody::~LockBatchResponseBody()
{
}

void LockBatchResponseBody::Encode(std::string& outData)
{

}

LockBatchResponseBody* LockBatchResponseBody::Decode(char* pData,int len)
{
	return new LockBatchResponseBody();
}

std::set<MessageQueue> LockBatchResponseBody::getLockOKMQSet()
{
	return m_lockOKMQSet;
}

void LockBatchResponseBody::setLockOKMQSet(const std::set<MessageQueue>& lockOKMQSet)
{
	m_lockOKMQSet = lockOKMQSet;
}
