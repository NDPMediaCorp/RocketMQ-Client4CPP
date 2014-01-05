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
#if!defined __PULLRESULT_H__
#define __PULLRESULT_H__

#include <list>

#include "MessageExt.h"
#include "RocketMQClient.h"

enum PullStatus
{
	FOUND,//找到消息
	NO_NEW_MSG,//没有新的消息可以被拉取
	NO_MATCHED_MSG,//经过过滤后，没有匹配的消息
	OFFSET_ILLEGAL//Offset不合法，可能过大或者过小
};

/**
* 拉消息返回结果
*
*/
struct ROCKETMQCLIENT_API PullResult
{
	PullResult()
	{

	}

	PullResult(PullStatus pullStatus,
			   long long nextBeginOffset,
			   long long minOffset,
			   long long maxOffset,
			   std::list<MessageExt*>& msgFoundList)
		:pullStatus(pullStatus),
		 nextBeginOffset(nextBeginOffset),
		 minOffset(minOffset),
		 maxOffset(maxOffset),
		 msgFoundList(msgFoundList)
	{

	}
	
	~PullResult()
	{
		std::list<MessageExt*>::iterator it = msgFoundList.begin();

		for (;it!=msgFoundList.end();it++)
		{
			delete *it;
		}
	}

	PullStatus pullStatus;
	long long nextBeginOffset;
	long long minOffset;
	long long maxOffset;
	std::list<MessageExt*> msgFoundList;
};

#endif
