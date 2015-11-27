// Producer.cpp : 定义控制台应用程序的入口点。
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <thread>
#include <chrono>

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

inline void MySleep(long millis) {
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

void Usage(const char *program) {
    printf("Usage:%s [-n] [-c] [-v]\n", program);
    printf("\t -n ip:port\n");
    printf("\t -c message count\n");
    printf("\t -v message size \n");
}

int main(int argc, char *argv[]) {
    int count = 1;
    int size = 32;
    std::string* name_server_list = nullptr;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            if (i + 1 < argc) {
                name_server_list = new std::string(argv[i + 1]);
                i++;
            } else {
                Usage(argv[0]);
                return 0;
            }

        } else if (strcmp(argv[i], "-c") == 0) {
            if (i + 1 < argc) {
                count = atoi(argv[i + 1]);
                i++;
            } else {
                Usage(argv[0]);
                return 0;
            }
        } else if (strcmp(argv[i], "-v") == 0) {
            if (i + 1 < argc) {
                size = atoi(argv[i + 1]);
                i++;
            } else {
                Usage(argv[0]);
                return 0;
            }
        } else {
            Usage(argv[0]);
            return 0;
        }
    }

    DefaultMQProducer producer("PG_CppClient");

    if (name_server_list != nullptr) {
        producer.setNamesrvAddr(*name_server_list);
    }

    producer.start();

    std::string tags[] = {"TagA", "TagB", "TagC", "TagD", "TagE"};

    char key[16];
    std::string value("Hello Cpp Client");

    for (int i = 0; i < count; i++) {
        try {
            sprintf(key, "KEY%d", i);
            Message msg("TopicTest",// topic
                        tags[i % 5],// tag
                        key,// key
                        value.c_str(),// body
                        value.size());
            SendResult sendResult = producer.send(msg);
            // MySleep(100);
            printf("sendresult=%d,msgid=%s\n", sendResult.getSendStatus(), sendResult.getMsgId().c_str());
        }
        catch (MQClientException &e) {
            std::cout << e << std::endl;
            MySleep(3000);
        }
    }

    producer.shutdown();
    return 0;
}

