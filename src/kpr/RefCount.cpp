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
#include "RefCount.h"
#include<assert.h>

namespace kpr
{
	RefCount::RefCount()
		: m_refCount(1)
	{
	}

	RefCount::~RefCount()
	{
	}

	void RefCount::IncRef()
	{
		assert(m_refCount >= 1);
		m_refCount++;
	}

	void RefCount::DecRef()
	{
		assert(m_refCount > 0);
		if(--m_refCount == 0)
		{
			delete this;
		}
	}
}
