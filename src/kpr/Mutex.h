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
#ifndef __KPR_MUTEX_H__
#define __KPR_MUTEX_H__

#include "KPRTypes.h"
#include <errno.h>

namespace kpr
{
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

		int Lock()const;
		int Unlock()const;
		int TryLock()const;

		ThreadId GetOwner()const;

	private:
		Mutex(const Mutex&);
		const Mutex& operator=(const Mutex&);

#ifdef WIN32
		mutable CRITICAL_SECTION m_mutex;
#else
		mutable pthread_mutex_t m_mutex;
#endif

		friend class Condition;
	};

	class RecursiveMutex
	{
	public:
		RecursiveMutex();
		~RecursiveMutex();

		bool Lock()const;
		bool Unlock()const;
		bool TryLock()const;

		ThreadId GetOwner()const;

		unsigned int GetCount()const
		{
			return m_count;
		}

	private:
		RecursiveMutex(const RecursiveMutex&);

		const RecursiveMutex& operator=(const RecursiveMutex&);

		bool  lock(int count);
		bool  tryLock();
		bool unlock();

		unsigned int reset4Condvar();

	private:
#ifdef WIN32
		CRITICAL_SECTION m_mutex;
#else
		pthread_mutex_t m_mutex;
#endif

		Mutex m_internal;
		mutable unsigned int m_count;
		mutable ThreadId m_owner;

		friend class Condition;
		friend class ConditionHelper;
	};
}
#endif
