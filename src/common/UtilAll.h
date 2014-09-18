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
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <string>
#include <vector>

#include "zlib.h"

const std::string WHITESPACE=" \t\r\n";
const int CHUNK = 8192;

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

			if (right == std::string::npos)
			{
				break;
			}

			out.push_back(in.substr(left,right-left));

			left = right+delimiter.length();
		}

		out.push_back(in.substr(left));

		return out.size();
	}
	
	static int Split(std::vector<std::string>& out,const std::string& in,const char delimiter )
	{
		std::string::size_type left=0;
		for(size_t i=1; i< in.size(); i++)
		{
			std::string::size_type right = in.find(delimiter,left);

			if (right == std::string::npos)
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

		return str.substr(left,right + 1 -left);
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

	static int stringHashCode(const char* pData, int len)
	{
		int hash = 0;
		if (pData !=NULL && len > 0)
		{
			for (int i = 0; i < len; i++)
			{
				hash = 31*hash + pData[i];
			}
		}

		return hash;
	}

	static bool compress(const char* pIn, int inLen, unsigned char** pOut, int* pOutLen, int level)
	{
		int ret, flush;
		int have;
		z_stream strm;

		unsigned char out[CHUNK];

		/* allocate deflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		ret = deflateInit(&strm, level);
		if (ret != Z_OK)
		{
			return false;
		}

		int outBufferLen = inLen;
		unsigned char* outData = (unsigned char*)malloc(outBufferLen);


		int left = inLen;
		int used = 0;
		int outDataLen = 0;
		int outPos=0;

		/* compress until end of buffer */
		do
		{
			strm.avail_in = left > CHUNK ? CHUNK : left;
			flush = left <= CHUNK ? Z_FINISH : Z_NO_FLUSH;
			strm.next_in = (unsigned char*)pIn+used;
			used += strm.avail_in;
			left -= strm.avail_in;

			/* run deflate() on input until output buffer not full, finish
			compression if all of source has been read in */
			do
			{
				strm.avail_out = CHUNK;
				strm.next_out = out;
				ret = deflate(&strm, flush);    /* no bad return value */
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				have = CHUNK - strm.avail_out;

				if (outDataLen + have > outBufferLen)
				{
					outBufferLen = outDataLen + have;
					outBufferLen <<= 1;
					unsigned char* tmp = (unsigned char*)realloc(outData, outBufferLen);
					if (!tmp)
					{
						free(outData);
						return false;
					}

					outData = tmp;
				}

				memcpy(outData+outDataLen, out, have);
				outDataLen += have;

			} while (strm.avail_out == 0);
			assert(strm.avail_in == 0);     /* all input will be used */

			/* done when last data in file processed */
		} while (flush != Z_FINISH);
		assert(ret == Z_STREAM_END);        /* stream will be complete */

		*pOutLen = outDataLen;
		*pOut = outData;

		/* clean up and return */
		(void)deflateEnd(&strm);
		return true;
	}

	static bool decompress(const char* pIn, int inLen, unsigned char** pOut, int* pOutLen)
	{
		int ret;
		int have;
		z_stream strm;

		unsigned char out[CHUNK];

		/* allocate inflate state */
		strm.zalloc = Z_NULL;
		strm.zfree = Z_NULL;
		strm.opaque = Z_NULL;
		strm.avail_in = 0;
		strm.next_in = Z_NULL;
		ret = inflateInit(&strm);
		if (ret != Z_OK)
		{
			return false;
		}

		int outBufferLen = inLen<<2;
		unsigned char* outData = (unsigned char*)malloc(outBufferLen);

		int left = inLen;
		int used = 0;
		int outDataLen = 0;
		int outPos=0;

		/* decompress until deflate stream ends or end of buffer */
		do 
		{
			strm.avail_in = left > CHUNK? CHUNK : left;
			if (strm.avail_in <= 0)
			{
				break;
			}

			strm.next_in = (unsigned char*)pIn+used;
			used += strm.avail_in;
			left -= strm.avail_in;

			/* run inflate() on input until output buffer not full */
			do
			{
				strm.avail_out = CHUNK;
				strm.next_out = out;
				ret = inflate(&strm, Z_NO_FLUSH);
				assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
				switch (ret)
				{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;     /* and fall through */
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					(void)inflateEnd(&strm);
					free(outData);
					return false;
				}
				have = CHUNK - strm.avail_out;

				if (outDataLen + have > outBufferLen)
				{
					outBufferLen = outDataLen + have;
					outBufferLen <<= 1;
					unsigned char* tmp = (unsigned char*)realloc(outData, outBufferLen);
					if (!tmp)
					{
						free(outData);
						return false;
					}

					outData = tmp;
				}

				memcpy(outData+outDataLen, out, have);
				outDataLen += have;

			} while (strm.avail_out == 0);

			/* done when inflate() says it's done */
		} while (ret != Z_STREAM_END);

		/* clean up and return */
		(void)inflateEnd(&strm);

		if (ret == Z_STREAM_END)
		{
			*pOutLen = outDataLen;
			*pOut = outData;

			return true;
		}
		else
		{
			free(outData);

			return false;
		}
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
