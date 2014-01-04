// Producer.cpp : 定义控制台应用程序的入口点。
//

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include <DefaultMQProducer.h>
#include <Message.h>
#include <SendResult.h>
#include <MQClientException.h>

int main(int argc, char* argv[])
{
	if (argc<2)
	{
		printf("Usage:%s ip:port\n",argv[0]);
		return 0;
	}

	int count = 1000;

	if (argc>2)
	{
		count = atoi(argv[2]);
	}

	DefaultMQProducer producer("please_rename_unique_group_name");
	producer.setNamesrvAddr(argv[1]);
	producer.start();

	std::string tags[] = { "TagA", "TagB", "TagC", "TagD", "TagE" };

	char key[8];
	char value[32];

	for (int i = 0; i < count; i++) {
		try
		{
			sprintf(key,"KEY%d",i);
			sprintf(value,"Hello RocketMQ %d",i);
			Message msg("TopicTest",// topic
				tags[i % 5],// tag
				key,// key
				value,// body
				strlen(value)+1);
			SendResult sendResult = producer.send(msg);
			Sleep(100);
			printf("sendresult=%d,msgid=%s\n",sendResult.getSendStatus(),sendResult.getMsgId().c_str());
		}
		catch (MQClientException& e) {
			std::cout<<e<<std::endl;
			Sleep(3000);
		}
	}

	producer.shutdown();
	return 0;
}

