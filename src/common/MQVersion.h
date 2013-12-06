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

#if!defined __MQVERSION_H__
#define __MQVERSION_H__

#include <string>

/**
* 定义各个版本信息
*
*/
class MQVersion
{
public:
	enum Version
	{
		V3_0_0_SNAPSHOT,
		V3_0_0_ALPHA1,
		V3_0_0_BETA1,
		V3_0_0_BETA2,
		V3_0_0_BETA3,
		V3_0_0_BETA4,
		V3_0_0_BETA5,
		V3_0_0_BETA6_SNAPSHOT,
		V3_0_0_BETA6,
		V3_0_0_BETA7_SNAPSHOT,
		V3_0_0_BETA7,
		V3_0_0_BETA8_SNAPSHOT,
		V3_0_0_BETA8,
		V3_0_0_BETA9_SNAPSHOT,
		V3_0_0_BETA9,
		V3_0_0_FINAL,
		V3_0_1_SNAPSHOT,
		V3_0_1,
		V3_0_2_SNAPSHOT,
		V3_0_2,
		V3_0_3_SNAPSHOT,
		V3_0_3,
		V3_0_4_SNAPSHOT,
		V3_0_4,
		V3_0_5_SNAPSHOT,
		V3_0_5,
		V3_0_6_SNAPSHOT,
		V3_0_6,
		V3_0_7_SNAPSHOT,
		V3_0_7,
	};

	static const char* getVersionDesc(int value);
	static Version value2Version(int value);

public:
	static int s_CurrentVersion;
};

#endif
