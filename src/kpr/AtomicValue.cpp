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

#include "AtomicValue.h"

#ifdef WIN32

//  InterlockedExchangeAdd
long MyInterlockedExchangeAdd(long volatile* addend,long value)
{
	return InterlockedExchangeAdd(addend,value);
}

long long MyInterlockedExchangeAdd(long long volatile* addend,long long value)
{
	return InterlockedExchangeAdd64(addend,value);
}

// InterlockedIncrement
long MyInterlockedIncrement(long volatile* addend)
{
	return InterlockedIncrement(addend);
}

long long MyInterlockedIncrement(long long volatile* addend)
{
	return InterlockedIncrement64(addend);
}

// InterlockedDecrement
long MyInterlockedDecrement(long volatile* addend)
{
	return InterlockedDecrement(addend);
}

long long MyInterlockedDecrement(long long volatile* addend)
{
	return InterlockedDecrement64(addend);
}

// InterlockedExchange
long MyInterlockedExchange(long volatile* target,long value)
{
	return InterlockedExchange(target,value);
}

long long MyInterlockedExchange(long long volatile* target,long long value)
{
	return InterlockedExchange64(target,value);
}

// InterlockedCompareExchange
long MyInterlockedCompareExchange(long volatile* destination,
										long exchange,
										long comparand)
{
	return InterlockedCompareExchange(destination, exchange, comparand);
}

long long MyInterlockedCompareExchange(long long volatile* destination,
	long long exchange,
	long long comparand)
{
	return InterlockedCompareExchange64(destination, exchange, comparand);
}

#endif
