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

#include "RebalanceImpl.h"
#include "AllocateMessageQueueStrategy.h"
#include "MQClientFactory.h"

RebalanceImpl::RebalanceImpl(const std::string& consumerGroup, 
	MessageModel messageModel,
	AllocateMessageQueueStrategy* pAllocateMessageQueueStrategy, 
	MQClientFactory* pMQClientFactory)
	:m_consumerGroup(consumerGroup),
	m_messageModel (messageModel),
	m_pAllocateMessageQueueStrategy (pAllocateMessageQueueStrategy),
	m_pMQClientFactory( pMQClientFactory)
{
	
}

RebalanceImpl::~RebalanceImpl()
{
}

void RebalanceImpl::unlock(MessageQueue& mq, bool oneway)
{
	//TODO
}

void RebalanceImpl::unlockAll(bool oneway)
{
	//TODO
}

bool RebalanceImpl::lock(MessageQueue& mq)
{
	//TODO
	return false;
}

void RebalanceImpl::lockAll()
{
	//TODO
}

void RebalanceImpl::doRebalance()
{
	//TODO
}

std::map<std::string, SubscriptionData>& RebalanceImpl::getSubscriptionInner()
{
	return m_subscriptionInner;
}

std::map<MessageQueue, ProcessQueue>& RebalanceImpl::getProcessQueueTable()
{
	return m_processQueueTable;
}

std::map<std::string, std::set<MessageQueue> >& RebalanceImpl::getTopicSubscribeInfoTable()
{
	return m_topicSubscribeInfoTable;
}

std::string& RebalanceImpl::getConsumerGroup()
{
	return m_consumerGroup;
}

void RebalanceImpl::setConsumerGroup(const std::string& consumerGroup)
{
	m_consumerGroup = consumerGroup;
}

MessageModel RebalanceImpl::getMessageModel()
{
	return m_messageModel;
}

void RebalanceImpl::setMessageModel(MessageModel messageModel)
{
	m_messageModel = messageModel;
}

AllocateMessageQueueStrategy* RebalanceImpl::getAllocateMessageQueueStrategy()
{
	return m_pAllocateMessageQueueStrategy;
}

void RebalanceImpl::setAllocateMessageQueueStrategy(AllocateMessageQueueStrategy* pAllocateMessageQueueStrategy)
{
	m_pAllocateMessageQueueStrategy = pAllocateMessageQueueStrategy;
}

MQClientFactory* RebalanceImpl::getmQClientFactory()
{
	return m_pMQClientFactory;
}

void RebalanceImpl::setmQClientFactory(MQClientFactory* pMQClientFactory)
{
	m_pMQClientFactory = pMQClientFactory;
}

std::map<std::string, std::set<MessageQueue> > RebalanceImpl::buildProcessQueueTableByBrokerName()
{
	//TODO
	std::map<std::string, std::set<MessageQueue> > result ;

	return result;
}

void RebalanceImpl::rebalanceByTopic(const std::string& topic)
{
	//TODO
	switch (m_messageModel)
	{
	case BROADCASTING:
		{
			break;
		}
	case CLUSTERING:
		{
			break;
		}
	default:
		break;
	}
}

bool RebalanceImpl::updateProcessQueueTableInRebalance(const std::string& topic, std::set<MessageQueue>& mqSet)
{
	//TODO
	bool changed = false;

	return changed;
}

void RebalanceImpl::truncateMessageQueueNotMyTopic()
{
	//TODO
}
