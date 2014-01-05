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
#include "SubscriptionData.h"

std::string SubscriptionData::SUB_ALL = "*";

SubscriptionData::SubscriptionData()
{

}

SubscriptionData::SubscriptionData(const std::string& topic, const std::string& subString)
	:m_topic(topic),
	 m_subString(subString)
{

}

std::string SubscriptionData::getTopic()const
{
	return m_topic;
}

void SubscriptionData::setTopic(const std::string& topic)
{
	m_topic = topic;
}

std::string SubscriptionData::getSubString()
{
	return m_subString;
}

void SubscriptionData::setSubString(const std::string& subString)
{
	m_subString = subString;
}

std::set<std::string>& SubscriptionData::getTagsSet()
{
	return m_tagsSet;
}

void SubscriptionData::setTagsSet(const std::set<std::string>& tagsSet)
{
	m_tagsSet = tagsSet;
}

long long SubscriptionData::getSubVersion()
{
	return m_subVersion;
}

void SubscriptionData::setSubVersion(long long subVersion)
{
	m_subVersion = subVersion;
}

std::set<int>& SubscriptionData::getCodeSet()
{
	return m_codeSet;
}

void SubscriptionData::setCodeSet(const std::set<int>& codeSet)
{
	m_codeSet = codeSet;
}

int SubscriptionData::hashCode()
{
	int prime = 31;
	int result = 1;
	/*result = prime * result + ((codeSet == null) ? 0 : codeSet.hashCode());
	result = prime * result + ((subString == null) ? 0 : subString.hashCode());
	result = prime * result + (int) (subVersion ^ (subVersion >>> 32));
	result = prime * result + ((tagsSet == null) ? 0 : tagsSet.hashCode());
	result = prime * result + ((topic == null) ? 0 : topic.hashCode());*/

	return result;
}

bool SubscriptionData::operator==(const SubscriptionData& other)
{
	if (m_codeSet!=other.m_codeSet)
	{
		return false;
	}

	if (m_subString!=other.m_subString)
	{
		return false;
	}

	if (m_subVersion!=other.m_subVersion)
	{
		return false;
	}

	if (m_tagsSet!=other.m_tagsSet)
	{
		return false;
	}

	if (m_topic!=other.m_topic)
	{
		return false;
	}

	return true;
}

bool SubscriptionData::operator<(const SubscriptionData& other)const
{
	if (m_topic<other.m_topic)
	{
		return true;
	}
	else if (m_topic == other.m_topic)
	{
		if (m_subString<other.m_subString)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
