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
#include "Mutex.h"

#ifndef WIN32
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#endif

namespace kpr
{
	Mutex::Mutex()
	{
#ifdef WIN32
		InitializeCriticalSection(&m_mutex);
#else
		pthread_mutex_init(&m_mutex,  NULL);
#endif

	}

	Mutex::~Mutex()
	{
#ifdef WIN32
		DeleteCriticalSection(&m_mutex);
#else
		pthread_mutex_destroy(&m_mutex);
#endif
	}

	int Mutex::Lock() const
	{
#ifdef WIN32
		EnterCriticalSection(&m_mutex);
		return 1;
#else
		return pthread_mutex_lock(&m_mutex);
#endif
	}

	int Mutex::TryLock() const
	{
#ifdef WIN32
		return TryEnterCriticalSection(&m_mutex);
#else
		return pthread_mutex_trylock(&m_mutex);
#endif
	}

	int Mutex::Unlock() const
	{
#ifdef WIN32
		LeaveCriticalSection(&m_mutex);
		return 1;
#else
		return pthread_mutex_unlock(&m_mutex);
#endif
	}

//***********
//RecursiveMutex
//***************

	RecursiveMutex::RecursiveMutex()
		: m_count(0),
		  m_owner(ThreadId())
	{
#ifdef WIN32
		InitializeCriticalSection(&m_mutex);
#else
		pthread_mutex_init(&m_mutex,  NULL);
#endif
	}

	RecursiveMutex::~RecursiveMutex()
	{
#ifdef WIN32
		DeleteCriticalSection(&m_mutex);
#else
		pthread_mutex_destroy(&m_mutex);
#endif
	}

	bool RecursiveMutex::Lock()const
	{
		return ((RecursiveMutex*)this)->lock(1);
	}

	bool RecursiveMutex::Unlock()const
	{
		return ((RecursiveMutex*)this)->unlock();
	}

	bool RecursiveMutex::TryLock()const
	{
		return ((RecursiveMutex*)this)->tryLock();
	}

	ThreadId RecursiveMutex::GetOwner()const
	{
		m_internal.Lock();
		ThreadId id;
		if (m_count > 0)
			id = m_owner;
		m_internal.Unlock();

		return id;
	}

	bool RecursiveMutex::lock(int count)
	{
		bool rc = false;
		bool obtained = false;

		while (!obtained)
		{
			m_internal.Lock();

			if (m_count == 0)
			{
				m_count = count;
				m_owner = ThreadId::GetCurrentThreadId();
				obtained = true;
				rc = true;

				try
				{
#ifdef WIN32
					EnterCriticalSection(&m_mutex);
#else
					pthread_mutex_lock(&m_mutex);
#endif

				}
				catch(...)
				{
					try
					{
						m_internal.Unlock();
					}
					catch(...)
					{
					}
					throw;
				}
			}
			else if (m_owner == ThreadId::GetCurrentThreadId())
			{
				m_count += count;
				obtained = true;
			}

			m_internal.Unlock();

			if (!obtained)
			{
#ifdef WIN32
				EnterCriticalSection(&m_mutex);
				LeaveCriticalSection(&m_mutex);
#else
				pthread_mutex_lock(&m_mutex);
				pthread_mutex_unlock(&m_mutex);
#endif
			}
		}

		return rc;
	}

	bool RecursiveMutex::tryLock()
	{
		bool obtained = false;

		m_internal.Lock();

		if (m_count == 0)
		{
			m_count = 1;
			m_owner = ThreadId::GetCurrentThreadId();
			obtained = true;

			try
			{
#ifdef WIN32
				EnterCriticalSection(&m_mutex);
#else
				pthread_mutex_lock(&m_mutex);
#endif
			}
			catch(...)
			{
				try
				{
					m_internal.Unlock();
				}
				catch(...)
				{
				}
				throw;
			}
		}
		else if (m_owner == ThreadId::GetCurrentThreadId())
		{
			++m_count;
			obtained = true;
		}

		m_internal.Unlock();

		return obtained;
	}

	bool RecursiveMutex::unlock()
	{
		bool rc;
		m_internal.Lock();

		if (--m_count == 0)
		{
			m_owner = ThreadId();

#ifdef WIN32
			LeaveCriticalSection(&m_mutex);

#else
			pthread_mutex_unlock(&m_mutex);
#endif

			rc = true;
		}
		else
		{
			rc = false;
		}

		m_internal.Unlock();

		return rc;
	}

	unsigned int RecursiveMutex::reset4Condvar()
	{
		m_internal.Lock();

		unsigned int count = m_count;
		m_count = 0;
		m_owner = ThreadId();

		m_internal.Unlock();

#if defined(WIN32)
		LeaveCriticalSection(&m_mutex);
#endif

		return count;
	}
}
