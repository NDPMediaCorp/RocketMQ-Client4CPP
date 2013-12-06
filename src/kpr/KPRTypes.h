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
#if!defined __KPR_TYPES_H__
#define __KPR_TYPES_H__

#ifdef WIN32
#include <WinSock2.h>
#include <Windows.h>
#include <sys/timeb.h>
#else
#include <sys/time.h>
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef WIN32
typedef DWORD ThreadKey;
#else
typedef pthread_key_t ThreadKey;
#endif

namespace kpr
{
	class ThreadId
	{
	public:
#ifdef WIN32
		ThreadId(DWORD id = 0)
#else
		ThreadId(pthread_t id = 0)
#endif
			: m_threadId(id)
		{
		}

		bool operator==(const ThreadId& id) const
		{
			return m_threadId == id.m_threadId;
		}

		bool operator!=(const ThreadId& id) const
		{
			return !(*this == id);
		}

#ifdef WIN32
		operator DWORD() const
		{
			return m_threadId;
		}
#else
		operator pthread_t() const
		{
			return m_threadId;
		}
#endif

		static ThreadId GetCurrentThreadId()
		{
#ifdef WIN32
			return ThreadId(::GetCurrentThreadId());
#else
			return ThreadId(pthread_self());
#endif
		}

#ifdef WIN32
		DWORD m_threadId;
#else
		pthread_t m_threadId;
#endif
	};
}

#endif
