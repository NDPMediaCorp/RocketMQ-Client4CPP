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

#if!defined __PROCESSQUEUE_H__
#define __PROCESSQUEUE_H__

#include <list>
#include <map>

#include "Mutex.h"
#include "AtomicValue.h"

class MessageExt;

/**
* 正在被消费的队列，含消息
*
*/
class ProcessQueue
{

public:
	ProcessQueue();

	bool isLockExpired();

	/**
	* @return 是否需要分发当前队列到消费线程池
	*/
	bool putMessage(const std::list<MessageExt*>& msgs);

	/**
	* 获取当前队列的最大跨度
	*/
	long long getMaxSpan();

	/**
	* 删除已经消费过的消息，返回最小Offset，这个Offset对应的消息未消费
	*
	* @param msgs
	* @return
	*/
	long long removeMessage(const std::list<MessageExt*>& msgs);

	std::map<long long, MessageExt*> getMsgTreeMap();

	AtomicLong getMsgCount();
	bool isDropped();
	void setDropped(bool dropped);

	/**
	* ========================================================================
	* 以下部分为顺序消息专有操作
	*/

	void setLocked(bool locked);
	bool isLocked();
	void rollback();

	long long commit();
	void makeMessageToCosumeAgain(const std::list<MessageExt*>& msgs);

	/**
	* 如果取不到消息，则将正在消费状态置为false
	*
	* @param batchSize
	* @return
	*/
	std::list<MessageExt*> takeMessages(int batchSize);

	long long getLastLockTimestamp();
	void setLastLockTimestamp(long long lastLockTimestamp);

public:
	static unsigned int s_RebalanceLockMaxLiveTime;// 客户端本地Lock存活最大时间，超过则自动过期，单位ms
	static unsigned int s_RebalanceLockInterval;// 定时Lock间隔时间，单位ms

private:
	kpr::Mutex m_lockTreeMap;
	std::map<long long, MessageExt*> m_msgTreeMap;
	volatile long long m_queueOffsetMax ;
	AtomicLong m_msgCount;
	volatile bool m_dropped;// 当前Q是否被rebalance丢弃

	/**
	* 顺序消息专用
	*/
	volatile bool m_locked;// 是否从Broker锁定
	volatile unsigned long long m_lastLockTimestamp;// 最后一次锁定成功时间戳
	volatile bool m_consuming;// 是否正在被消费
	std::map<long long, MessageExt*> m_msgTreeMapTemp;// 事务方式消费，未提交的消息
};

#endif
