#!/bin/bash
g++ -o ../../bin/producer Producer.cpp -g -m64 -I../../include -L../../bin -lrocketmq64 

g++ -o ../../bin/pullconsumer PullConsumer.cpp -m64 -g -I../../include -L../../bin -lrocketmq64 

g++ -o ../../bin/pushconsumer PushConsumer.cpp -m64 -g -I../../include -L../../bin -lrocketmq64 

