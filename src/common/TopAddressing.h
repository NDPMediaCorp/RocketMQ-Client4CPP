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

/**
 * Ñ°Ö··þÎñ
 *
 */
class TopAddressing
{
public:
	TopAddressing()
	{
		//  HttpConnectionManagerParams managerParams = httpClient.getHttpConnectionManager().getParams();
		//  managerParams.setConnectionTimeout(5000);
		//  managerParams.setSoTimeout(5000);
	}

	std::string fetchNSAddr()
	{

		return "";
	}

	//private static String clearNewLine(final String str) {
	//    String newString = str.trim();
	//    int index = newString.indexOf("\r");
	//    if (index != -1) {
	//        return newString.substring(0, index);
	//    }

	//    index = newString.indexOf("\n");
	//    if (index != -1) {
	//        return newString.substring(0, index);
	//    }

	//    return newString;
	//}


	//public final String fetchNSAddr() {
	//    HttpMethod httpMethod = null;

	//    try {
	//        httpMethod = new GetMethod(MixAll.WS_ADDR);
	//        int httpStatus = this.httpClient.executeMethod(httpMethod);
	//        if (200 == httpStatus) {
	//            byte[] responseBody = httpMethod.getResponseBody();
	//            if (responseBody != null) {
	//                String responseStr = new String(responseBody);
	//                return clearNewLine(responseStr);
	//            }
	//            else {
	//                log.error("httpMethod.getResponseBody() return null");
	//            }
	//        }
	//        else {
	//            log.error("HttpClient.executeMethod return not OK, " + httpStatus);
	//        }
	//    }
	//    catch (HttpException e) {
	//        log.error("fetchZKAddr exception", e);
	//    }
	//    catch (IOException e) {
	//        log.error("fetchZKAddr exception", e);
	//    }
	//    finally {
	//        if (httpMethod != null) {
	//            httpMethod.releaseConnection();
	//        }
	//    }

	//    String errorMsg =
	//            "connect to " + MixAll.WS_ADDR + " failed, maybe the domain name " + MixAll.WS_DOMAIN_NAME
	//                    + " not bind in /etc/hosts";
	//    errorMsg += FAQUrl.suggestTodo(FAQUrl.NAME_SERVER_ADDR_NOT_EXIST_URL);

	//    log.warn(errorMsg);
	//    System.out.println(errorMsg);
	//    return null;
	//}


	//protected void doOnNSAddrChanged(final String newNSAddr) {
	//}


	//public void tryToAddressing() {
	//    try {
	//        String newNSAddr = this.fetchNSAddr();
	//        if (newNSAddr != null) {
	//            if (null == this.nsAddr || !newNSAddr.equals(this.nsAddr)) {
	//                log.info("nsaddr in top web server changed, " + newNSAddr);
	//                this.doOnNSAddrChanged(newNSAddr);
	//            }
	//        }
	//    }
	//    catch (Exception e) {
	//        log.error("", e);
	//    }
	//}


	const std::string& getNsAddr()
	{
		return m_nsAddr;
	}


	void setNsAddr(const std::string& nsAddr)
	{
		m_nsAddr = nsAddr;
	}

private:
	//static final Logger log = LoggerFactory.getLogger(LoggerName.CommonLoggerName);
	//private HttpClient httpClient = new HttpClient();
	std::string m_nsAddr;
};

#endif
