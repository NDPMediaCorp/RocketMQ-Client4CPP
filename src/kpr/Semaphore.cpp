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

#include "Semaphore.h"

#if !defined(WIN32)
#   include <unistd.h>
#   include <sys/time.h>
#endif

#include "KPRUtil.h"

namespace kpr
{
	Semaphore::Semaphore(long initial_count)
	{
#ifdef WIN32
		m_sem = CreateSemaphore(0, initial_count, 0x7fffffff, 0);
#else
		sem_init(&m_sem, 0, initial_count);
#endif
	}

	Semaphore::~Semaphore()
	{
#ifdef WIN32
		CloseHandle(m_sem);
#else
		sem_destroy(&m_sem);
#endif
	}

	bool Semaphore::Wait()
	{
#ifdef WIN32
		int rc = WaitForSingleObject(m_sem, INFINITE);

		return rc != WAIT_TIMEOUT && rc !=WAIT_ABANDONED;
#else
		int rc;
		rc = sem_wait(&m_sem);

		return !rc;
#endif
	}

	bool Semaphore::Wait(long timeout)
	{
#ifdef WIN32
		if (timeout < 0)
		{
			timeout = INFINITE;
		}
		int rc = WaitForSingleObject(m_sem, timeout);
		return rc != WAIT_TIMEOUT;
#else
		int rc;
		if (timeout < 0)
		{
			rc = sem_wait(&m_sem);
		}
		else
		{
			struct timespec abstime = CalcAbsTime(timeout);
			rc = sem_timedwait(&m_sem, &abstime);
		}

		return !rc;
#endif
	}

	void Semaphore::Release(int count)
	{
#ifdef WIN32
		::ReleaseSemaphore(m_sem, count, 0);
#else
		sem_post(&m_sem);
#endif
	}
}
