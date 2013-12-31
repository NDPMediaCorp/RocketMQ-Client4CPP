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
#if!defined __KPR_TIMERTASKMANAGER_H__
#define __KPR_TIMERTASKMANAGER_H__

#include <list>
#include <map>

#include "TimerThread.h"

class ThreadPoolWork;

namespace kpr
{
	class ThreadPool;

	class TimerTaskManager : public TimerHandler
	{
	public:
		TimerTaskManager();
		virtual ~TimerTaskManager();

		int Init(int maxThreadCount, int checklnteval);
		unsigned int RegisterTimer(unsigned int elapse, ThreadPoolWork* pHandler);
		bool UnRegisterTimer(unsigned int timerId);
		bool ResetTimer(unsigned int timerId);
		void Close();

		virtual void OnTimeOut(unsigned int timerId);

	private:
		std::map<unsigned int, ThreadPoolWork*> m_timerTasks;
		kpr::Mutex m_mutex;
		unsigned int m_checklnterval;

		TimerThread_var m_timerThread;
		kpr::ThreadPool* m_pThreadPool;
	};
}
#endif
