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

#include "RemoteBrokerOffsetStore.h"
#include "MQClientFactory.h"

RemoteBrokerOffsetStore::RemoteBrokerOffsetStore(MQClientFactory* pMQClientFactory, const std::string& groupName) 
{
	m_pMQClientFactory = pMQClientFactory;
	m_groupName = groupName;
}

void RemoteBrokerOffsetStore::load()
{

}

void RemoteBrokerOffsetStore::updateOffset(MessageQueue& mq, long long offset, bool increaseOnly)
{

}

long long RemoteBrokerOffsetStore::readOffset(MessageQueue& mq, ReadOffsetType type)
{
	return -1;
}

void RemoteBrokerOffsetStore::persistAll(std::set<MessageQueue>& mqs)
{

}

void RemoteBrokerOffsetStore::persist(MessageQueue& mq)
{
}

void RemoteBrokerOffsetStore::updateConsumeOffsetToBroker(MessageQueue& mq, long long offset)
{

}

long long RemoteBrokerOffsetStore::fetchConsumeOffsetFromBroker(MessageQueue& mq)
{
	return 0;
}

void RemoteBrokerOffsetStore::removeOffset(MessageQueue& mq) 
{
	m_offsetTable.erase(mq);
}
