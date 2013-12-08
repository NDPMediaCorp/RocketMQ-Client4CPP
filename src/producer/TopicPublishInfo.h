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

#if!defined __TOPICPUBLISHINFO_H__
#define __TOPICPUBLISHINFO_H__

#include <list>
#include <vector>
#include <string>
#include <math.h>
#include <stdlib.h>
#include "MessageQueue.h"
#include "AtomicValue.h"

/**
* 发布Topic用到的路由信息
*
*/
class TopicPublishInfo
{
public:
	TopicPublishInfo()
	{
		m_orderTopic = false;
	}

	bool isOrderTopic()
	{
		return m_orderTopic;
	}

	bool ok()
	{
		return !m_messageQueueList.empty();
	}

	void setOrderTopic(bool orderTopic)
	{
		m_orderTopic = orderTopic;
	}

	std::vector<MessageQueue*>& getMessageQueueList()
	{
		return m_messageQueueList;
	}

	void setMessageQueueList(const std::vector<MessageQueue*>& messageQueueList)
	{
		m_messageQueueList = messageQueueList;
	}

	AtomicInteger& getSendWhichQueue()
	{
		return m_sendWhichQueue;
	}

	void setSendWhichQueue(AtomicInteger& sendWhichQueue)
	{
		m_sendWhichQueue = sendWhichQueue;
	}

	/**
	* 如果lastBrokerName不为null，则寻找与其不同的MessageQueue
	*/
	MessageQueue* selectOneMessageQueue(const std::string lastBrokerName)
	{
		if (!lastBrokerName.empty())
		{
			int index = m_sendWhichQueue++;
			for (size_t i = 0; i < m_messageQueueList.size(); i++)
			{
				int pos = abs(index++) % m_messageQueueList.size();
				MessageQueue* mq = m_messageQueueList.at(pos);
				if (mq->getBrokerName()!=lastBrokerName)
				{
					return mq;
				}
			}

			return NULL;
		}
		else
		{
			int index = m_sendWhichQueue++;
			int pos = abs(index) % m_messageQueueList.size();
			return m_messageQueueList.at(pos);
		}
	}

private:
	bool m_orderTopic;
	std::vector<MessageQueue*> m_messageQueueList;
	AtomicInteger m_sendWhichQueue;
};

#endif
