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
#include "KPRUtil.h"

#include <assert.h>

unsigned long long GetCurrentTimeMillis()
{
#ifdef WIN32
	timeb tb;
	ftime(&tb);
	return tb.time * 1000ULL + tb.millitm;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000ULL+tv.tv_usec/1000;
#endif
}

#ifndef WIN32
struct timespec CalcAbsTime(long timeout)
{
	assert(timeout >= 0);
	struct timeval tv;
	gettimeofday(&tv, 0);

	struct timespec abstime;

	abstime.tv_sec = tv.tv_sec + (timeout/1000);
	abstime.tv_nsec = (tv.tv_usec * 1000) + ((timeout%1000) * 1000000);
	if (abstime.tv_nsec >= 1000000000)
	{
		++abstime.tv_sec;
		abstime.tv_nsec -= 1000000000;
	}

	return abstime;
}
#endif

long long str2ll( const char *str )
{
#ifdef WIN32
	return _atoi64(str);
#else
	return atoll(str);
#endif
}
