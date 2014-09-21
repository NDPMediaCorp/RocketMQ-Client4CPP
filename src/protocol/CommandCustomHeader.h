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

#if!defined __COMMANDCUSTOMHEADER_H__
#define __COMMANDCUSTOMHEADER_H__

#include <string>

/**
* RemotingCommand中自定义字段对象
*
*/
class CommandCustomHeader
{
public :
	virtual ~CommandCustomHeader() {}
	virtual void Encode(std::string& outData)=0;

	static CommandCustomHeader* Decode(int code,char* pData,int len,bool isResponseType);
};

class GetRouteInfoRequestHeader : public CommandCustomHeader
{
public:
	GetRouteInfoRequestHeader();
	~GetRouteInfoRequestHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);
public:
	std::string topic;
};

class CreateTopicRequestHeader : public CommandCustomHeader
{
public:
	CreateTopicRequestHeader();
	~CreateTopicRequestHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	std::string topic;
	std::string defaultTopic;
	int readQueueNums;
	int writeQueueNums;
	int perm;
	std::string topicFilterType;
};

class SendMessageRequestHeader: public CommandCustomHeader
{
public:
	SendMessageRequestHeader();
	~SendMessageRequestHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	std::string producerGroup;
	std::string topic;
	std::string defaultTopic;
	int defaultTopicQueueNums;
	int queueId;
	int sysFlag;
	long long bornTimestamp;
	int flag;
	std::string properties;
	int reconsumeTimes;
};

class SendMessageResponseHeader: public CommandCustomHeader
{
public:
	SendMessageResponseHeader();
	~SendMessageResponseHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	std::string msgId;
	int queueId;
	long long queueOffset;
};

class PullMessageRequestHeader: public CommandCustomHeader
{
public:
	PullMessageRequestHeader();
	~PullMessageRequestHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	std::string consumerGroup;
	std::string topic;
	int queueId;
	long long queueOffset;
	int maxMsgNums;
	int sysFlag;
	long  long commitOffset;
	long long suspendTimeoutMillis;
	std::string subscription;
	long long subVersion;
};

class PullMessageResponseHeader: public CommandCustomHeader
{
public:
	PullMessageResponseHeader();
	~PullMessageResponseHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	long long suggestWhichBrokerId;
	long long nextBeginOffset;
	long long minOffset;
	long long maxOffset;
};

class GetConsumerListByGroupRequestHeader : public CommandCustomHeader
{
public:
	GetConsumerListByGroupRequestHeader();
	~GetConsumerListByGroupRequestHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	std::string consumerGroup;
};

class GetConsumerListByGroupResponseHeader : public CommandCustomHeader
{
public:
	GetConsumerListByGroupResponseHeader();
	~GetConsumerListByGroupResponseHeader();
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);
};

class ConsumerSendMsgBackRequestHeader : public CommandCustomHeader
{
public:
	ConsumerSendMsgBackRequestHeader();
	~ConsumerSendMsgBackRequestHeader();
	
	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	long long offset;
	std::string group;
	int delayLevel;
};

/**
 *  QueryConsumerOffsetRequestHeader 
 */
class QueryConsumerOffsetRequestHeader : public CommandCustomHeader
{
public:
	QueryConsumerOffsetRequestHeader();
	~QueryConsumerOffsetRequestHeader();

	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	std::string consumerGroup;
	std::string topic;
	int queueId;
};

/**
 *  QueryConsumerOffsetResponseHeader 
 */
class QueryConsumerOffsetResponseHeader : public CommandCustomHeader
{
public:
	QueryConsumerOffsetResponseHeader();
	~QueryConsumerOffsetResponseHeader();

	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	long long offset;
};


/**
 *  UpdateConsumerOffsetRequestHeader 
 */
class UpdateConsumerOffsetRequestHeader : public CommandCustomHeader
{
public:
	UpdateConsumerOffsetRequestHeader();
	~UpdateConsumerOffsetRequestHeader();

	virtual void Encode(std::string& outData);
	static CommandCustomHeader* Decode(char* pData,int len);

public:
	std::string consumerGroup;
	std::string topic;
	int queueId;
	long long commitOffset;
};

#endif
