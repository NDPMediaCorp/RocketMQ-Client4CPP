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
#include "ScopedLock.h"
#include "MQClientException.h"
#include "CommandCustomHeader.h"
#include "MQClientAPIImpl.h"

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
	kpr::ScopedLock<kpr::Mutex> lock(m_tableMutex);
	std::map<MessageQueue, AtomicLong*>::iterator it = m_offsetTable.find(mq);
	if (it != m_offsetTable.end())
	{
		if (increaseOnly)
		{
			MixAll::compareAndIncreaseOnly(*it->second,offset);
		}
		else
		{
			it->second->Set(offset);
		}
	}
	else
	{
		m_offsetTable[mq] = new AtomicLong(offset);
	}
}

long long RemoteBrokerOffsetStore::readOffset(MessageQueue& mq, ReadOffsetType type)
{
		switch (type)
		{
		case MEMORY_FIRST_THEN_STORE:
		case READ_FROM_MEMORY: 
			{
				kpr::ScopedLock<kpr::Mutex> lock(m_tableMutex);
				std::map<MessageQueue, AtomicLong*>::iterator it = m_offsetTable.find(mq);
				if (it != m_offsetTable.end())
				{
					return it->second->Get();
				}
				else 
				{
					if (READ_FROM_MEMORY == type)
					{
						return -1;
					}
				}

			}
		case READ_FROM_STORE:
			{
				try
				{
					long long brokerOffset = fetchConsumeOffsetFromBroker(mq);
					if (brokerOffset>=0)
					{
						updateOffset(mq, brokerOffset, false);
					}
					
					return brokerOffset;
				}
				// 当前订阅组在服务器没有对应的Offset
				catch (MQClientException& e)
				{
					return -1;
				}
			}
		default:
			break;
		}

	return -1;
}

void RemoteBrokerOffsetStore::persistAll(std::set<MessageQueue>& mqs)
{
	if (mqs.empty())
	{
		return;
	}

	std::map<MessageQueue, long long> tmp;

	long long times = m_storeTimesTotal++;
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_tableMutex);
		std::map<MessageQueue, AtomicLong*>::iterator it = m_offsetTable.begin();
		for (;it!=m_offsetTable.end();)
		{
			if (mqs.find(it->first)!=mqs.end())
			{
				tmp[it->first] = it->second->Get();
				it++;
			}
			else
			{
				std::map<MessageQueue, AtomicLong*>::iterator itTmp = it;
				it++;
				m_offsetTable.erase(itTmp);
			}
		}
	}

	std::map<MessageQueue, long long>::iterator it = tmp.begin();
	for(;it != tmp.end();it++)
	{
		try
		{
			updateConsumeOffsetToBroker(it->first, it->second);
			// 每隔1分钟打印一次消费进度
			if ((times % 12) == 0)
			{
				//log.info("Group: {} ClientId: {} updateConsumeOffsetToBroker {} {}", //
				//	this.groupName,//
				//	this.mQClientFactory.getClientId(),//
				//	mq, //
				//	offset.get());
			}
		}
		catch (MQClientException& e)
		{
			//log.error("updateConsumeOffsetToBroker exception, " + mq.toString(), e);
		}
	}
}

void RemoteBrokerOffsetStore::persist(MessageQueue& mq)
{
	long long offset;
	bool find = false;
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_tableMutex);
		std::map<MessageQueue, AtomicLong*>::iterator it = m_offsetTable.find(mq);
		if (it != m_offsetTable.end())
		{
			offset = it->second->Get();
		}
	}

	if (find)
	{
		try
		{
			updateConsumeOffsetToBroker(mq, offset);
			//log.debug("updateConsumeOffsetToBroker {} {}", mq, offset.get());
		}
		catch (MQClientException& e)
		{
			//log.error("updateConsumeOffsetToBroker exception, " + mq.toString(), e);
		}
	}
}

void RemoteBrokerOffsetStore::updateConsumeOffsetToBroker(const MessageQueue& mq, long long offset)
{
	FindBrokerResult findBrokerResult = m_pMQClientFactory->findBrokerAddressInAdmin(mq.getBrokerName());
	
	if (findBrokerResult.brokerAddr.empty())
	{
		// TODO 此处可能对Name Server压力过大，需要调优
		m_pMQClientFactory->updateTopicRouteInfoFromNameServer(mq.getTopic());
		findBrokerResult = m_pMQClientFactory->findBrokerAddressInAdmin(mq.getBrokerName());
	}

	if (!findBrokerResult.brokerAddr.empty())
	{
		UpdateConsumerOffsetRequestHeader* requestHeader = new UpdateConsumerOffsetRequestHeader();
		requestHeader->topic = mq.getTopic();
		requestHeader->consumerGroup = m_groupName;
		requestHeader->queueId = mq.getQueueId();
		requestHeader->commitOffset = offset;

		// 使用oneway形式，原因是服务器在删除文件时，这个调用可能会超时
		m_pMQClientFactory->getMQClientAPIImpl()->updateConsumerOffsetOneway(
			findBrokerResult.brokerAddr, requestHeader, 1000 * 5);
	}
	//else {
	//	throw new MQClientException("The broker[" + mq.getBrokerName() + "] not exist", null);
	//}
}

long long RemoteBrokerOffsetStore::fetchConsumeOffsetFromBroker(MessageQueue& mq)
{
	FindBrokerResult findBrokerResult = m_pMQClientFactory->findBrokerAddressInAdmin(mq.getBrokerName());

	if (findBrokerResult.brokerAddr.empty())
	{
		// TODO 此处可能对Name Server压力过大，需要调优
		m_pMQClientFactory->updateTopicRouteInfoFromNameServer(mq.getTopic());
		findBrokerResult = m_pMQClientFactory->findBrokerAddressInAdmin(mq.getBrokerName());
	}

	if (!findBrokerResult.brokerAddr.empty())
	{
		QueryConsumerOffsetRequestHeader* requestHeader = new QueryConsumerOffsetRequestHeader();
		requestHeader->topic = mq.getTopic();
		requestHeader->consumerGroup = m_groupName;
		requestHeader->queueId = mq.getQueueId();

		return m_pMQClientFactory->getMQClientAPIImpl()->queryConsumerOffset(
			findBrokerResult.brokerAddr, requestHeader, 1000 * 5);
	}
	//else {
	//	throw new MQClientException("The broker[" + mq.getBrokerName() + "] not exist", null);
	//}
	return -1;
}

void RemoteBrokerOffsetStore::removeOffset(MessageQueue& mq) 
{
	kpr::ScopedLock<kpr::Mutex> lock(m_tableMutex);
	m_offsetTable.erase(mq);
}
