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

#if!defined __GETCONSUMERLISTBYGROUPRESPONSEBODY_H__
#define __GETCONSUMERLISTBYGROUPRESPONSEBODY_H__

#include <string>
#include <list>
#include "json/json.h"

#include "RemotingSerializable.h"

class GetConsumerListByGroupResponseBody : public RemotingSerializable
{
public:
	GetConsumerListByGroupResponseBody()
	{

	}
	
	~GetConsumerListByGroupResponseBody()
	{

	}

	void Encode(std::string& outData)
	{

	}

	static GetConsumerListByGroupResponseBody* Decode(char* pData,int len)
	{
		//"consumerIdList":["192.168.1.120@DEFAULT"]
		GetConsumerListByGroupResponseBody* ret =  new GetConsumerListByGroupResponseBody();
		Json::Reader reader;
		Json::Value object;
		if (!reader.parse(pData, object))
		{
			return NULL;
		}

		std::list<std::string> consumers;

		Json::Value ext = object["consumerIdList"];

		for (int i=0;i< ext.size();i++)
		{
			Json::Value v = ext[i];
			if (v!=Json::Value::null)
			{
				consumers.push_back(v.asString());
			}
		}
		
		ret->setConsumerIdList(consumers);

		return ret;
	}

	std::list<std::string>& getConsumerIdList()
	{
		return m_consumerIdList;
	}

	void setConsumerIdList(const std::list<std::string>& consumerIdList)
	{
		m_consumerIdList = consumerIdList;
	}

private:
	std::list<std::string> m_consumerIdList;
};

#endif
