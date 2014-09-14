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
#if!defined __HEARTBEATDATA_H__
#define __HEARTBEATDATA_H__

#include <string>
#include <set>

#include "ConsumeType.h"
#include "SubscriptionData.h"
#include "RemotingSerializable.h"

typedef struct ConsumerData
{
	std::string groupName;
	ConsumeType consumeType;
	MessageModel messageModel;
	ConsumeFromWhere consumeFromWhere;
	std::set<SubscriptionData> subscriptionDataSet;
	bool operator < (const ConsumerData& cd)const
	{
		return groupName<cd.groupName;
	}
} ConsumerData;

typedef struct  ProducerData
{
	std::string groupName;
	bool operator < (const ProducerData& pd)const
	{
		return groupName<pd.groupName;
	}

} ProducerData;


class HeartbeatData : public RemotingSerializable
{
public:
	HeartbeatData();
	~HeartbeatData();
	void Encode(std::string& outData);

	std::string getClientID();
	void setClientID(const std::string& clientID);

	std::set<ProducerData>& getProducerDataSet();
	void setProducerDataSet(const std::set<ProducerData>& producerDataSet);

	std::set<ConsumerData>& getConsumerDataSet();
	void setConsumerDataSet(const std::set<ConsumerData>& consumerDataSet);

private:
	std::string m_clientID;
	std::set<ProducerData> m_producerDataSet;
	std::set<ConsumerData> m_consumerDataSet;
};

#endif
