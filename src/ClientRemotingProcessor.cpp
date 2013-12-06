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
#include "ClientRemotingProcessor.h"
#include "MQProtos.h"
#include "RemotingCommand.h"
#include "MQClientFactory.h"

ClientRemotingProcessor::ClientRemotingProcessor(MQClientFactory* pMQClientFactory)
	:m_pMQClientFactory (pMQClientFactory)
{
	
}

RemotingCommand* ClientRemotingProcessor::processRequest(RemotingCommand* pRequest)
{
	int code = pRequest->getCode();
	switch (code)
	{
	case CHECK_TRANSACTION_STATE_VALUE:
		return checkTransactionState(pRequest);
	case NOTIFY_CONSUMER_IDS_CHANGED_VALUE:
		return notifyConsumerIdsChanged(pRequest);
	default:
		break;
	}

	return NULL;
}

RemotingCommand* ClientRemotingProcessor::checkTransactionState(RemotingCommand* request)
{
	//TODO
	return NULL;
}

RemotingCommand* ClientRemotingProcessor::notifyConsumerIdsChanged(RemotingCommand* pRequest)
{
	//TODO
	return NULL;
}
