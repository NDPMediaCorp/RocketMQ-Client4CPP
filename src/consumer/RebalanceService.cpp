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

#include "RebalanceService.h"
#include "MQClientFactory.h"

long RebalanceService::s_WaitInterval = 1000 * 10;

RebalanceService::RebalanceService(MQClientFactory* pMQClientFactory)
	:ServiceThread("RebalanceService"),
	 m_pMQClientFactory(pMQClientFactory)
{
}


RebalanceService::~RebalanceService()
{

}

void RebalanceService::Run()
{
	while (!m_stoped)
	{
		waitForRunning(s_WaitInterval);
		m_pMQClientFactory->doRebalance();
	}
}

std::string RebalanceService::getServiceName()
{
	return "RebalanceService";
}

