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
#if!defined __SOCKETUTIL_H__
#define __SOCKETUTIL_H__

#ifdef WIN32
#include <Winsock2.h>
#include <Windows.h>

#define NET_ERROR WSAGetLastError()
#define socklen_t int
#define SocketUninit() WSACleanup()

#pragma comment(lib,"ws2_32.lib")
#else

#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>

#define NET_ERROR errno
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define WSAECONNRESET ECONNRESET
#define WSAEWOULDBLOCK EWOULDBLOCK
#define WSAEINPROGRESS EINPROGRESS
#define WSAEBADF EBADF
#define closesocket close
#define SD_SEND SHUT_WR
#define SD_RECEIVE SHUT_RD
#define SD_BOTH SHUT_RDWR
typedef int SOCKET;

#define SocketUninit()

#endif

#include <string>

int SocketInit();
int MakeSocketNonblocking (SOCKET fd);
bool SplitURL(const std::string& serverURL, std::string &addr, short &nPort);
/**
* IP:PORT
*/
sockaddr string2SocketAddress(const std::string& addr);
std::string socketAddress2String(sockaddr addr);
std::string getHostName(sockaddr addr);
std::string getLocalAddress();
#endif
