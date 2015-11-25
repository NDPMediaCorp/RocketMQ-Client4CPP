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


#include "MixAll.h"

long MixAll::MASTER_ID = 0L;
const std::string MixAll::DEFAULT_TOPIC = "TBW102";
const std::string MixAll::BENCHMARK_TOPIC = "BenchmarkTest";
const std::string MixAll::DEFAULT_PRODUCER_GROUP = "DEFAULT_PRODUCER";
const std::string MixAll::DEFAULT_CONSUMER_GROUP = "DEFAULT_CONSUMER";
const std::string MixAll::TOOLS_CONSUMER_GROUP = "TOOLS_CONSUMER";
const std::string MixAll::CLIENT_INNER_PRODUCER_GROUP = "CLIENT_INNER_PRODUCER";
const std::string MixAll::SELF_TEST_TOPIC = "SELF_TEST_TOPIC";
const std::string MixAll::RETRY_GROUP_TOPIC_PREFIX = "%RETRY%";
const std::string MixAll::DLQ_GROUP_TOPIC_PREFIX = "%DLQ%";
const std::string MixAll::NAMESRV_ADDR_ENV = "NAMESRV_ADDR";
const std::string MixAll::ROCKETMQ_HOME_ENV = "ROCKETMQ_HOME";
const std::string MixAll::ROCKETMQ_HOME_PROPERTY = "rocketmq.home.dir";
const std::string MixAll::MESSAGE_COMPRESS_LEVEL = "rocketmq.message.compressLevel";
const std::string MixAll::ROCKETMQ_NAMESRV_DOMAIN ="172.30.30.125"; //config.graphene.spellso.com
const std::vector<std::string> MixAll::CIDR = {"10.0.0.0/8", "172.16.0.0/12", "192.168.0.0/16"};

std::string MixAll::getRetryTopic(const std::string& consumerGroup)
{
	std::string result;
	result.append(RETRY_GROUP_TOPIC_PREFIX).append(consumerGroup);
	return result;
}

bool MixAll::compareAndIncreaseOnly(AtomicLong& target, long long value)
{
	long long current = target.Get();
	while (value > current)
	{
		long long tmp = target.CompareAndSet(current, value);
		
		if (tmp == current)
		{
			return true;
		}

		current = target.Get();
	}

	return false;
}


/**
 * This method supports IPv4 only.
 */
bool MixAll::is_public_ip(const std::string& ip) {

	struct sockaddr_in sock;
	inet_pton(AF_INET, ip.c_str(), &sock.sin_addr.s_addr);

	for (std::string cidr : CIDR) {
		std::string::size_type pos = cidr.find("/");
		std::string base = cidr.substr(0, pos);
		struct sockaddr_in sock_base;
		inet_pton(AF_INET, base.c_str(), &sock_base.sin_addr.s_addr);
		if (sock_base.sin_addr.s_addr == (sock_base.sin_addr.s_addr & sock.sin_addr.s_addr)) {
			return false;
		}
	}

	return true;
}

std::string MixAll::filterIP(const std::string& addr) {
	std::string separator(",");
	if (addr.find(separator) == std::string::npos) {
		return addr;
	} else {
		std::vector<std::string> ips;
		std::string::size_type previous = 0;
		std::string::size_type current = std::string::npos;
		while ((current = addr.find(separator, previous)) != std::string::npos) {
			std::string ip = addr.substr(previous, current - previous);
			ips.push_back(ip);
			previous = current + 1;

			if (current == addr.find_last_of(separator)) {
				ips.push_back(addr.substr(previous));
				break;
			}
		}

		struct ifaddrs* if_addr = nullptr;
		if (getifaddrs(&if_addr) == -1) {
			// TODO log error.
			std::cout << "Failed to execute getifaddrs()" << std::endl;
		}

		// Choose IP that share the same subnet with current host.
		for (std::vector<std::string>::iterator it = ips.begin(); it != ips.end(); it++) {
			struct sockaddr_in sock;
			inet_pton(AF_INET, it->c_str(), &sock.sin_addr.s_addr);
			struct ifaddrs* p = if_addr;
			while (p != nullptr) {
				if(p->ifa_addr->sa_family == AF_INET) {
					struct sockaddr_in* ip_addr = (struct sockaddr_in*)p->ifa_addr;
					struct sockaddr_in* netmask_addr = (struct sockaddr_in*)p->ifa_netmask;
					if ((ip_addr->sin_addr.s_addr & netmask_addr->sin_addr.s_addr) == (sock.sin_addr.s_addr & netmask_addr->sin_addr.s_addr)) {
						return *it;
					}
				}
				p = p->ifa_next;
			}
		}

		// If not found in the previous step, choose a public IP address.
		for (std::string ip : ips) {
			if (is_public_ip(ip)) {
				return ip;
			}
		}

		// Just return the first one and warn.
		std::cout << "Unable to figure out an ideal IP, returning the first candiate." << std::endl;
		return ips[0];
	}
}
