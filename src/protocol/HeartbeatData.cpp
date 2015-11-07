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

#include "HeartbeatData.h"
#include <sstream>

HeartbeatData::HeartbeatData()
{
}

HeartbeatData::~HeartbeatData()
{
}

void HeartbeatData::Encode(std::string& outData)
{
	std::stringstream ss;
	std::string consumerDataSet;

	std::set<ConsumerData>::iterator it = m_consumerDataSet.begin();

	for (;it!=m_consumerDataSet.end();it++)
	{
		const ConsumerData& data=*it;
		std::string subscriptionDataSet;
		std::stringstream s1;

		std::set<SubscriptionData>::iterator it1 = data.subscriptionDataSet.begin();

		for (;it1!= data.subscriptionDataSet.end();it1++)
		{
			SubscriptionData data1 = *it1;
			std::string sdata;
			data1.encode(sdata);
			subscriptionDataSet.append(sdata).append(",");
		}

		if (!subscriptionDataSet.empty())
		{
			subscriptionDataSet.pop_back();
		}

		s1<<"{"<<"\"consumeFromWhere\":\""<<getConsumeFromWhereString(data.consumeFromWhere)<<"\","
			<<"\"consumeType\":\""<<getConsumeTypeString(data.consumeType)<<"\","
			<<"\"groupName\":\""<<data.groupName<<"\","
			<<"\"messageModel\":\""<<getMessageModelString(data.messageModel)<<"\","
			<<"\"subscriptionDataSet\":["<<subscriptionDataSet<<"]},";

		consumerDataSet+= s1.str();
	}

	if (!consumerDataSet.empty())
	{
		consumerDataSet.pop_back();
	}

	std::string producerDataSet;
	std::set<ProducerData>::iterator it1 = m_producerDataSet.begin();
	for (;it1!=m_producerDataSet.end();it1++)
	{
		const ProducerData&  data = *it1;
		producerDataSet.append("{\"groupName\":\"").append(data.groupName).append("\"},");
	}

	if (!producerDataSet.empty())
	{
		producerDataSet.pop_back();
	}

	ss<<"{"<<"\"clientID\":\""<<m_clientID<<"\","
		<<"\"consumerDataSet\":["<<consumerDataSet<<"],"
		<<"\"producerDataSet\":["<<producerDataSet<<"]}";

	outData = ss.str();
}

std::string HeartbeatData::getClientID()
{
	return m_clientID;
}

void HeartbeatData::setClientID(const std::string& clientID)
{
	m_clientID = clientID;
}

std::set<ProducerData>& HeartbeatData::getProducerDataSet()
{
	return m_producerDataSet;
}

void HeartbeatData::setProducerDataSet(const std::set<ProducerData>& producerDataSet)
{
	m_producerDataSet = producerDataSet;
}

std::set<ConsumerData>& HeartbeatData::getConsumerDataSet()
{
	return m_consumerDataSet;
}

void HeartbeatData::setConsumerDataSet(const std::set<ConsumerData>& consumerDataSet)
{
	m_consumerDataSet = consumerDataSet;
}
