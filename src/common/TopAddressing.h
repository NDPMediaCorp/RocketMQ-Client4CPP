/**
* Copyright (C) 2013 kangliqiang, kangliq@163.com
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
#if!defined __TOPADDRESSING_H__
#define  __TOPADDRESSING_H__

#include <string>

extern "C" {
#include <curl/curl.h>
}

#include "MixAll.h"
#include "Logger.h"

class fetch_ns_exception : public std::exception {
public:
    virtual const char* what() const throw() {
        return "Connection time";
    }
};

/**
 * Ñ°Ö··þÎñ
 *
 */
class TopAddressing
{
public:
	TopAddressing()
	{
	}

	static size_t curl_callback(char* data, size_t size, size_t nmemb, std::string *buffer) {
		if (buffer != nullptr) {
			buffer->append(data, size * nmemb);
			return size * nmemb;
		}
		return 0;
	}

    std::string fetchNSAddr()
    {
        std::string ns_domain = MixAll::ROCKETMQ_NAMESRV_DOMAIN;
        std::string name_server_discovery_end_point = ns_domain.append("/rocketmq/nsaddr");
        return fetchNameServer(name_server_discovery_end_point, 3000);
    }

	std::string fetchNameServer(std::string& domain, int timeout) throw (fetch_ns_exception)  {
		CURL *curl;
		CURLcode res;
		std::string result = "";
		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, domain.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, timeout);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, timeout);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK) {
			const char* errMsg = curl_easy_strerror(res);
			Logger::get_logger()->error(errMsg);
			fetch_ns_exception e;
			throw e;
		}
		return result;
	}


	const std::string& getNsAddr()
	{
		return m_nsAddr;
	}


	void setNsAddr(const std::string& nsAddr)
	{
		m_nsAddr = nsAddr;
	}

private:
	std::string m_nsAddr;
};

#endif
