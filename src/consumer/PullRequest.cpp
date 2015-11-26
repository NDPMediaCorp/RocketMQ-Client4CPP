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

#include "PullRequest.h"


PullRequest::~PullRequest()
{

}

std::string PullRequest::getConsumerGroup()
{
	return m_consumerGroup;
}

void PullRequest::setConsumerGroup(const std::string& consumerGroup)
{
	m_consumerGroup = consumerGroup;
}

MessageQueue* PullRequest::getMessageQueue()
{
	return m_pMessageQueue;
}

void PullRequest::setMessageQueue(MessageQueue* pMessageQueue)
{
	m_pMessageQueue = pMessageQueue;
}

long long PullRequest::getNextOffset()
{
	return m_nextOffset;
}

void PullRequest::setNextOffset(long long nextOffset)
{
	m_nextOffset = nextOffset;
}


std::string PullRequest::toString() {
	std::string s;
	s.append("{")
			.append("ConsumerGroup:").append(m_consumerGroup)
			.append(", ").append("MessageQueue: ").append(m_pMessageQueue->toString())
			.append(", NextOffset: ").append(std::to_string(m_nextOffset))
	 .append("}");
	return s;
}

int PullRequest::hashCode()
{
	int prime = 31;
	int result = 1;
	//result = prime * result + ((consumerGroup == null) ? 0 : consumerGroup.hashCode());
	//result = prime * result + ((messageQueue == null) ? 0 : messageQueue.hashCode());
	return result;
}

bool PullRequest::operator==(const PullRequest& other)
{
	if (m_consumerGroup!=other.m_consumerGroup)
	{
		return false;
	}

	if (!(*m_pMessageQueue==*(other.m_pMessageQueue)))
	{
		return false;
	}

	return true;
}

ProcessQueue* PullRequest::getProcessQueue()
{
	return m_pProcessQueue;
}

void PullRequest::setProcessQueue(ProcessQueue* pProcessQueue)
{
	m_pProcessQueue = pProcessQueue;
}
