/**
	httpserver
	Client.h
	Copyright 2011-2014 Ramsey Kant

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	    http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <netinet/in.h>
#include <arpa/inet.h>
#include <queue>

#include "SendQueueItem.h"

typedef unsigned char byte;


// HTTP客户端
// 
class Client {
    
    // 连接的socket
    int socketDesc; 
    
    // 地址信息
    sockaddr_in clientAddr;

    // 数据发送队列
    std::queue<SendQueueItem*> sendQueue;
    
public:

    // 构造函数与析构函数
    Client(int fd, sockaddr_in addr);
    ~Client();
    
    // 返回地址信息
    sockaddr_in getClientAddr() {
        return clientAddr;
    }

    // 返回socket描述符
    int getSocket() {
        return socketDesc;
    }
    
    // 返回客户端IP地址
    char* getClientIP() {
        return inet_ntoa(clientAddr.sin_addr);
    }

    // 添加新的数据到发送队列
    void addToSendQueue(SendQueueItem* item);
    
    // 发送队列长度
    unsigned int sendQueueSize();
    
    // 获取发送队列中第一个元素
    SendQueueItem* nextInSendQueue();
    
    // 出队操作，删除第一个元素
    void dequeueFromSendQueue();
    
    // 清空发送队列
    void clearSendQueue();
};

#endif
