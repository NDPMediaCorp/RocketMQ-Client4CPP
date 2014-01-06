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
#if!defined __UTILALL_H__
#define __UTILALL_H__

#include <stdlib.h>
#include <string>
#include <vector>

const std::string WHITESPACE=" \t\r\n";

/**
 * 各种方法大杂烩
 *
 */
class UtilAll
{
public:
	static int Split(std::vector<std::string>& out,const std::string& in,const std::string& delimiter )
	{
		std::string::size_type left=0;
		for(size_t i=1; i< in.size(); i++)
		{
			std::string::size_type right = in.find(delimiter,left);

			if (right = std::string::npos)
			{
				break;
			}

			out.push_back(in.substr(left,right-left));

			left = right+1;
		}

		out.push_back(in.substr(left));

		return out.size();
	}

	static std::string Trim(const std::string& str)
	{
		if (str.empty())
		{
			return str;
		}

		std::string::size_type left = str.find_first_not_of(WHITESPACE);

		if (left==std::string::npos)
		{
			return "";
		}

		std::string::size_type right = str.find_last_not_of(WHITESPACE);

		if (right==std::string::npos)
		{
			return str.substr(left);
		}

		return str.substr(left,right-left);
	}

	static bool isBlank( const std::string& str )
	{
		if (str.empty())
		{
			return true;
		}

		std::string::size_type left = str.find_first_not_of(WHITESPACE);

		if (left==std::string::npos)
		{
			return true;
		}

		return false;
	}

	static int availableProcessors()
	{
		return 4;
	}

	static int hashCode(void* pData, int len)
	{
		return 0;
	}

	static const char* uncompress(const char* pIn,int inLen,int* outLen)
	{
		*outLen = inLen;
		return pIn;
	}

	static unsigned long long hexstr2ull(const char* str)
	{
		char* end;
#ifdef WIN32
		return _strtoui64(str,&end,16);
#else
		return strtoull(str,&end,16);
#endif
	}
};

#endif
