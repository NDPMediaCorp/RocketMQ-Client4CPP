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
#include "MessageDecoder.h"

const char MessageDecoder::NAME_VALUE_SEPARATOR = 1;
const char MessageDecoder::PROPERTY_SEPARATOR = 2;
const int MessageDecoder::MSG_ID_LENGTH = 8 + 8;

int MessageDecoder::MessageMagicCodePostion = 4;
int MessageDecoder::MessageFlagPostion = 16;
int MessageDecoder::MessagePhysicOffsetPostion = 28;
int MessageDecoder::MessageStoreTimestampPostion = 56;

std::string MessageDecoder::createMessageId(const char* pInput,
											int inputLen,
											const char* pAddr,
											int addrLen,
											long long offset)
{
	return "";
}

MessageId MessageDecoder::decodeMessageId(const std::string& msgId)
{
	sockaddr address;
	MessageId id(address,0);
	return id;
}

MessageExt* MessageDecoder::decode(const char* pData, int len)
{
	return NULL;
}

MessageExt* MessageDecoder::decode(const char* pData, int len, bool readBody)
{
	return NULL;
}

std::list<MessageExt*> MessageDecoder::decodes(const char* pData, int len)
{
	std::list<MessageExt*> list;
	return list;
}

std::list<MessageExt*> MessageDecoder::decodes(const char* pData, int len, bool readBody)
{
	std::list<MessageExt*> list;
	return list;
}

std::string MessageDecoder::messageProperties2String(const std::map<std::string, std::string>& properties)
{
	return "";
}

std::map<std::string, std::string> MessageDecoder::string2messageProperties(std::string properties)
{
	std::map<std::string, std::string> prop;

	return prop;
}
