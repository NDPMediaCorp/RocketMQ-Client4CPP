// Producer.cpp : 定义控制台应用程序的入口点。
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

#ifdef WIN32
#   include <sys/timeb.h>
#   include <process.h>
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <signal.h>
#endif

#include <DefaultMQProducer.h>
#include <Message.h>
#include <SendResult.h>
#include <MQClientException.h>

void MySleep(long millis)
{

#ifdef WIN32
	::Sleep(millis);
#else
	struct timespec tv;
	tv.tv_sec = millis / 1000;
	tv.tv_nsec = (millis % 1000) * 1000000;
	 nanosleep(&tv, 0);
#endif
}

void Usage(const char* program)
{
	printf("Usage:%s ip:port [-n] [-v]\n",program);
	printf("\t -n message count\n");
	printf("\t -v message size \n");
}

int main(int argc, char* argv[])
{
	if (argc<2)
	{
		Usage(argv[0]);
		return 0;
	}

	int count = 1000;
	int size = 32;

	for (int i=2; i< argc; i++)
	{
		if (strcmp(argv[i],"-n")==0)
		{
			if (i+1 < argc)
			{
				count = atoi(argv[i+1]);
				i++;
			}
			else
			{
				Usage(argv[0]);
				return 0;
			}
		}
		else if (strcmp(argv[i],"-v")==0)
		{
			if (i+1 < argc)
			{
				size = atoi(argv[i+1]);
				i++;
			}
			else
			{
				Usage(argv[0]);
				return 0;
			}
		}
		else
		{
			Usage(argv[0]);
			return 0;
		}
	}

	DefaultMQProducer producer("please_rename_unique_group_name");
	producer.setNamesrvAddr(argv[1]);
	producer.start();

	std::string tags[] = { "TagA", "TagB", "TagC", "TagD", "TagE" };

	char key[16];
	char* value = new char[size];
	
	strcpy(value,"Hello RocketMQ");

	for (int i=14;i<size-8;i++)
	{
		char ch;
		switch (i%3)
		{
		case 0:
			ch='a';
			break;
		case 1:
			ch='b';
			break;
		case 2:
		default:
			ch='c';
			break;
		}

		*(value+i) = ch;
	}

	for (int i = 0; i < count; i++) {
		try
		{
			sprintf(key,"KEY%d",i);
			sprintf(value + size -8,"%d",i);
			Message msg("TopicTest",// topic
				tags[i % 5],// tag
				key,// key
				value,// body
				strlen(value)+1);
			SendResult sendResult = producer.send(msg);
			MySleep(100);
			printf("sendresult=%d,msgid=%s\n",sendResult.getSendStatus(),sendResult.getMsgId().c_str());
		}
		catch (MQClientException& e) {
			std::cout<<e<<std::endl;
			MySleep(3000);
		}
	}

	producer.shutdown();
	return 0;
}

