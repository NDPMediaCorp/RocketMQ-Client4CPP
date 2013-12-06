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

#include "LocalFileOffsetStore.h"

#include "MQClientFactory.h"

LocalFileOffsetStore::LocalFileOffsetStore(MQClientFactory* pMQClientFactory, 
	const std::string& groupName) 
{
	m_pMQClientFactory = pMQClientFactory;
	m_groupName = groupName;
	m_storePath = m_pMQClientFactory->getClientId() + "/"+ m_groupName +"/offsets.json";
}

void  LocalFileOffsetStore::load() 
{
}


void  LocalFileOffsetStore::updateOffset(MessageQueue& mq, long long offset, bool increaseOnly)
{

}

long long  LocalFileOffsetStore::readOffset(MessageQueue& mq, ReadOffsetType type)
{

	return -1;
}

void  LocalFileOffsetStore::persistAll(std::set<MessageQueue>& mqs)
{

}

void  LocalFileOffsetStore::persist(MessageQueue& mq)
{
}

void  LocalFileOffsetStore::removeOffset(MessageQueue& mq)
{
	// 消费进度存储到Consumer本地时暂不做 offset 清理
}
