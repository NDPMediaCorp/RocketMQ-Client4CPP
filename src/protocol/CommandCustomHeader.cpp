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

#include "CommandCustomHeader.h"
#include <sstream>
#include <cstdlib>

#include "RemotingCommand.h"
#include "MQProtos.h"
#include "KPRUtil.h"
#include "json/json.h"

CommandCustomHeader* CommandCustomHeader::Decode(int code,char* pData,int len,bool isResponseType)
{
	if (isResponseType)
	{
		switch(code)
		{
		case SEND_MESSAGE_VALUE:
			return SendMessageResponseHeader::Decode(pData,len);
			break;
		case PULL_MESSAGE_VALUE:
			return PullMessageResponseHeader::Decode(pData,len);
			break;

		default:
			break;
		}
	}

	return NULL;
}

//
//GetRouteInfoRequestHeader
//
GetRouteInfoRequestHeader::GetRouteInfoRequestHeader()
{
}

GetRouteInfoRequestHeader::~GetRouteInfoRequestHeader()
{

}

void GetRouteInfoRequestHeader::Encode(std::string& outData)
{
	std::stringstream ss;
	ss<<"{"<<"\"topic\":"<<"\""<<topic<<"\"}";

	outData = ss.str();
}

//
//CreateTopicRequestHeader
//
CreateTopicRequestHeader::CreateTopicRequestHeader()
{
}

CreateTopicRequestHeader::~CreateTopicRequestHeader()
{
}

void CreateTopicRequestHeader::Encode(std::string& outData)
{
	std::stringstream ss;

	ss<<"{"<<"\"topic\":"<<"\""<<topic<<"\","
	  <<"\"defaultTopic\":"<<"\""<<defaultTopic<<"\","
	  <<"\"readQueueNums\":"<<readQueueNums<<","
	  <<"\"writeQueueNums\":"<<writeQueueNums<<","
	  <<"\"perm\":"<<perm<<","
	  <<"\"topicFilterType\":"<<"\""<<topicFilterType<<"\""
	  <<"}";

	outData = ss.str();
}

//
//SendMessageRequestHeader
//
SendMessageRequestHeader::SendMessageRequestHeader()
{
}

SendMessageRequestHeader::~SendMessageRequestHeader()
{
}

void SendMessageRequestHeader::Encode(std::string& outData)
{
	std::stringstream ss;

	ss<<"{"<<"\"producerGroup\":"<<"\""<<producerGroup<<"\","
	  <<"\"topic\":"<<"\""<<topic<<"\","
	  <<"\"defaultTopic\":"<<"\""<<defaultTopic<<"\","
	  <<"\"defaultTopicQueueNums\":"<<defaultTopicQueueNums<<","
	  <<"\"queueId\":"<<queueId<<","
	  <<"\"sysFlag\":"<<sysFlag<<","
	  <<"\"bornTimestamp\":"<<bornTimestamp<<","
	  <<"\"flag\":"<<flag<<","
	  <<"\"properties\":"<<"\""<<properties<<"\","
	  <<"\"reconsumeTimes\":"<<reconsumeTimes
	  <<"}";

	outData = ss.str();
}
//
//SendMessageResponseHeader
//

SendMessageResponseHeader::SendMessageResponseHeader()
{
}

SendMessageResponseHeader::~SendMessageResponseHeader()
{
}

void SendMessageResponseHeader::Encode(std::string& outData)
{
	std::stringstream ss;

	ss<<"{"<<"\"msgId\":"<<"\""<<msgId<<"\","
	  <<"\"queueId\":"<<queueId<<","
	  <<"\"queueOffset\":"<<queueOffset
	  <<"}";

	outData = ss.str();
}

CommandCustomHeader* SendMessageResponseHeader::Decode(char* pData,int len)
{
	Json::Reader reader;
	Json::Value object;
	if (!reader.parse(pData+8, object))
	{
		return NULL;
	}

	Json::Value ext = object["extFields"];

	std::string msgId = ext["msgId"].asString();
	int queueId = atoi(ext["queueId"].asCString());
	long long queueOffset = str2ll(ext["queueOffset"].asCString());

	SendMessageResponseHeader* h = new SendMessageResponseHeader();

	h->msgId = msgId;
	h->queueId = queueId;
	h->queueOffset = queueOffset;

	return h;
}

//
//PullMessageRequestHeader
//
PullMessageRequestHeader::PullMessageRequestHeader()
{
}

PullMessageRequestHeader::~PullMessageRequestHeader()
{
}

void PullMessageRequestHeader::Encode(std::string& outData)
{
	std::stringstream ss;

	ss<<"{"<<"\"consumerGroup\":"<<"\""<<consumerGroup<<"\","
	  <<"\"topic\":"<<"\""<<topic<<"\","
	  <<"\"queueId\":"<<queueId<<","
	  <<"\"queueOffset\":"<<queueOffset<<","
	  <<"\"maxMsgNums\":"<<maxMsgNums<<","
	  <<"\"sysFlag\":"<<sysFlag<<","
	  <<"\"commitOffset\":"<<commitOffset<<","
	  <<"\"suspendTimeoutMillis\":"<<suspendTimeoutMillis<<","
	  <<"\"subscription\":"<<"\""<<subscription<<"\","
	  <<"\"subVersion\":"<<subVersion
	  <<"}";

	outData = ss.str();
}

//
//PullMessageResponseHeader
//
PullMessageResponseHeader::PullMessageResponseHeader()
{
}

PullMessageResponseHeader::~PullMessageResponseHeader()
{
}

void PullMessageResponseHeader::Encode(std::string& outData)
{
	std::stringstream ss;

	ss<<"{"<<"\"suggestWhichBrokerId\":"<<suggestWhichBrokerId<<","
	  <<"\"nextBeginOffset\":"<<nextBeginOffset<<","
	  <<"\"minOffset\":"<<minOffset<<","
	  <<"\"maxOffset\":"<<maxOffset
	  <<"}";

	outData = ss.str();
}

CommandCustomHeader* PullMessageResponseHeader::Decode(char* pData,int len)
{
	Json::Reader reader;
	Json::Value object;
	if (!reader.parse(pData+8, object))
	{
		return NULL;
	}

	Json::Value ext = object["extFields"];
	long long suggestWhichBrokerId = str2ll(ext["suggestWhichBrokerId"].asCString());
	long long nextBeginOffset = str2ll(ext["nextBeginOffset"].asCString());
	long long minOffset = str2ll(ext["minOffset"].asCString());
	long long maxOffset = str2ll(ext["maxOffset"].asCString());
	
	PullMessageResponseHeader* h = new PullMessageResponseHeader();

	h->suggestWhichBrokerId = suggestWhichBrokerId;
	h->nextBeginOffset = nextBeginOffset;
	h->minOffset = minOffset;
	h->maxOffset = maxOffset;

	return h;
}
