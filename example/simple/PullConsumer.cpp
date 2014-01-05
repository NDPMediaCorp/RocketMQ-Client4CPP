// PullConsumer.cpp : 定义控制台应用程序的入口点。
//

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <set>

#include <DefaultMQPullConsumer.h>
#include <Message.h>
#include <MessageExt.h>
#include <MessageQueue.h>
#include <PullResult.h>
#include <MQClientException.h>

std::map<MessageQueue, long long> offseTable;

void putMessageQueueOffset(const MessageQueue& mq, long long offset)
{
	offseTable[mq]=offset;
}

long long getMessageQueueOffset(const MessageQueue& mq) 
{
	std::map<MessageQueue, long long>::iterator it = offseTable.find(mq);

	if (it!=offseTable.end())
	{
		return it->second;
	}

	return 0;
}

void PrintResult(PullResult& result)
{
	std::cout<<"[pullStatus="<<result.pullStatus
		<<"][nextBeginOffset="<<result.nextBeginOffset
		<<"][minOffset="<<result.minOffset
		<<"][maxOffset="<<result.maxOffset
		<<"]"<<std::endl;

	std::list<MessageExt*>::iterator it = result.msgFoundList.begin();

	for (;it!=result.msgFoundList.end();it++)
	{
		MessageExt* me = *it;
		std::string str;
		str.assign(me->getBody(),me->getBodyLen());
		std::cout<<str<<std::endl;
	}
}

int main(int argc, char* argv[])
{
	if (argc<2)
	{
		printf("Usage:%s ip:port\n",argv[0]);
		return 0;
	}

	DefaultMQPullConsumer consumer("please_rename_unique_group_name");
	consumer.setNamesrvAddr(argv[1]);
	consumer.start();

	std::set<MessageQueue>* mqs = consumer.fetchSubscribeMessageQueues("TopicTest");

	std::set<MessageQueue>::iterator it = mqs->begin();

	for (;it!=mqs->end();it++)
	{
		MessageQueue mq = *it;
		bool noNewMsg = false;
		while (!noNewMsg)
		{
			try
			{
				PullResult* pullResult = consumer.pullBlockIfNotFound(mq, "", getMessageQueueOffset(*it), 32);
				PrintResult(*pullResult);
				putMessageQueueOffset(mq, pullResult->nextBeginOffset);
				switch (pullResult->pullStatus)
				{
				case FOUND:
					// TODO
					break;
				case NO_MATCHED_MSG:
					break;
				case NO_NEW_MSG:
					noNewMsg = true;
					break;
				case OFFSET_ILLEGAL:
					break;
				default:
					break;
				}

				delete pullResult;
			}
			catch (MQClientException& e)
			{
				std::cout<<e<<std::endl;
			}
		}
	}

	consumer.shutdown();

	return 0;
}
