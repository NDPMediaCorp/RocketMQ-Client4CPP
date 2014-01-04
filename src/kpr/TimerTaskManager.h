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
#include "ThreadPoolWork.h"

class TimerTask : public ThreadPoolWork
{
public:
	TimerTask()
		:m_isProcessing(false)
	{

	}

	virtual void Do()
	{
		DoTask();
		m_isProcessing = false;
	}

	bool IsProcessing()
	{
		return m_isProcessing;
	}

	void SetProcessing(bool isProcessing)
	{
		m_isProcessing = isProcessing;
	}

	virtual void DoTask()=0;

private:
	bool m_isProcessing;
};

namespace kpr
{
	class ThreadPool;

	class TimerTaskManager : public TimerHandler
	{
	public:
		TimerTaskManager();
		virtual ~TimerTaskManager();

		int Init(int maxThreadCount, int checklnteval);
		unsigned int RegisterTimer(unsigned int initialDelay, unsigned int elapse, TimerTask* pHandler);
		bool UnRegisterTimer(unsigned int timerId);
		bool ResetTimer(unsigned int timerId);
		void Close();

		virtual void OnTimeOut(unsigned int timerId);

	private:
		std::map<unsigned int, TimerTask*> m_timerTasks;
		kpr::Mutex m_mutex;
		unsigned int m_checklnterval;

		TimerThread_var m_timerThread;
		kpr::ThreadPool* m_pThreadPool;
	};
}
#endif
