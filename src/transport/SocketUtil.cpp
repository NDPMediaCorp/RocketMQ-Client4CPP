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
#include <vector>
#include <iostream>

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

int SetTcpNoDelay(SOCKET fd)
{
	int flag = 1;
	return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (const char*)&flag, sizeof(flag));
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
	} else {
		addr = MixAll::filterIP(addr);
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

void GetLocalAddrs(std::vector<unsigned int> &addrs)
{
	addrs.clear();

#ifdef WIN32
	WSADATA wsaData;
	int ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != 0) {
		return;
	}

	SOCKET sfd = socket(AF_INET, SOCK_DGRAM, 0);

	if(sfd != INVALID_SOCKET)
	{
		DWORD bytesRet = 0;
		char * buffer;
		int bufSize = 1024;
		do
		{
			buffer = (char*)malloc(bufSize);
			if (WSAIoctl(sfd, SIO_GET_INTERFACE_LIST, NULL, 0, buffer, bufSize, &bytesRet, NULL, NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() == WSAEFAULT)
				{
					bufSize <<= 1;
					free(buffer);
					continue;
				}
				else
				{
					bytesRet = 0;
					break;
				}
			}
			else
			{
				break;
			}
		}while (true);

		int size = bytesRet / sizeof(INTERFACE_INFO);
		INTERFACE_INFO* iinfo = (INTERFACE_INFO*)buffer;

		for (int i = 0; i < size; i++)
		{
			if (iinfo[i].iiAddress.AddressIn.sin_family == AF_INET)
			{
				addrs.push_back(ntohl(iinfo[i].iiAddress.AddressIn.sin_addr.s_addr));
			}
		}

		free(buffer);
		buffer = NULL;
		closesocket(sfd);
	}
#else

	struct ifconf ifc;
	ifc.ifc_buf = NULL;
	ifc.ifc_len = 0;

	int sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd != INVALID_SOCKET)
	{
		int ret = ioctl(sfd, SIOCGIFCONF, (char *)&ifc);

		if(ret != -1)
		{
			ifc.ifc_req=(struct ifreq *)malloc(ifc.ifc_len);
			ret = ioctl(sfd, SIOCGIFCONF, (char *)&ifc);
			if(ret != -1)
			{
				for(int i=0; i<ifc.ifc_len/sizeof(struct ifreq); i++)
				{
					struct sockaddr* sa = (struct sockaddr *)&(ifc.ifc_req[i].ifr_addr);
					if (AF_INET==sa->sa_family)
					{
						unsigned int addr = ((struct sockaddr_in *)sa)->sin_addr.s_addr;
						addrs.push_back(htonl(addr));
					}				
				}			
			}

			free(ifc.ifc_req);
			ifc.ifc_req = NULL;
		}

		close(sfd);
	}

#endif

	if(addrs.empty())
	{		
		char hostname[1024];

		int ret = gethostname(hostname, sizeof(hostname));
		if (ret == 0)
		{
			struct addrinfo *result = NULL;
			struct addrinfo *ptr = NULL;
			struct addrinfo hints;

			memset(&hints, 0,sizeof(hints) );
			hints.ai_family = AF_INET;
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;

			ret = getaddrinfo(hostname, NULL, &hints, &result);
			if ( ret == 0 ) 
			{
				for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) 
				{
					
					struct sockaddr_in  *sockaddr_ipv4 = (struct sockaddr_in *) ptr->ai_addr;
					addrs.push_back(ntohl(sockaddr_ipv4->sin_addr.s_addr));
				}
			}

			freeaddrinfo(result);
		}
	}

	std::vector<unsigned int>::iterator it = addrs.begin();
	for (;it!=addrs.end();)
	{
		if (*it>= 0x7F000000U && *it < 0x80000000U)
		{
			it = addrs.erase(it);
		}
		else
		{
			it++;
		}
	}

	if (addrs.empty())
	{
		addrs.push_back(INADDR_LOOPBACK);
	}

#ifdef WIN32
		WSACleanup();
#endif
}

std::string getLocalAddress()
{
	std::vector<unsigned int> addrs;
	GetLocalAddrs(addrs);
	struct in_addr addr;
	addr.s_addr=htonl(addrs[0]);

	return inet_ntoa(addr);
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


unsigned long long swapll(unsigned long long v)
{
#ifdef ENDIANMODE_BIG
	return v;
#else
	unsigned long long ret = ((v << 56) 
								| ((v & 0xff00) << 40)
								| ((v & 0xff0000) << 24)
								| ((v & 0xff000000) << 8)
								| ((v >> 8 ) & 0xff000000)
								| ((v >> 24 ) & 0xff0000)
								| ((v >> 40 ) & 0xff00)
								| (v >> 56 ));

	return ret;
#endif
}

unsigned long long h2nll(unsigned long long v)
{
	return swapll(v);
}

unsigned long long n2hll(unsigned long long v)
{
	return swapll(v);
}

std::string socketAddress2IPPort( sockaddr addr )
{
	sockaddr_in in;
	memcpy(&in,&addr,sizeof(sockaddr));

	char tmp[32];
	sprintf(tmp,"%s:%d",inet_ntoa(in.sin_addr),ntohs(in.sin_port));

	std::string ipport = tmp;
	return ipport;
}

void load_certificate(SSL_CTX* ctx, char const * cacert, char const * cakey, char const * trustCALocation) {
    if (SSL_CTX_use_certificate_file(ctx, cacert, SSL_FILETYPE_PEM) <= 0) {
		Logger::get_logger()->error("Failed to apply public certeificate");
        abort();
    }

    if(SSL_CTX_use_PrivateKey_file(ctx, cakey, SSL_FILETYPE_PEM) <= 0) {
		Logger::get_logger()->error("Failed to apply private key.");
        abort();
    }

    if (SSL_CTX_check_private_key(ctx) <= 0) {
		Logger::get_logger()->error("Private key does not match the public certificate.");
        abort();
    }

    /*
     * @param CAfile
     * A pointer to the name of the file that contains the certificates of the trusted CAs and CRLs.
     * The file must be in base64 privacy enhanced mail (PEM) format.
     * The value of this parameter can be NULL if the value of the CApath parameter is not NULL.
     * The maximum length is 255 characters.
     *
     * @param CApath
     * A pointer to the name of the directory that contains the certificates of the trusted CAs and CRLs.
     * The files in the directory must be in PEM format.
     * The value of this parameter can be NULL if the value of the CAfile parameter is not NULL.
     * The maximum length is 255 characters.
     */
    SSL_CTX_load_verify_locations(ctx, NULL, trustCALocation);
    SSL_CTX_set_verify_depth(ctx, 5);
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
	Logger::get_logger()->info("Load Certificate Successfully");
}

SSL_CTX* initializeSSL() {
	SSL_library_init();
	SSL_load_error_strings();
	ERR_load_BIO_strings();
	OpenSSL_add_all_algorithms(); // load and register all encryption algorithms.
	SSL_CTX* ctx = SSL_CTX_new(SSLv23_method());
	char const * client_certificate = "/dianyi/config/RocketMQ/SSL/client.pem";
	char const * client_private_key = "/dianyi/config/RocketMQ/SSL/client.pkey";
	char const * cacerts = "/dianyi/config/RocketMQ/SSL/";

    load_certificate(ctx, //SSL_CTX*
					 client_certificate, // public CA certificate
                     client_private_key, // private CA  key
                     cacerts // trust CA certificates

    );

	return ctx;
}

void destroySSL() {
	ERR_free_strings();
	EVP_cleanup();
}

void show_certificate(SSL* ssl) {
    X509 *cert;
    char *line;
    cert = SSL_get_peer_certificate(ssl); /* Get certificates (if available) */
    if ( cert != NULL ) {
		Logger::get_logger()->info("Server certificates:");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		Logger::get_logger()->info("Subject: {}", line);
		free(line);
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		Logger::get_logger()->info("Issuer: {}", line);
        free(line);
        X509_free(cert);
    } else {
		Logger::get_logger()->warn("No certificates.");
    }
}

void shutdownSSL(SSL * ssl) {
	SSL_shutdown(ssl);
	SSL_free(ssl);
}