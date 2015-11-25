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
#include "Thread.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>

#ifdef WIN32
#   include <sys/timeb.h>
#   include <process.h>
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <signal.h>
#endif

#include "ScopedLock.h"
#include "Exception.h"

namespace kpr
{
	AtomicInteger Thread::s_threadNumber = 0;

#ifdef WIN32
	unsigned __stdcall Thread::ThreadRoute(void* pArg)
#else
	void* Thread::ThreadRoute(void* pArg)
#endif
	{
		Thread_var tv = Duplicate((Thread*)pArg);

		try
		{
			tv -> Startup();
		}
		catch(...)
		{

		}

		try
		{
			tv -> Cleanup();
		}
		catch(...)
		{
		}

		return 0;
	}

	Thread::Thread(const char* name)
	{
		m_name = NULL;
		m_started = false;
		m_threadId = ThreadId();

#ifdef WIN32
		m_handle = 0;
#endif
		m_threadNumber = s_threadNumber++;

		SetName(name);
	}

	Thread::~Thread()
	{
		try
		{
			delete[] m_name;
		}
		catch(...)
		{
		}
	}

	void Thread::SetName(const char* name)
	{
		ScopedLock<Mutex> guard(m_mutex);

		char* tmp;

		//如果name为空指针，则用内部名称，Thread-threadNumber
		if (name == NULL)
		{
			char buf[32];
			sprintf(buf, "Thread-%u", m_threadNumber);
			tmp = new char[strlen(buf) + 1];
			strcpy(tmp, buf);
		}
		else
		{
			tmp = new char[strlen(name) + 1];
			strcpy(tmp, name);
		}

		delete[] m_name;
		m_name = tmp;
	}

	const char* Thread::GetName() const
	{
		ScopedLock<Mutex> guard(m_mutex);
		return m_name;
	}

	const char* Thread::GetNameWithLockHeld() const {
		return m_name;
	}

	void Thread::Start()
	{
		ScopedLock<Mutex> guard(m_mutex);

		if (m_started)
		{
			return;
		}

#ifdef WIN32
		DWORD id;
		m_handle = (HANDLE)::_beginthreadex(NULL, NULL,ThreadRoute, (LPVOID)this, 0, (unsigned int*)&id);

		if(m_handle==NULL)
		{
			THROW_EXCEPTION(SystemCallException,"_beginthreadex error",GetLastError())
		}

		m_threadId = ThreadId(id);
#else
		pthread_attr_t attr;
		int retcode = 0;
		retcode = pthread_attr_init(&attr);
		if(retcode != 0)
		{
			THROW_EXCEPTION(SystemCallException,"pthread_attr_init failed!",errno)
		}

		pthread_t id;

		retcode = pthread_create(&id, &attr, ThreadRoute, (void *)this);
		if(retcode != 0)
		{
			THROW_EXCEPTION(SystemCallException,"pthread_create error",errno)
		}

		m_threadId = id;
		pthread_attr_destroy(&attr);
#endif
		m_started = true;

		std::string service_name(GetNameWithLockHeld());
		Logger::get_logger()->info("{} start successfully", service_name);
	}

	void Thread::Run()
	{
		//TODO support runable
	}

	bool Thread::IsAlive() const
	{
		if(m_started)
		{
#ifdef WIN32

#else
			int retcode = pthread_kill(m_threadId, 0);
			return (retcode == ESRCH);
#endif
		}

		return false;
	}

	void  Thread::Join()
	{
		if (m_started)
		{
#ifdef WIN32
			WaitForSingleObject(m_handle, INFINITE );
#else
			pthread_join(m_threadId, NULL);
#endif
		}
	}

	void Thread::Sleep(long millis, int nanos)
	{
		assert(millis >= 0&&nanos >=0&&nanos < 999999);

#ifdef WIN32
		::Sleep(millis);
#else
		struct timespec tv;
		tv.tv_sec = millis / 1000;
		tv.tv_nsec = (millis % 1000) * 1000000 + nanos;
		nanosleep(&tv, 0);
#endif
	}

	void Thread::Yield()
	{
#ifdef WIN32
		Sleep(0);
#else
		pthread_yield();
#endif
	}

	ThreadId Thread::GetId() const
	{
		ScopedLock<Mutex> guard(m_mutex);
		return m_threadId;
	}

	void Thread::Startup()
	{
		try
		{
			Run();
		}
		catch(...)
		{

		}
	}

	void Thread::Cleanup()
	{

	}
}
