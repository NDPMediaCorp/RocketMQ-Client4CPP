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

#include <sstream>

#include "KPRUtil.h"

std::string SubscriptionData::SUB_ALL = "*";

SubscriptionData::SubscriptionData()
{
	m_subVersion = GetCurrentTimeMillis();
}

SubscriptionData::SubscriptionData(const std::string& topic, const std::string& subString)
	:m_topic(topic),
	 m_subString(subString)
{
	m_subVersion = GetCurrentTimeMillis();
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

void SubscriptionData::encode( std::string& outData )
{
	std::stringstream ss;
	std::stringstream ss1;

	std::set<int>::iterator it1 = m_codeSet.begin();
	for (;it1!=m_codeSet.end();it1++)
	{
		ss1<<*it1<<",";
	}
	
	std::string codeSet=ss1.str();
	if (!codeSet.empty())
	{
		codeSet.pop_back();
	}

	std::string tagsSet;

	std::set<std::string>::iterator it = m_tagsSet.begin();
	for (;it!=m_tagsSet.end();it++)
	{
		tagsSet+= "\""+*it+"\",";
	}

	if (!tagsSet.empty())
	{
		tagsSet.pop_back();
	}

	ss<<"{"<<"\"codeSet\":["<<codeSet<<"],"
		<<"\"subString\":\""<<m_subString<<"\","
		<<"\"subVersion\":"<<m_subVersion<<","
		<<"\"tagsSet\":["<<tagsSet<<"],"
		<<"\"topic\":\""<<m_topic<<"\"}";

	outData = ss.str();
}
