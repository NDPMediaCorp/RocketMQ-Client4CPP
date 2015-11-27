/**
* Copyright (C) 2013 kangliqiang ,kangliq@163.com
*
* Licensed under the Apache License, Version 2.0 (the "License")
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

#include "DefaultMQProducerImpl.h"
#include "DefaultMQProducer.h"
#include "MessageExt.h"
#include "QueryResult.h"
#include "TopicPublishInfo.h"
#include "MQClientException.h"
#include "LocalTransactionExecuter.h"
#include "SendMessageHook.h"
#include "MQClientManager.h"
#include "MQClientFactory.h"
#include "Validators.h"
#include "MQAdminImpl.h"
#include "MQClientAPIImpl.h"
#include "MessageSysFlag.h"
#include "CommandCustomHeader.h"
#include "KPRUtil.h"
#include "MessageDecoder.h"
#include "MQProtos.h"
#include "RemotingCommand.h"
#include "UtilAll.h"

DefaultMQProducerImpl::DefaultMQProducerImpl(DefaultMQProducer* pDefaultMQProducer)
	:m_pDefaultMQProducer(pDefaultMQProducer),
	m_serviceState(CREATE_JUST)
{

}

void DefaultMQProducerImpl::initTransactionEnv()
{
	//TODO
}

void DefaultMQProducerImpl::destroyTransactionEnv()
{
	//TODO
}

bool DefaultMQProducerImpl::hasHook()
{
	return !m_hookList.empty();
}

void DefaultMQProducerImpl::registerHook(SendMessageHook* pHook)
{
	m_hookList.push_back(pHook);
}

void DefaultMQProducerImpl::executeHookBefore(const SendMessageContext& context)
{
	std::list<SendMessageHook*>::iterator it = m_hookList.begin();
	for(; it!=m_hookList.end(); it++)
	{
		try
		{
			(*it)->sendMessageBefore(context);
		}
		catch (...)
		{
		}
	}
}

void DefaultMQProducerImpl::executeHookAfter(const SendMessageContext& context)
{
	std::list<SendMessageHook*>::iterator it = m_hookList.begin();
	for(; it!=m_hookList.end(); it++)
	{
		try
		{
			(*it)->sendMessageAfter(context);
		}
		catch (...)
		{
		}
	}
}

void DefaultMQProducerImpl::start()
{
	start(true);
}

void DefaultMQProducerImpl::start(bool startFactory)
{
	Logger::get_logger()->info("Starting DefaultMQProducerImpl");
	switch (m_serviceState)
	{
	case CREATE_JUST:
	{
		m_serviceState = START_FAILED;

		checkConfig();

		m_pMQClientFactory = MQClientManager::getInstance()->getAndCreateMQClientFactory(*m_pDefaultMQProducer);

		bool registerOK = m_pMQClientFactory->registerProducer(m_pDefaultMQProducer->getProducerGroup(), this);
		if (!registerOK)
		{
			m_serviceState = CREATE_JUST;
			std::string msg("The producer group[");
			msg.append(m_pDefaultMQProducer->getProducerGroup())
			   .append("] has been created before, specify another name please.");
			Logger::get_logger()->error(msg);
			THROW_MQEXCEPTION(MQClientException, msg, -1);
		}

		// 默认Topic注册
		m_topicPublishInfoTable[m_pDefaultMQProducer->getCreateTopicKey()] = TopicPublishInfo();

		if (startFactory)
		{
			m_pMQClientFactory->start();
		}

		m_serviceState = RUNNING;
		Logger::get_logger()->info("DefaultMQProducerImpl started.");
	}
	break;
	case RUNNING:
		Logger::get_logger()->error("This client is already running.");
		THROW_MQEXCEPTION(MQClientException,"The producer service state not OK, maybe started once, ",-1);
	case START_FAILED:
		Logger::get_logger()->error("This client failed to start previously.");
		THROW_MQEXCEPTION(MQClientException,"The producer service state not OK, maybe started once, ",-1);
	case SHUTDOWN_ALREADY:
		Logger::get_logger()->error("This client has been shutted down.");
		THROW_MQEXCEPTION(MQClientException,"The producer service state not OK, maybe started once, ",-1);
	default:
		break;
	}

	m_pMQClientFactory->sendHeartbeatToAllBrokerWithLock();
}

void DefaultMQProducerImpl::shutdown()
{
	shutdown(true);
}

void DefaultMQProducerImpl::shutdown(bool shutdownFactory)
{
	switch (m_serviceState)
	{
	case CREATE_JUST:
		break;
	case RUNNING:
		m_pMQClientFactory->unregisterProducer(m_pDefaultMQProducer->getProducerGroup());
		if (shutdownFactory)
		{
			m_pMQClientFactory->shutdown();
		}

		m_serviceState = SHUTDOWN_ALREADY;
		break;
	case SHUTDOWN_ALREADY:
		break;
	default:
		break;
	}
}

//父类接口实现
std::set<std::string> DefaultMQProducerImpl::getPublishTopicList()
{
	std::set<std::string> toplist;

	std::map<std::string, TopicPublishInfo>::iterator it = m_topicPublishInfoTable.begin();

	for(; it!=m_topicPublishInfoTable.end(); it++)
	{
		toplist.insert(it->first);
	}

	return toplist;
}

bool DefaultMQProducerImpl::isPublishTopicNeedUpdate(const std::string& topic)
{
	std::map<std::string, TopicPublishInfo>::iterator it = m_topicPublishInfoTable.find(topic);
	if (it!=m_topicPublishInfoTable.end())
	{
		return !it->second.ok();
	}

	return true;
}

void DefaultMQProducerImpl::checkTransactionState(const std::string& addr, //
		const MessageExt& msg, //
		const CheckTransactionStateRequestHeader& checkRequestHeader)
{
	//TODO
}

void DefaultMQProducerImpl::updateTopicPublishInfo(const std::string& topic,
		 TopicPublishInfo& info)
{
	std::map<std::string, TopicPublishInfo>::iterator it = m_topicPublishInfoTable.find(topic);

	if (it!=m_topicPublishInfoTable.end())
	{
		info.getSendWhichQueue()=it->second.getSendWhichQueue();
	}

	m_topicPublishInfoTable[topic]=info;
}

//父类接口实现 end

void DefaultMQProducerImpl::createTopic(const std::string& key, const std::string& newTopic, int queueNum)
{
	makeSureStateOK();
	// topic 有效性检查
	Validators::checkTopic(newTopic);

	m_pMQClientFactory->getMQAdminImpl()->createTopic(key, newTopic, queueNum);
}

std::vector<MessageQueue>* DefaultMQProducerImpl::fetchPublishMessageQueues(const std::string& topic)
{
	makeSureStateOK();
	return m_pMQClientFactory->getMQAdminImpl()->fetchPublishMessageQueues(topic);
}

long long DefaultMQProducerImpl::searchOffset(const MessageQueue& mq, long long timestamp)
{
	makeSureStateOK();
	return m_pMQClientFactory->getMQAdminImpl()->searchOffset(mq,timestamp);
}

long long DefaultMQProducerImpl::maxOffset(const MessageQueue& mq)
{
	makeSureStateOK();
	return m_pMQClientFactory->getMQAdminImpl()->maxOffset(mq);
}

long long DefaultMQProducerImpl::minOffset(const MessageQueue& mq)
{
	makeSureStateOK();
	return m_pMQClientFactory->getMQAdminImpl()->minOffset(mq);
}

long long DefaultMQProducerImpl::earliestMsgStoreTime(const MessageQueue& mq)
{
	makeSureStateOK();
	return m_pMQClientFactory->getMQAdminImpl()->earliestMsgStoreTime(mq);
}

MessageExt DefaultMQProducerImpl::viewMessage(const std::string& msgId)
{
	makeSureStateOK();
	return m_pMQClientFactory->getMQAdminImpl()->viewMessage(msgId);
}

QueryResult DefaultMQProducerImpl::queryMessage(const std::string& topic, const std::string& key, int maxNum, long long begin, long long end)
{
	makeSureStateOK();
	return m_pMQClientFactory->getMQAdminImpl()->queryMessage(topic,key,maxNum,begin,end);
}

SendResult DefaultMQProducerImpl::send(Message& msg)
{
	return sendDefaultImpl(msg, SYNC, NULL);
}

void DefaultMQProducerImpl::send(Message& msg, SendCallback* pSendCallback)
{
	try
	{
		sendDefaultImpl(msg, ASYNC, pSendCallback);
	}
	catch (MQBrokerException e)
	{
		
	}
}

void DefaultMQProducerImpl::sendOneway(Message& msg)
{
	try
	{
		sendDefaultImpl(msg, ONEWAY,NULL);
	}
	catch (MQBrokerException e)
	{

	}
}

SendResult DefaultMQProducerImpl::send(Message& msg, MessageQueue& mq)
{
	// 有效性检查
	makeSureStateOK();
	Validators::checkMessage(msg, m_pDefaultMQProducer->getMaxMessageSize());

	if (msg.getTopic()!=mq.getTopic())
	{
		THROW_MQEXCEPTION(MQClientException,"message's topic not equal mq's topic",-1);
	}

	return sendKernelImpl(msg, mq, SYNC, NULL);
}

void DefaultMQProducerImpl::send(Message& msg, MessageQueue& mq, SendCallback* pSendCallback)
{
	// 有效性检查
	makeSureStateOK();
	Validators::checkMessage(msg, m_pDefaultMQProducer->getMaxMessageSize());

	if (msg.getTopic()!=mq.getTopic())
	{
		THROW_MQEXCEPTION(MQClientException,"message's topic not equal mq's topic",-1);
	}

	try
	{
		sendKernelImpl(msg, mq, ASYNC, pSendCallback);
	}
	catch(...)
	{

	}
}

void DefaultMQProducerImpl::sendOneway(Message& msg, MessageQueue& mq)
{
	// 有效性检查
	makeSureStateOK();
	Validators::checkMessage(msg, m_pDefaultMQProducer->getMaxMessageSize());

	if (msg.getTopic()!=mq.getTopic())
	{
		THROW_MQEXCEPTION(MQClientException,"message's topic not equal mq's topic",-1);
	}

	try
	{
		sendKernelImpl(msg, mq, ONEWAY, NULL);
	}
	catch(...)
	{

	}
}

SendResult DefaultMQProducerImpl::send(Message& msg, MessageQueueSelector* pSelector, void* arg)
{
	return sendSelectImpl(msg, pSelector, arg, SYNC, NULL);
}

void DefaultMQProducerImpl::send(Message& msg,
								 MessageQueueSelector* pSelector,
								 void* arg,
								 SendCallback* pSendCallback)
{
	try
	{
		sendSelectImpl(msg, pSelector, arg, ASYNC, pSendCallback);
	}
	catch (MQBrokerException e)
	{

	}
}

void DefaultMQProducerImpl::sendOneway(Message& msg, MessageQueueSelector* pSelector, void* arg)
{
	try
	{
		sendSelectImpl(msg, pSelector, arg, ASYNC, NULL);
	}
	catch (MQBrokerException e)
	{

	}
}

TransactionSendResult DefaultMQProducerImpl::sendMessageInTransaction(Message& msg,
		LocalTransactionExecuter* tranExecuter, void* arg)
{
	//TODO
	TransactionSendResult result;

	return result;
}

void DefaultMQProducerImpl::endTransaction(//
	SendResult sendResult, //
	LocalTransactionState localTransactionState, //
	MQClientException localException)
{
	//TODO
}

std::map<std::string, TopicPublishInfo> DefaultMQProducerImpl::getTopicPublishInfoTable()
{
	return m_topicPublishInfoTable;
}

MQClientFactory* DefaultMQProducerImpl::getmQClientFactory()
{
	return m_pMQClientFactory;
}

int DefaultMQProducerImpl::getZipCompressLevel()
{
	return m_zipCompressLevel;
}

void DefaultMQProducerImpl::setZipCompressLevel(int zipCompressLevel)
{
	m_zipCompressLevel = zipCompressLevel;
}

SendResult DefaultMQProducerImpl::sendDefaultImpl(Message& msg,
												  CommunicationMode communicationMode,
												  SendCallback* pSendCallback)
{
	// 有效性检查
	makeSureStateOK();
	Validators::checkMessage(msg, m_pDefaultMQProducer->getMaxMessageSize());

	long long beginTimestamp = GetCurrentTimeMillis();
	long long endTimestamp = beginTimestamp;

	TopicPublishInfo topicPublishInfo = tryToFindTopicPublishInfo(msg.getTopic());
	SendResult sendResult;
	if (topicPublishInfo.ok()) 
	{
		MessageQueue* mq=NULL;
		
		for (int times = 0; times < 3
			&& int(endTimestamp - beginTimestamp) < m_pDefaultMQProducer->getSendMsgTimeout(); times++) 
		{
				std::string lastBrokerName = NULL == mq ? "" : mq->getBrokerName();
				MessageQueue* tmpmq = topicPublishInfo.selectOneMessageQueue(lastBrokerName);
				if (tmpmq != NULL) 
				{
					mq = tmpmq;
					try 
					{
						sendResult = sendKernelImpl(msg, *mq, communicationMode, pSendCallback);
						endTimestamp =GetCurrentTimeMillis();
						switch (communicationMode) 
						{
						case ASYNC:
							return sendResult;
						case ONEWAY:
							return sendResult;
						case SYNC:
							if (sendResult.getSendStatus() != SEND_OK) 
							{
								if (m_pDefaultMQProducer->isRetryAnotherBrokerWhenNotStoreOK())
								{
									continue;
								}
							}

							return sendResult;
						default:
							break;
						}
					}
					catch (RemotingException& /*e*/)
					{
						endTimestamp = GetCurrentTimeMillis();
						continue;
					}
					catch (MQClientException& /*e*/)
					{
						endTimestamp = GetCurrentTimeMillis();
						continue;
					}
					catch (MQBrokerException& e) 
					{
						endTimestamp =GetCurrentTimeMillis();
						switch (e.GetError()) {
						case TOPIC_NOT_EXIST_VALUE:
						case SERVICE_NOT_AVAILABLE_VALUE:
						case SYSTEM_ERROR_VALUE:
						case NO_PERMISSION_VALUE:
							continue;
						default:
							return sendResult;

							throw;
						}
					}
					catch (InterruptedException& /*e*/) 
					{
						throw;
					}
				}
				else
				{
					break;
				}
		} // end of for

		THROW_MQEXCEPTION(MQClientException,"Retry many times, still failed",-1);

		return sendResult;
	}

	std::list<std::string> nsList = getmQClientFactory()->getMQClientAPIImpl()->getNameServerAddressList();
	if (nsList.empty()) 
	{
		// 说明没有设置Name Server地址
		THROW_MQEXCEPTION(MQClientException,"No name server address, please set it.",-1);
	}

	THROW_MQEXCEPTION(MQClientException,"No route info of this topic, ",-1);

	return sendResult;
}

SendResult DefaultMQProducerImpl::sendKernelImpl(Message& msg,
												 const MessageQueue& mq,
												 CommunicationMode communicationMode,
												 SendCallback* sendCallback)
{
	std::string brokerAddr = m_pMQClientFactory->findBrokerAddressInPublish(mq.getBrokerName());

	//找不到，直接抛出异常
	if (brokerAddr.empty()) 
	{
		// TODO 此处可能对Name Server压力过大，需要调优
		tryToFindTopicPublishInfo(mq.getTopic());
		brokerAddr = m_pMQClientFactory->findBrokerAddressInPublish(mq.getBrokerName());
	}

	SendMessageContext context;
	if (!brokerAddr.empty()) 
	{
		const char* prevBody = msg.getBody();
		int prevLen = msg.getBodyLen();

		try 
		{
			int sysFlag = 0;
			if (tryToCompressMessage(msg)) 
			{
				sysFlag |= MessageSysFlag::CompressedFlag;
			}

			std::string tranMsg = msg.getProperty(Message::PROPERTY_TRANSACTION_PREPARED);
			if (!tranMsg.empty() && tranMsg=="true")
			{
				sysFlag |= MessageSysFlag::TransactionPreparedType;
			}

			// 执行hook
			if (hasHook()) 
			{
				context.producerGroup=(m_pDefaultMQProducer->getProducerGroup());
				context.communicationMode=(communicationMode);
				context.brokerAddr=(brokerAddr);
				context.msg=(msg);
				context.mq=(mq);
				executeHookBefore(context);
			}

			SendMessageRequestHeader* requestHeader = new SendMessageRequestHeader();
			requestHeader->producerGroup=(m_pDefaultMQProducer->getProducerGroup());
			requestHeader->topic=(msg.getTopic());
			requestHeader->defaultTopic=(m_pDefaultMQProducer->getCreateTopicKey());
			requestHeader->defaultTopicQueueNums=(m_pDefaultMQProducer->getDefaultTopicQueueNums());
			requestHeader->queueId=(mq.getQueueId());
			requestHeader->sysFlag=(sysFlag);
			requestHeader->bornTimestamp=(GetCurrentTimeMillis());
			requestHeader->flag=(msg.getFlag());
			requestHeader->properties=(MessageDecoder::messageProperties2String(msg.getProperties()));

			SendResult sendResult = m_pMQClientFactory->getMQClientAPIImpl()->sendMessage(
				brokerAddr,
				mq.getBrokerName(),
				msg,
				requestHeader,
				m_pDefaultMQProducer->getSendMsgTimeout(),
				communicationMode,
				sendCallback
				);

			if (hasHook())
			{
				context.sendResult=(sendResult);
				executeHookAfter(context);
			}

			return sendResult;
		}
		catch (RemotingException& e)
		{
			if (hasHook())
			{
				context.pException=(&e);
				executeHookAfter(context);
			}
			throw;
		}
		catch (MQBrokerException& e)
		{
			if (hasHook())
			{
				context.pException=(&e);
				executeHookAfter(context);
			}
			throw;
		}
		catch (InterruptedException& e)
		{
			if (hasHook())
			{
				context.pException=(&e);
				executeHookAfter(context);
			}
			throw;
		}
	}

	THROW_MQEXCEPTION(MQClientException,"The broker[" + mq.getBrokerName() + "] not exist",-1);
}

SendResult DefaultMQProducerImpl::sendSelectImpl(Message& msg,
												 MessageQueueSelector* selector,
												 void* pArg,
												 CommunicationMode communicationMode,
												 SendCallback* sendCallback)
{
	//TODO
	SendResult result;

	return result;
}

void DefaultMQProducerImpl::makeSureStateOK()
{
	if (m_serviceState != RUNNING)
	{
		THROW_MQEXCEPTION(MQClientException,"The producer service state not OK, ",-1);
	}
}

void DefaultMQProducerImpl::checkConfig()
{
}

TopicPublishInfo DefaultMQProducerImpl::tryToFindTopicPublishInfo(const std::string& topic)
{
	TopicPublishInfo* info=NULL;
	std::map<std::string, TopicPublishInfo>::iterator it = m_topicPublishInfoTable.find(topic);

	if (it==m_topicPublishInfoTable.end()|| !it->second.ok())
	{
		m_topicPublishInfoTable[topic]= TopicPublishInfo();
		m_pMQClientFactory->updateTopicRouteInfoFromNameServer(topic);
		it = m_topicPublishInfoTable.find(topic);
	}

	if (it==m_topicPublishInfoTable.end()||!it->second.ok())
	{
		m_pMQClientFactory->updateTopicRouteInfoFromNameServer(topic, true, m_pDefaultMQProducer);
		it = m_topicPublishInfoTable.find(topic);
	}

	return (it->second);
}

bool DefaultMQProducerImpl::tryToCompressMessage(Message& msg)
{
	const char* body = msg.getBody();
	if (body != NULL)
	{
		if (msg.getBodyLen() >= m_pDefaultMQProducer->getCompressMsgBodyOverHowmuch())
		{
			unsigned char* pOut;
			int outLen;

			if (UtilAll::compress(body,msg.getBodyLen(),&pOut,&outLen,m_pDefaultMQProducer->getCompressLevel()))
			{
				msg.setBody((char*)pOut,outLen);
				free(pOut);

				return true;
			}
		}
	}

	return false;
}

TransactionCheckListener* DefaultMQProducerImpl::checkListener()
{
	return NULL;
}
