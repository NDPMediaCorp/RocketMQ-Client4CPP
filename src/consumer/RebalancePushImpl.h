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

#if!defined __REBALANCEPUSHIMPL_H__
#define __REBALANCEPUSHIMPL_H__

#include "RebalanceImpl.h"
#include <string.h>
#include <climits>
#include "DefaultMQPushConsumerImpl.h"
#include "AllocateMessageQueueStrategy.h"
#include "MQClientFactory.h"
#include "MessageQueueListener.h"
#include "OffsetStore.h"
#include "DefaultMQPushConsumer.h"

class DefaultMQPushConsumerImpl;

class RebalancePushImpl : public RebalanceImpl
{
public:
	RebalancePushImpl(DefaultMQPushConsumerImpl* pDefaultMQPushConsumerImpl);

	RebalancePushImpl(const std::string& consumerGroup,
		MessageModel messageModel,
		AllocateMessageQueueStrategy* pAllocateMessageQueueStrategy,
		MQClientFactory* pMQClientFactory,
		DefaultMQPushConsumerImpl* pDefaultMQPushConsumerImpl);

	void dispatchPullRequest(std::list<PullRequest*>& pullRequestList);
	long long computePullFromWhere(MessageQueue& mq);
	void messageQueueChanged(const std::string& topic,
		std::set<MessageQueue>& mqAll, 
		std::set<MessageQueue>& mqDivided);
	void removeUnnecessaryMessageQueue(MessageQueue& mq, ProcessQueue& pq);

private:
	DefaultMQPushConsumerImpl* m_pDefaultMQPushConsumerImpl;
};

#endif
