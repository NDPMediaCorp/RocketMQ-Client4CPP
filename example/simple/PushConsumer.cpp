// PullConsumer.cpp : 定义控制台应用程序的入口点。
//

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <map>
#include <set>

#include <DefaultMQPushConsumer.h>
#include <MessageListener.h>
#include <Message.h>
#include <MessageExt.h>
#include <MessageQueue.h>
#include <PullResult.h>
#include <MQClientException.h>

long long str2ll( const char *str )
{
#ifdef WIN32
	return _atoi64(str);
#else
	return atoll(str);
#endif

}

class MsgListener : public MessageListenerConcurrently
{
public:
	MsgListener()
	{
		consumeTimes = 0;
	}

	~MsgListener()
	{

	}

	ConsumeConcurrentlyStatus consumeMessage(std::list<MessageExt*>& msgs,
											ConsumeConcurrentlyContext& context)
	{
		std::cout<<"consumeMessage"<<std::endl;
		MessageExt* msg = msgs.front();
		long long offset = msg->getQueueOffset();
		std::string maxOffset = msg->getProperty(Message::PROPERTY_MAX_OFFSET);

		long long diff = str2ll(maxOffset.c_str()) - offset;

		if (diff > 100000)
		{
			// TODO 消息堆积情况的特殊处理
			// return ConsumeConcurrentlyStatus.CONSUME_SUCCESS;
		}

		std::list<MessageExt*>::iterator it = msgs.begin();

		for (;it != msgs.end();it++)
		{
			MessageExt* me = *it;
			std::string str;
			str.assign(me->getBody(),me->getBodyLen());
			std::cout<<str<<std::endl;
		}

		consumeTimes++;
		if ((consumeTimes % 2) == 0)
		{
			return RECONSUME_LATER;
		}
		else if ((consumeTimes % 3) == 0)
		{
			context.delayLevelWhenNextConsume = 5;
			return RECONSUME_LATER;
		}

		return CONSUME_SUCCESS;
	}

	int consumeTimes;
};

int main(int argc, char* argv[])
{
	bool auto_fetch_ns = true;

	if (argc >= 2) {
		auto_fetch_ns = false;
	}

	DefaultMQPushConsumer consumer("CG_Cpp_Push");
	if (!auto_fetch_ns) {
        printf("Usage:%s [ip:port]\n",argv[0]);
        std::cout << "Now we are using specified ip:port name server address" << std::endl;
		consumer.setNamesrvAddr(argv[1]);
	}

	/**
	* 订阅指定topic下所有消息
	*/
	// consumer.subscribe("TopicTest", "*");

	/**
	* 订阅指定topic下tags分别等于TagA或TagC或TagD
	*/
	consumer.subscribe("TopicTest", "TagA || TagC || TagD");

	MsgListener* listener = new MsgListener();

	consumer.registerMessageListener(listener);
	consumer.start();
	
	while(1)
	{
		if (getchar()=='e'&&getchar()=='x'&&getchar()=='i'&&getchar()=='t')
		{
			break;
		}

	}

	consumer.shutdown();

	delete listener;

	return 0;
}
