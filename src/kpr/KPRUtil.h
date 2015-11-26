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
#if!defined __KPR_UTIL_H__
#define __KPR_UTIL_H__

#ifdef WIN32
#   include <sys/timeb.h>
#   include <process.h>
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/time.h>
#endif

#include <stdlib.h>
#include <chrono>
#include <assert.h>
#include <string>

#ifndef WIN32
struct timespec CalcAbsTime(long timeout);
#endif

unsigned long long GetCurrentTimeMillis();

long long str2ll(const char *str);

#endif
