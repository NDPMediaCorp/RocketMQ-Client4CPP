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
		GetConsumerListByGroupResponseBody* ret =  new GetConsumerListByGroupResponseBody();


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
