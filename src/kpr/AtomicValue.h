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
#if!defined __KPR_ATOMICVALUE_H__
#define __KPR_ATOMICVALUE_H__

#include "KPRTypes.h"

#ifdef WIN32

template <class T>
class AtomicValue
{
public:
	AtomicValue(T init=0) : value(init) {}

	AtomicValue<T>& operator=(T newValue)
	{
		Set(newValue);
		return *this;
	}
	
	AtomicValue<T>& operator=(const AtomicValue<T>& v)
	{
		Set(v.Get());
		return *this;
	}

	inline T operator+=(T n)
	{
		return InterlockedExchangeAdd(&value,n)+n;
	}

	inline T operator-=(T n)
	{
		return InterlockedExchangeAdd(&value,-1*n)-n;
	}

	inline T operator++()
	{
		return InterlockedIncrement((T*)&value);
	}

	inline T operator--()
	{
		return InterlockedDecrement((T*)&value);
	}

	inline T fetchAndAdd(T n)
	{
		return InterlockedExchangeAdd((T*)&value,n);
	}

	inline T fetchAndSub(T n)
	{
		return InterlockedExchangeAdd((T*)&value,-1*n);
	}

	inline T CompareAndSet(T comparand,T exchange)
	{
		return InterlockedCompareExchange((T*)&value, exchange, comparand);
	}

	inline T operator++(int)
	{
		return fetchAndAdd(1);
	}

	inline T operator--(int)
	{
		return fetchAndSub(1);
	}

	operator T () const
	{
		return Get();
	}

	T Get() const
	{
		return const_cast<AtomicValue<T>*>(this)->fetchAndAdd(static_cast<T>(0));
	}

	void Set(T n)
	{
		InterlockedExchange((T*)&value,n);
	}

private:
	T value;
};

typedef AtomicValue<long> AtomicInteger;
typedef AtomicValue<long> AtomicLong;

#else
template <class T>
class AtomicValue
{
public:
	AtomicValue(T init=0) : value(init) {}

	AtomicValue<T>& operator=(T newValue)
	{
		Set(newValue);
		return *this;
	}

	AtomicValue<T>& operator=(const AtomicValue<T>& v)
	{
		Set(v.Get());

		return *this;
	}

	inline T operator+=(T n)
	{
		return __sync_add_and_fetch(&value, n);
	}

	inline T operator-=(T n)
	{
		return __sync_sub_and_fetch(&value, n);
	}

	inline T operator++()
	{
		return *this += 1;
	}

	inline T operator--()
	{
		return *this -= 1;
	}

	inline T fetchAndAdd(T n)
	{
		return __sync_fetch_and_add(&value, n);
	}

	inline T fetchAndSub(T n)
	{
		return __sync_fetch_and_sub(&value, n);
	}

	inline T operator++(int)
	{
		return fetchAndAdd(1);
	}

	inline T operator--(int)
	{
		return fetchAndSub(1);
	}

	operator T () const
	{
		return Get();
	}

	T Get() const
	{
		return const_cast<AtomicValue<T>*>(this)->fetchAndAdd(static_cast<T>(0));
	}

	void Set(T n)
	{
		__sync_lock_test_and_set((T*)&value,n);
	}

	inline T CompareAndSet(T comparand,T exchange)
	{
		return __sync_val_compare_and_swap((T*)&value, comparand, exchange);
	}

private:
	T value;
};

typedef AtomicValue<int> AtomicInteger;
typedef AtomicValue<long> AtomicLong;

#endif

#endif
