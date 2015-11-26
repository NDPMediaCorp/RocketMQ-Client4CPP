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

#include "MessageQueue.h"

#include <string.h>
#include <string>

MessageQueue::MessageQueue()
{

}

MessageQueue::MessageQueue(const std::string& topic, const std::string& brokerName, int queueId)
	:m_topic(topic),m_brokerName(brokerName),m_queueId(queueId)
{

}

std::string MessageQueue::getTopic()const
{
	return m_topic;
}

void MessageQueue::setTopic(const std::string& topic)
{
	m_topic = topic;
}

std::string MessageQueue::getBrokerName()const
{
	return m_brokerName;
}

void MessageQueue::setBrokerName(const std::string& brokerName)
{
	m_brokerName = brokerName;
}

int MessageQueue::getQueueId()const
{
	return m_queueId;
}

void MessageQueue::setQueueId(int queueId)
{
	m_queueId = queueId;
}

std::string MessageQueue::toString() {
	std::string s;
	s.append("{")
			.append("Topic:").append(m_topic)
			.append(", BrokerName: ").append(m_brokerName)
			.append(", QueueId: ").append(std::to_string(m_queueId))
	  .append("}");
	return s;
}

int MessageQueue::hashCode()
{
	// final int prime = 31;
	int result = 1;
	//  result = prime * result + ((brokerName == null) ? 0 : brokerName.hashCode());
	//  result = prime * result + queueId;
	// result = prime * result + ((topic == null) ? 0 : topic.hashCode());
	return result;
}

bool MessageQueue::operator==(const MessageQueue& mq)const
{
	if (this==&mq)
	{
		return true;
	}

	if (m_brokerName!=mq.m_brokerName)
	{
		return false;
	}

	if (m_queueId!=mq.m_queueId)
	{
		return false;
	}

	if (m_topic!=mq.m_topic)
	{
		return false;
	}

	return true;
}

int MessageQueue::compareTo(const MessageQueue& mq)const
{
	{
		int result = strcmp(m_topic.c_str(), mq.m_topic.c_str());
		if (result != 0)
		{
			return result;
		}
	}

	{
		int result = strcmp(m_brokerName.c_str(),mq.m_brokerName.c_str());
		if (result != 0)
		{
			return result;
		}
	}

	return m_queueId - mq.m_queueId;
}

bool MessageQueue::operator<(const MessageQueue& mq)const
{
	return compareTo(mq)<0;
}
