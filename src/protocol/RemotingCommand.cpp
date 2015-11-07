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

#include "RemotingCommand.h"

#include <sstream>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <json/json.h>

#include "SocketUtil.h"
#include "CommandCustomHeader.h"
#include "MQVersion.h"


AtomicInteger RemotingCommand::s_seqNumber = 0;
volatile int RemotingCommand::s_configVersion=MQVersion::s_CurrentVersion;

RemotingCommand::RemotingCommand(int code)
	:m_code(code),m_language("CPP"),m_version(0),m_opaque(s_seqNumber++),
	 m_flag(0),m_remark(""),m_pCustomHeader(NULL),m_headLen(0),m_pHead(NULL),
	 m_bodyLen(0),m_pBody(NULL)
{
}

RemotingCommand::RemotingCommand( int code,
								  const std::string& language,
								  int version,
								  int opaque,
								  int flag,
								  const std::string& remark,
								  CommandCustomHeader* pCustomHeader )
	:m_code(code),m_language(language),m_version(version),m_opaque(opaque),
	m_flag(flag),m_remark(remark),m_pCustomHeader(pCustomHeader),
	m_headLen(0),m_pHead(NULL),m_bodyLen(0),m_pBody(NULL),m_releaseBody(false)
{

}

RemotingCommand::~RemotingCommand()
{
	delete[] m_pHead;

	if (m_releaseBody)
	{
		delete[] m_pBody;
		m_bodyLen=0;
		m_pBody = NULL;
	}

	delete m_pCustomHeader;
}

void RemotingCommand::Encode()
{
	std::string extHeader="{}";
	if (m_pCustomHeader)
	{
		m_pCustomHeader->Encode(extHeader);
	}

	std::stringstream ss;
	ss<<"{"<<CODE_STRING<<m_code<<","
	  <<language_STRING<<"\"CPP\","
	  <<version_STRING<<m_version<<","
	  <<opaque_STRING<<m_opaque<<","
	  <<flag_STRING<<m_flag<<","
	  <<remark_STRING<<"\""<<m_remark<<"\""<<","
	  <<extFields_STRING<<extHeader<<"}";

	int headLen=ss.str().size();

	m_headLen = 4+4+ headLen;

	m_pHead = new char[m_headLen];

	int tmp = htonl(m_headLen+m_bodyLen-4);

	memcpy(m_pHead,&tmp,4);

	tmp = htonl(headLen);
	memcpy(m_pHead+4,&tmp,4);

	memcpy(m_pHead+8,ss.str().c_str(),headLen);
}

const char* RemotingCommand::GetHead()
{
	return m_pHead;
}

int RemotingCommand::GetHeadLen()
{
	return m_headLen;
}

void RemotingCommand::SetData(char* pData,int len)
{
	m_pHead = pData;
	m_headLen = len;
}

const char* RemotingCommand::GetBody()
{
	return m_pBody;
}

int RemotingCommand::GetBodyLen()
{
	return m_bodyLen;
}

void RemotingCommand::SetBody(char* pData,int len,bool copy)
{
	m_releaseBody = copy;

	if (copy)
	{
		m_pBody = new char[len];
		m_bodyLen = len;
		memcpy(m_pBody,pData,len);
	}
	else
	{
		m_pBody = pData;
		m_bodyLen = len;
	}
}

RemotingCommand* RemotingCommand::Decode(char* pData,int len)
{
	//½âÎöheader

	Json::Reader reader;
	Json::Value object;
	if (!reader.parse(pData+8, object))
	{
		return NULL;
	}

	int code = object["code"].asInt();
	std::string language = object["language"].asString();
	int version = object["version"].asInt();
	int opaque =object["opaque"].asInt();
	int flag = object["flag"].asInt();

	Json::Value v = object["remark"];
	std::string remark="";
	if (!v.isNull())
	{
		remark = object["remark"].asString();
	}
	
	RemotingCommand* cmd = new RemotingCommand(code,
			language,
			version,
			opaque,
			flag,
			remark,
			NULL);

	int headLen;
	memcpy(&headLen,pData+4,4);
	headLen = ntohl(headLen);

	int bodyLen = len - 8 - headLen;

	if (bodyLen>0)
	{
		cmd->SetBody(pData+8 + headLen,bodyLen,true);
	}

	return cmd;
}

RemotingCommand* RemotingCommand::CreateRemotingCommand(const char* pData,int len )
{
	RemotingCommand* cmd = Decode((char*)pData,len);

	return cmd;
}

RemotingCommand* RemotingCommand::createRequestCommand( int code, CommandCustomHeader* pCustomHeader )
{
	RemotingCommand* cmd = new RemotingCommand(code);
	cmd->setCommandCustomHeader(pCustomHeader);
	setCmdVersion(cmd);

	return cmd;
}

void RemotingCommand::markResponseType()
{
	int bits = 1 << RPC_TYPE;
	m_flag |= bits;
}

bool RemotingCommand::isResponseType()
{
	int bits = 1 << RPC_TYPE;
	return (m_flag & bits) == bits;
}

void RemotingCommand::markOnewayRPC()
{
	int bits = 1 << RPC_ONEWAY;
	m_flag |= bits;
}

bool RemotingCommand::isOnewayRPC()
{
	int bits = 1 << RPC_ONEWAY;
	return (m_flag & bits) == bits;
}

void RemotingCommand::setCmdVersion(RemotingCommand* pCmd)
{
	if (s_configVersion >= 0)
	{
		pCmd->setVersion(s_configVersion);
	}
	else
	{
		int value = MQVersion::s_CurrentVersion;
		pCmd->setVersion(value);
		s_configVersion = value;
	}
}

void RemotingCommand::MakeCustomHeader( int code,const char* pData,int len )
{
	const char* p = strstr(pData+8,extFields_STRING.c_str());

	if(p)
	{
		CommandCustomHeader* customHeader = CommandCustomHeader::Decode(code,(char*)pData,len, isResponseType());
		setCommandCustomHeader(customHeader);
	}
}

int RemotingCommand::getCode()
{
	return m_code;
}

void RemotingCommand::setCode(int code)
{
	m_code = code;
}

std::string RemotingCommand::getLanguage()
{
	return m_language;
}

void RemotingCommand::setLanguage(const std::string& language)
{
	m_language = language;
}

int RemotingCommand::getVersion()
{
	return m_version;
}

void RemotingCommand::setVersion(int version)
{
	m_version = version;
}

int RemotingCommand::getOpaque()
{
	return m_opaque;
}

void RemotingCommand::setOpaque(int opaque)
{
	m_opaque = opaque;
}

int RemotingCommand::getFlag()
{
	return m_flag;
}

void RemotingCommand::setFlag(int flag)
{
	m_flag = flag;
}

std::string RemotingCommand::getRemark()
{
	return m_remark;
}

void RemotingCommand::setRemark(const std::string& remark)
{
	m_remark = remark;
}

void RemotingCommand::setCommandCustomHeader(CommandCustomHeader* pCommandCustomHeader)
{
	m_pCustomHeader = pCommandCustomHeader;
}

CommandCustomHeader* RemotingCommand::getCommandCustomHeader()
{
	return m_pCustomHeader;
}

RemotingCommandType RemotingCommand::getType()
{
	if (isResponseType())
	{
		return RESPONSE_COMMAND;
	}

	return REQUEST_COMMAND;
}
