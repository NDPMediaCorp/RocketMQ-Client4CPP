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
#include "MixAll.h"
#include "LockBatchBody.h"
#include "MQClientAPIImpl.h"
#include "KPRUtil.h"
#include "ScopedLock.h"

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
	FindBrokerResult findBrokerResult =
		m_pMQClientFactory->findBrokerAddressInSubscribe(mq.getBrokerName(), MixAll::MASTER_ID, true);
	if (!findBrokerResult.brokerAddr.empty())
	{
		UnlockBatchRequestBody* requestBody = new UnlockBatchRequestBody();
		requestBody->setConsumerGroup(m_consumerGroup);
		requestBody->setClientId(m_pMQClientFactory->getClientId());
		requestBody->getMqSet().insert(mq);

		try
		{
			m_pMQClientFactory->getMQClientAPIImpl()->unlockBatchMQ(findBrokerResult.brokerAddr,
				requestBody, 1000, oneway);
		}
		catch (...)
		{
			//TODO log.error("unlockBatchMQ exception, " + mq, e);
		}
	}
}

void RebalanceImpl::unlockAll(bool oneway)
{
	std::map<std::string, std::set<MessageQueue> > brokerMqs = buildProcessQueueTableByBrokerName();
	std::map<std::string, std::set<MessageQueue> >::iterator it = brokerMqs.begin();

	for (; it != brokerMqs.end(); it++)
	{
		std::string brokerName = it->first;
		std::set<MessageQueue> mqs = it->second;

		if (mqs.empty())
		{
			continue;
		}

		FindBrokerResult findBrokerResult =
			m_pMQClientFactory->findBrokerAddressInSubscribe(brokerName, MixAll::MASTER_ID, true);

		if (!findBrokerResult.brokerAddr.empty())
		{
			UnlockBatchRequestBody* requestBody = new UnlockBatchRequestBody();
			requestBody->setConsumerGroup(m_consumerGroup);
			requestBody->setClientId(m_pMQClientFactory->getClientId());
			requestBody->setMqSet(mqs);

			try
			{
				m_pMQClientFactory->getMQClientAPIImpl()->unlockBatchMQ(findBrokerResult.brokerAddr,
					requestBody, 1000, oneway);

				kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
				std::set<MessageQueue>::iterator itm = mqs.begin();
				for (;itm!=mqs.end();itm++)
				{
					std::map<MessageQueue, ProcessQueue*>::iterator itp = m_processQueueTable.find(*itm);
					if (itp != m_processQueueTable.end())
					{
						itp->second->setLocked(false);
						//TODO log.info("the message queue unlock OK, Group: {} {}", this.consumerGroup, mq);
					}
				}
			}
			catch (...)
			{
				//TODO log.error("unlockBatchMQ exception, " + mqs, e);
			}
		}
	}
}

bool RebalanceImpl::lock(MessageQueue& mq)
{
	FindBrokerResult findBrokerResult =
		m_pMQClientFactory->findBrokerAddressInSubscribe(mq.getBrokerName(), MixAll::MASTER_ID, true);
	if (!findBrokerResult.brokerAddr.empty())
	{
		LockBatchRequestBody* requestBody = new LockBatchRequestBody();
		requestBody->setConsumerGroup(m_consumerGroup);
		requestBody->setClientId(m_pMQClientFactory->getClientId());
		requestBody->getMqSet().insert(mq);

		try
		{
			std::set<MessageQueue> lockedMq =
				m_pMQClientFactory->getMQClientAPIImpl()->lockBatchMQ(
				findBrokerResult.brokerAddr, requestBody, 1000);

			std::set<MessageQueue>::iterator it = lockedMq.begin();
			for (; it != lockedMq.end(); it++)
			{
				kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
				MessageQueue mmqq = *it;
				std::map<MessageQueue, ProcessQueue*>::iterator itt = m_processQueueTable.find(mmqq);
				if (itt != m_processQueueTable.end())
				{
					itt->second->setLocked(true);
					itt->second->setLastLockTimestamp(GetCurrentTimeMillis());
				}
			}

			it = lockedMq.find(mq);
			if (it != lockedMq.end())
			{
				return true;
			}

			//TODO log.info("the message queue lock {}, {} {}",//
			//	(lockOK ? "OK" : "Failed"), //
			//	this.consumerGroup, //
			//	mq);
			return false;
		}
		catch (...)
		{
			//TODO log.error("lockBatchMQ exception, " + mq, e);
		}
	}

	return false;
}

void RebalanceImpl::lockAll()
{
	std::map<std::string, std::set<MessageQueue> > brokerMqs = buildProcessQueueTableByBrokerName();

	std::map<std::string, std::set<MessageQueue> >::iterator it = brokerMqs.begin();
	for (;it != brokerMqs.end();it++)
	{
		std::string brokerName = it->first;
		std::set<MessageQueue> mqs = it->second;

		if (mqs.empty())
		{
			continue;
		}

		FindBrokerResult findBrokerResult =
			m_pMQClientFactory->findBrokerAddressInSubscribe(brokerName, MixAll::MASTER_ID, true);
		if (!findBrokerResult.brokerAddr.empty())
		{
			LockBatchRequestBody* requestBody = new LockBatchRequestBody();
			requestBody->setConsumerGroup(m_consumerGroup);
			requestBody->setClientId(m_pMQClientFactory->getClientId());
			requestBody->setMqSet(mqs);

			try
			{
				std::set<MessageQueue> lockOKMQSet =
					m_pMQClientFactory->getMQClientAPIImpl()->lockBatchMQ(
					findBrokerResult.brokerAddr, requestBody, 1000);

				// 锁定成功的队列
				std::set<MessageQueue>::iterator its = lockOKMQSet.begin();
				for (;its != lockOKMQSet.end();its++)
				{
					kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
					MessageQueue mq = *its;
					std::map<MessageQueue, ProcessQueue*>::iterator itt = m_processQueueTable.find(mq);
					if (itt != m_processQueueTable.end())
					{
						ProcessQueue* processQueue = itt->second;
						if (!processQueue->isLocked())
						{
							//TODO log.info("the message queue locked OK, Group: {} {}", this.consumerGroup, mq);
						}

						processQueue->setLocked(true);
						processQueue->setLastLockTimestamp(GetCurrentTimeMillis());
					}
				}

				// 锁定失败的队列
				its = mqs.begin();
				for (;its != mqs.end();its++)
				{
					MessageQueue mq = *its;
					std::set<MessageQueue>::iterator itf = lockOKMQSet.find(mq);
					if (itf == lockOKMQSet.end())
					{
						kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
						std::map<MessageQueue, ProcessQueue*>::iterator itt = m_processQueueTable.find(mq);
						if (itt != m_processQueueTable.end())
						{
							itt->second->setLocked(false);
							//TODO log.warn("the message queue locked Failed, Group: {} {}", this.consumerGroup,
							//	mq);
						}
					}
				}
			}
			catch (...)
			{
				//TODO log.error("lockBatchMQ exception, " + mqs, e);
			}
		}
	}
}

void RebalanceImpl::doRebalance()
{
	std::map<std::string, SubscriptionData> subTable = getSubscriptionInner();
	std::map<std::string, SubscriptionData>::iterator it = subTable.begin();
	for (; it != subTable.end(); it++)
	{
		std::string topic = it->first;
		try
		{
			rebalanceByTopic(topic);
		}
		catch (...)
		{
			if (topic.find(MixAll::RETRY_GROUP_TOPIC_PREFIX) != 0)
			{
				//TODO log.warn("rebalanceByTopic Exception", e);
			}
		}
	}


	truncateMessageQueueNotMyTopic();
}

std::map<std::string, SubscriptionData>& RebalanceImpl::getSubscriptionInner()
{
	return m_subscriptionInner;
}

std::map<MessageQueue, ProcessQueue*>& RebalanceImpl::getProcessQueueTable()
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
	kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
	std::map<std::string, std::set<MessageQueue> > result ;
	std::map<MessageQueue, ProcessQueue*>::iterator it = m_processQueueTable.begin();

	for ( ; it != m_processQueueTable.end();)
	{
		MessageQueue mq = it->first;
		std::map<std::string, std::set<MessageQueue> >::iterator itm = result.find(mq.getBrokerName());
		if (itm == result.end())
		{
			std::set<MessageQueue> mqs ;
			mqs.insert(mq);
			result[mq.getBrokerName()] = mqs;
		}
		else
		{
			itm->second.insert(mq);
		}
	}

	return result;
}

void RebalanceImpl::rebalanceByTopic(const std::string& topic)
{
	switch (m_messageModel)
	{
	case BROADCASTING:
		{
			std::map<std::string, std::set<MessageQueue> >::iterator it = m_topicSubscribeInfoTable.find(topic);

			if (it != m_topicSubscribeInfoTable.end())
			{
				std::set<MessageQueue> mqSet = it->second;
				bool changed = updateProcessQueueTableInRebalance(topic, mqSet);
				if (changed)
				{
					messageQueueChanged(topic, mqSet, mqSet);
					//TODO log.info("messageQueueChanged {} {} {} {}",//
					//	consumerGroup,//
					//	topic,//
					//	mqSet,//
					//	mqSet);
				}
			}
			else
			{
				//TODO log.warn("doRebalance, {}, but the topic[{}] not exist.", consumerGroup, topic);
			}
			break;
		}
	case CLUSTERING:
		{
			std::map<std::string, std::set<MessageQueue> >::iterator it = m_topicSubscribeInfoTable.find(topic);

			if (it == m_topicSubscribeInfoTable.end())
			{
				if (topic.find(MixAll::RETRY_GROUP_TOPIC_PREFIX) !=0 )
				{
					Logger::get_logger()->warn("doRebalance, {}, but the topic[{}] not exist.", m_consumerGroup, topic);
				}
			}

			std::list<std::string> cidAll = m_pMQClientFactory->findConsumerIdList(topic, m_consumerGroup);

			if (cidAll.empty())
			{
				Logger::get_logger()->warn("doRebalance, {} {}, get consumer id list failed", m_consumerGroup, topic);
			}

			if (it != m_topicSubscribeInfoTable.end() && !cidAll.empty())
			{
				std::vector<MessageQueue> mqAll;
				std::set<MessageQueue> mqSet = it->second;
				std::set<MessageQueue>::iterator its = mqSet.begin();

				//set 本身已经排序
				for (; its != mqSet.end();its++)
				{
					mqAll.push_back(*its);
				}

				// 排序
				cidAll.sort();

				AllocateMessageQueueStrategy* strategy = m_pAllocateMessageQueueStrategy;

				// 执行分配算法
				std::vector<MessageQueue>* allocateResult;
				try
				{
					allocateResult = strategy->allocate(m_pMQClientFactory->getClientId(), mqAll, cidAll);
				}
				catch (...)
				{
					//TODO log.error("AllocateMessageQueueStrategy.allocate Exception", e);
				}

				std::set<MessageQueue> allocateResultSet;
				if (allocateResult != NULL)
				{
					for(size_t i=0;i<allocateResult->size();i++)
					{
						allocateResultSet.insert(allocateResult->at(i));
					}

					delete allocateResult;
				}

				// 更新本地队列
				bool changed = updateProcessQueueTableInRebalance(topic, allocateResultSet);
				if (changed)
				{
					//TODO	log.info("reblance result is [{}], ConsumerId is [{}], mqAll is[{}], cidAll is [{}]",
					//		allocateResult, this.mQClientFactory.getClientId(), mqAll, cidAll);

					messageQueueChanged(topic, mqSet, allocateResultSet);
					//TODO log.info("messageQueueChanged {} {} {} {}",//
					//	consumerGroup,//
					//	topic,//
					//	mqSet,//
					//	allocateResultSet);

					//log.info("messageQueueChanged consumerIdList: {}",//
					//	cidAll);
				}
			}
			break;
		}
	default:
		break;
	}
}

bool RebalanceImpl::updateProcessQueueTableInRebalance(const std::string& topic, std::set<MessageQueue>& mqSet)
{
	bool changed = false;

	// 将多余的队列删除
	std::map<MessageQueue, ProcessQueue*> tmp;
	{
		kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
		std::map<MessageQueue, ProcessQueue*>::iterator it = m_processQueueTable.begin();

		for ( ; it != m_processQueueTable.end();)
		{
			MessageQueue mq = it->first;
			if (mq.getTopic() == topic)
			{
				std::set<MessageQueue>::iterator its = mqSet.find(mq);
				if (its == mqSet.end())
				{
					changed = true;
					ProcessQueue* pq = it->second;
					std::map<MessageQueue, ProcessQueue*>::iterator ittmp = it;
					it++;
					m_processQueueTable.erase(ittmp);

					if (pq != NULL)
					{
						tmp[mq]=pq;
					}

				}
				else
				{
					it++;
				}
			}
			else
			{
				it++;
			}
		}
	}

	std::map<MessageQueue, ProcessQueue*>::iterator itTmp = tmp.begin();

	for ( ; itTmp != tmp.end();itTmp++)
	{
		ProcessQueue* pq = itTmp->second;
		MessageQueue mq = itTmp->first;
		pq->setDropped(true);
		removeUnnecessaryMessageQueue(mq, *pq);
		//TODO log.info("doRebalance, {}, remove unnecessary mq, {}",
		//	consumerGroup, mq);

		//TODO 怎么删除ProcessQueue？
	}

	tmp.clear();

	// 增加新增的队列
	std::list<PullRequest*> pullRequestList;

	std::set<MessageQueue>::iterator its = mqSet.begin();
	for (; its != mqSet.end(); its++)
	{
		
		MessageQueue mq = *its;
		bool find = false;
		{
			kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
			std::map<MessageQueue, ProcessQueue*>::iterator itm = m_processQueueTable.find(mq);
			if (itm != m_processQueueTable.end())
			{
				find = true;
			}
		}

		if (!find)
		{
			PullRequest* pullRequest = new PullRequest();
			pullRequest->setConsumerGroup(m_consumerGroup);
			pullRequest->setMessageQueue(new MessageQueue(mq.getTopic(),mq.getBrokerName(),mq.getQueueId()));
			pullRequest->setProcessQueue(new ProcessQueue());

			// 这个需要根据策略来设置
			long long nextOffset = computePullFromWhere(mq);
			if (nextOffset >= 0)
			{
				pullRequest->setNextOffset(nextOffset);
				pullRequestList.push_back(pullRequest);
				changed = true;
				kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
				m_processQueueTable[mq] = pullRequest->getProcessQueue();
				//TODO log.info("doRebalance, {}, add a new mq, {}", consumerGroup, mq);
			}
			else
			{
				// 等待此次Rebalance做重试
				//TODO log.warn("doRebalance, {}, add new mq failed, {}", consumerGroup, mq);
			}
		}
	}

	dispatchPullRequest(pullRequestList);

	return changed;
}

void RebalanceImpl::truncateMessageQueueNotMyTopic()
{
	std::map<std::string, SubscriptionData> subTable = getSubscriptionInner();
	kpr::ScopedLock<kpr::Mutex> lock(m_processQueueTableLock);
	std::map<MessageQueue, ProcessQueue*>::iterator it = m_processQueueTable.begin();

	for ( ; it != m_processQueueTable.end();)
	{
		MessageQueue mq = it->first;
		std::map<std::string, SubscriptionData>::iterator itt = subTable.find(mq.getTopic());

		if (itt == subTable.end())
		{
			ProcessQueue* pq = it->second;
			if (pq != NULL)
			{
				pq->setDropped(true);
				//TODO log.info("doRebalance, {}, truncateMessageQueueNotMyTopic remove unnecessary mq, {}",
				//	consumerGroup, mq);
			}
			std::map<MessageQueue, ProcessQueue*>::iterator ittmp = it;
			it++;
			m_processQueueTable.erase(ittmp);
		}
		else
		{
			it++;
		}
	}
}
