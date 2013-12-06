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

#include "MQVersion.h"

// TODO 每次发布版本都要修改此处版本号
int MQVersion::s_CurrentVersion = MQVersion::V3_0_2;

const char* MQVersion::getVersionDesc(int value)
{
	switch(value)
	{
	//V3_0_0_ALPHA1,
	//V3_0_0_BETA1,
	//V3_0_0_BETA2,
	//V3_0_0_BETA3,
	//V3_0_0_BETA4,
	//V3_0_0_BETA5,
	//V3_0_0_BETA6_SNAPSHOT,
	//V3_0_0_BETA6,
	//V3_0_0_BETA7_SNAPSHOT,
	//V3_0_0_BETA7,
	//V3_0_0_BETA8_SNAPSHOT,
	//V3_0_0_BETA8,
	case V3_0_0_BETA9_SNAPSHOT:
		return "V3_0_0_BETA9_SNAPSHOT";

	case V3_0_0_BETA9:
		return "V3_0_0_BETA9";

	case V3_0_0_FINAL:
		return "V3_0_0_FINAL";

	case V3_0_1_SNAPSHOT:
		return "V3_0_1_SNAPSHOT";

	case V3_0_1:
		return "V3_0_1";

	case V3_0_2_SNAPSHOT:
		return "V3_0_2_SNAPSHOT";

	case V3_0_2:
		return "V3_0_2";

	case V3_0_3:
		return "V3_0_3";

	case V3_0_4_SNAPSHOT:
		return "V3_0_4_SNAPSHOT";

	case V3_0_4:
		return "V3_0_4";

	case V3_0_5_SNAPSHOT:
		return "V3_0_5_SNAPSHOT";

	case V3_0_5:
		return "V3_0_5";

	case V3_0_6_SNAPSHOT:
		return "V3_0_6_SNAPSHOT";

	case V3_0_6:
		return "V3_0_6";

	case V3_0_7_SNAPSHOT:
		return "V3_0_7_SNAPSHOT";

	case V3_0_7:
		return "V3_0_7";
	}

	return "";
}


MQVersion::Version MQVersion::value2Version(int value)
{
	return (MQVersion::Version)value;
}

