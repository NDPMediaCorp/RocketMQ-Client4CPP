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

#include "SocketUtil.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sstream>

int SocketInit()
{
#ifdef WIN32

	WSADATA wsaData;
	WORD versionRequested;

	versionRequested = MAKEWORD(2, 2);

	if (0 != WSAStartup(versionRequested, &wsaData))
	{
		return -1;
	}

#else

	signal(SIGPIPE, SIG_IGN);

#endif

	return 0;
}

int MakeSocketNonblocking (SOCKET fd)
{
#ifdef WIN32
	unsigned long para = 1;
	return ioctlsocket (fd,FIONBIO,&para);
#else
	int flags = fcntl (fd, F_GETFL, 0);
	assert (flags != -1);
	flags = (flags | O_NONBLOCK);
	return fcntl (fd, F_SETFL, flags);
#endif
}

bool SplitURL(const std::string& serverURL, std::string &addr, short &nPort)
{
	size_t pos = serverURL.find(':');
	if (pos == std::string::npos)
	{
		return false;
	}

	addr = serverURL.substr(0, pos);
	if (0 == addr.compare("localhost"))
	{
		addr = "127.0.0.1";
	}

	pos ++;

	std::string port = serverURL.substr(pos, serverURL.length() - pos);
	nPort = atoi(port.c_str());
	return true;
}

sockaddr string2SocketAddress(const std::string& addrString)
{
	std::string strAddr;
	short port;
	SplitURL(addrString,strAddr,port);

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);

	sa.sin_addr.s_addr = inet_addr(strAddr.c_str());

	sockaddr addr;
	memcpy(&addr,&sa,sizeof(sockaddr));

	return addr;
}

std::string socketAddress2String(sockaddr addr)
{
	sockaddr_in in;
	memcpy(&in,&addr,sizeof(sockaddr));

	return inet_ntoa(in.sin_addr);
}

std::string getLocalAddress()
{
	//TODO
	return "127.0.0.1";
}

std::string getHostName(sockaddr addr)
{
	sockaddr_in in;
	memcpy(&in,&addr,sizeof(sockaddr));
	
	struct hostent *remoteHost = gethostbyaddr((char*) &(in.sin_addr), 4, AF_INET);
	char** alias = remoteHost->h_aliases;
	if (*alias!=0)
	{
		return *alias;
	}
	else
	{
		return inet_ntoa(in.sin_addr);
	}
}
