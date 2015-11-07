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
#if!defined __CONSUMETYPE_H__
#define __CONSUMETYPE_H__

#include "RocketMQClient.h"

/**
 * 消费类型
 *
 */
enum ConsumeType
{
	/**
	* 主动方式消费
	*/
	CONSUME_ACTIVELY,
	/**
	* 被动方式消费
	*/
	CONSUME_PASSIVELY,
};

enum ConsumeFromWhere
{
	/**
	* 每次启动都从上次记录的位点开始消费，如果是第一次启动则从最大位点开始消费，建议在生产环境使用
	*/
	CONSUME_FROM_LAST_OFFSET,
	/**
	* 每次启动都从上次记录的位点开始消费，如果是第一次启动则从最小位点开始消费，建议测试时使用<br>
	* 线上环境此配置项可能需要审核，否则无效
	*/
	CONSUME_FROM_LAST_OFFSET_AND_FROM_MIN_WHEN_BOOT_FIRST,
	/**
	* 每次启动都从最小位点开始消费，建议测试时使用<br>
	* 线上环境此配置项可能需要审核，否则无效
	*/
	CONSUME_FROM_MIN_OFFSET,
	/**
	* 每次启动都从最大位点开始消费，建议测试时使用
	*/
	CONSUME_FROM_MAX_OFFSET,
};

enum MessageModel
{
	/**
	* 广播模型
	*/
	BROADCASTING,
	/**
	* 集群模型
	*/
	CLUSTERING,
	// /**
	// * 未知，如果是主动消费，很难确定应用的消息模型
	// */
	// UNKNOWNS,
};

ROCKETMQCLIENT_API const char* getConsumeTypeString(ConsumeType type);
ROCKETMQCLIENT_API const char* getConsumeFromWhereString(ConsumeFromWhere type);
ROCKETMQCLIENT_API const char* getMessageModelString(MessageModel type);

#endif
