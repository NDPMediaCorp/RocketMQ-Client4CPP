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
#include "Condition.h"

#include <errno.h>
#include <assert.h>

#include "Mutex.h"
#include "ScopedLock.h"
#include "Semaphore.h"
#include "KPRUtil.h"
#include "Exception.h"

namespace kpr
{
#if defined(WIN32)

	class CondImpl
	{
	public:
		CondImpl()
			: m_gate(1),
			  m_blocked(0),
			  m_unblocked(0),
			  m_toUnblock(0)
		{
		}

		void Notify(bool broadcast)
		{
			m_gate.Wait();
			m_internal.Lock();

			if (m_unblocked != 0)
			{
				m_blocked -= m_unblocked;
				m_unblocked = 0;
			}

			if (m_blocked > 0)
			{
				m_toUnblock = (broadcast) ? m_blocked : 1;
				m_internal.Unlock();
				m_queue.Release();
			}
			else
			{
				m_gate.Release();
				m_internal.Unlock();
			}
		}

		void PreWait()
		{
			m_gate.Wait();
			++m_blocked;
			m_gate.Release();
		}

		bool Wait(long timeout)
		{
			try
			{
				bool rc = m_queue.Wait(timeout);
				postwait(!rc);
				return rc;
			}
			catch(...)
			{
				postwait(false);
				throw;
			}
		}

	private:
		void postwait(bool timeout)
		{
			m_internal.Lock();
			++m_unblocked;

			if (m_toUnblock != 0)
			{
				bool last = (--m_toUnblock == 0);
				m_internal.Unlock();

				if (last)
				{
					m_gate.Release();
				}
				else
				{
					m_queue.Release();
				}
			}
			else
			{
				m_internal.Unlock();
			}
		}

		Semaphore m_gate;
		Semaphore m_queue;
		Mutex m_internal;

		long m_blocked;
		long m_unblocked;
		long m_toUnblock;
	};
#else
	class ConditionHelper
	{
		RecursiveMutex& m_mutex;
		int m_count;

	public:

		ConditionHelper(RecursiveMutex& mutex, int count)
			: m_mutex(mutex),
			m_count(count)
		{
		}

		~ConditionHelper()
		{
			pthread_mutex_unlock(&m_mutex.m_mutex);
			m_mutex.lock(m_count);
		}
	};
#endif


	Condition::Condition()
	{
#ifdef WIN32
		m_impl = new CondImpl;
#else
		pthread_cond_init(&m_cond, 0);
#endif
	}

	Condition::~Condition()
	{
#if defined(WIN32)
		delete m_impl;
#else
		pthread_cond_destroy(&m_cond);
#endif
	}
	
	void Condition::Wait(Mutex& mutex)
	{
		wait(mutex, -1);
	}

	bool Condition::Wait(Mutex& mutex, long timeout)
	{
		assert(timeout>=0&&"timeout value is negative");

		return wait(mutex, timeout);
	}

	void Condition::Wait( RecursiveMutex& mutex )
	{
		wait(mutex, -1);
	}

	bool Condition::Wait( RecursiveMutex& mutex, long timeout )
	{
		assert(timeout>=0&&"timeout value is negative");

		return wait(mutex, timeout);
	}

	void Condition::Notify()
	{
#ifdef WIN32
		m_impl -> Notify(false);
#else
		pthread_cond_signal(&m_cond);
#endif
	}

	void Condition::NotifyAll()
	{
#if defined(WIN32)
		m_impl -> Notify(true);
#else
		pthread_cond_broadcast(&m_cond);
#endif
	}

	bool Condition::wait(Mutex& mutex, long timeout)
	{
#if defined(WIN32)

		m_impl -> PreWait();
		mutex.Unlock();

		try
		{
			bool rc = m_impl -> Wait(timeout);
			mutex.Lock();
			return rc;
		}
		catch(...)
		{
			mutex.Lock();
			throw;
		}
#else

		int ret = 0;
		if (timeout < 0)
		{
			ret = pthread_cond_wait(&m_cond, &mutex.m_mutex);
		}
		else
		{
			struct timespec abstime = CalcAbsTime(timeout);
			ret = pthread_cond_timedwait(&m_cond, &mutex.m_mutex, &abstime);
		}
		if (ret==0)
		{
			return true;
		}
		else
		{
			if (errno==EINTR)
			{
				THROW_EXCEPTION(InterruptedException,"pthread_cond_timedwait failed",errno);
			}
			else if (errno==ETIMEDOUT && timeout >= 0)
			{
				return false;
			}
		}
		return true;
#endif
	}

	bool Condition::wait( RecursiveMutex& mutex, long timeout )
	{
#if defined(WIN32)

		m_impl -> PreWait();
		mutex.Unlock();

		try
		{
			bool rc = m_impl -> Wait(timeout);
			mutex.Lock();
			return rc;
		}
		catch(...)
		{
			mutex.Lock();
			throw;
		}
#else
		unsigned int count = mutex.reset4Condvar();
		ConditionHelper unlock(mutex, count);

		int ret = 0;
		if (timeout < 0)
		{
			ret = pthread_cond_wait(&m_cond, &mutex.m_mutex);
		}
		else
		{
			struct timespec abstime = CalcAbsTime(timeout);
			ret = pthread_cond_timedwait(&m_cond, &mutex.m_mutex, &abstime);
		}

		if (ret==0)
		{
			return true;
		}
		else
		{
			if (errno==EINTR)
			{
				THROW_EXCEPTION(InterruptedException,"pthread_cond_timedwait failed",errno);
			}
			else if (errno==ETIMEDOUT && timeout >= 0)
			{
				return false;
			}
		}

		return true;
#endif
	}
}
