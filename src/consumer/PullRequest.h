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

#if!defined __PULLREQUEST_H__
#define __PULLREQUEST_H__

#include <string>
#include "MessageQueue.h"
#include "ProcessQueue.h"

/**
* 拉消息请求
*
*/
class PullRequest
{
public:
	virtual ~PullRequest();

	std::string getConsumerGroup();
	void setConsumerGroup(const std::string& consumerGroup);

	MessageQueue* getMessageQueue();
	void setMessageQueue(MessageQueue* pMessageQueue);

	long long getNextOffset();

	void setNextOffset(long long nextOffset);

	std::string toString();

	int hashCode();

	bool operator==(const PullRequest& other);

	ProcessQueue* getProcessQueue();
	void setProcessQueue(ProcessQueue* pProcessQueue);

private:
	std::string m_consumerGroup;
	MessageQueue* m_pMessageQueue;
	ProcessQueue* m_pProcessQueue;
	// hashCode与equals方法不包含此字段
	long long m_nextOffset;
};

#endif
