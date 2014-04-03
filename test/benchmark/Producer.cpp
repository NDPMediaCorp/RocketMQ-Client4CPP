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

unsigned long long GetCurrentTimeMillis()
{
#ifdef WIN32
	timeb tb;
	ftime(&tb);
	return tb.time * 1000ULL + tb.millitm;
#else
	struct timeval tv;
	gettimeofday(&tv, 0);
	return tv.tv_sec * 1000ULL+tv.tv_usec/1000;
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

	long long begin = GetCurrentTimeMillis();	
	int success=0;

	for (int i = 0; i < count; i++)
	{
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
			if (sendResult.getSendStatus() == SEND_OK)
			{
				success++;
			}
		}
		catch (MQClientException& e) {
			std::cout<<e<<std::endl;
			MySleep(1000);
		}
	}

	long long end = GetCurrentTimeMillis();
	int time = end - begin;
	int tps = success*1.0/time*1000.;;

	printf("success=%d,time=%d,tps=%d\n",success,time,tps);

	producer.shutdown();

	return 0;
}

