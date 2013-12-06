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
#ifndef __KPR_REFHANDLET_H__
#define __KPR_REFHANDLET_H__

#include "KPRTypes.h"

template <class T>
T* Duplicate(T *pRef)
{
	if(pRef)
	{
		pRef->IncRef();
	}
	return pRef;
}

template <class T>
void Release(T *pRef)
{
	if(pRef)
	{
		pRef->DecRef();
	}
}

template <class T>
class RefHandleT
{
public:
	RefHandleT(T* p = 0)
		:m_ptr(p)
	{
	}

	RefHandleT(const RefHandleT<T>& v)
		:m_ptr(v.m_ptr)
	{
		m_ptr->IncRef();
	}

	~RefHandleT()
	{
		Release(m_ptr);
	}

	RefHandleT<T>& operator=(const RefHandleT<T>& v)
	{
		if (m_ptr != v.m_ptr)
		{
			Release(m_ptr);

			m_ptr = v.m_ptr;

			Duplicate(m_ptr);
		}

		return *this;
	}

	RefHandleT<T>& operator=(T* p)
	{
		if (m_ptr != p)
		{
			Release(m_ptr);
			m_ptr = p;
		}

		return *this;
	}

	T* operator->() const
	{
		return m_ptr;
	}

	T& operator*()
	{
		return *m_ptr;
	}

	operator T*() const
	{
		return m_ptr;
	}

	T* ptr() const
	{
		return m_ptr;
	}

	T* retn()
	{
		T *p = m_ptr;
		m_ptr = 0;

		return p;
	}

	bool operator==(const RefHandleT<T>& v) const
	{
		return m_ptr == v.m_ptr;
	}

	bool operator==(T* p) const
	{
		return m_ptr == p;
	}

	bool operator!=(const RefHandleT<T>& v) const
	{
		return m_ptr != v.m_ptr;
	}

	bool operator!=(T* p) const
	{
		return m_ptr != p;
	}

	bool  operator!() const
	{
		return m_ptr == 0;
	}

	operator bool() const
	{
		return m_ptr != 0;
	}

private:
	T* m_ptr;
};

#define DECLAREVAR(T)   typedef RefHandleT<T> T ## _var;\
	typedef T* T ## _ptr;

#define DECLAREPTR(T)   typedef T* T ## _ptr;

#endif
