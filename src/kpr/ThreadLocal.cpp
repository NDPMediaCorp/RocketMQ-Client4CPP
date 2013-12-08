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
#include "ThreadLocal.h"

#include <errno.h>

#include "Exception.h"

namespace kpr
{
	ThreadLocal::ThreadLocal()
		: m_Key(0)
	{
#ifdef WIN32
		m_Key = TlsAlloc();
#else
		int retcode = 0;

		retcode = pthread_key_create(&m_Key, 0);
		if(retcode != 0)
		{
			THROW_EXCEPTION(SystemCallException,"pthread_key_create error",errno);
		}
#endif
	}

	ThreadLocal::~ThreadLocal()
	{
#ifdef WIN32
		TlsFree(m_Key);
#else
		pthread_key_delete(m_Key);
#endif
	}

	void* ThreadLocal::GetValue()
	{
		void* v;
#ifdef WIN32
		v = TlsGetValue(m_Key);
#else
		v = pthread_getspecific(m_Key);
#endif
		return v;
	}

	void ThreadLocal::SetValue(void * value)
	{
#ifdef WIN32
		TlsSetValue( m_Key, value);
#else
		int retcode = pthread_setspecific(m_Key, value);
		if(retcode != 0)
		{
			THROW_EXCEPTION(SystemCallException,"pthread_setspecific error",errno);
		}
#endif
	}
}
