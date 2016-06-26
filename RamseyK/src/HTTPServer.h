/**
	httpserver
	HTTPServer.h
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

#ifndef _HTTPSERVER_H_
#define _HTTPSERVER_H_

#include <unordered_map>
#include <vector>
#include <string>

#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

// 根据操作系统类型引入不同的kqueue头文件
#ifdef __linux__
#include <kqueue/sys/event.h> // libkqueue Linux
#else
#include <sys/event.h> // kqueue BSD / OS X
#endif

#include "Client.h"
#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include "ResourceHost.h"

// 非法的socket
#define INVALID_SOCKET -1
// kevent队列大小
#define QUEUE_SIZE 1024

// HTTPServer：Web服务器类，提供服务器的创建，启动，停止等管理操作
class HTTPServer {	
	// 监听的端口号
	int listenPort;

    // 监听的socket
    int listenSocket;
    
    // 服务器地址信息
    struct sockaddr_in serverAddr; 

    // Kqueue
    // 设置Kqueue时间片：2秒0纳秒
    struct timespec kqTimeout = {2, 0}; 
    // kqueue 描述符
	int kqfd; 
    
    // kevent队列
	struct kevent evList[QUEUE_SIZE]; 

	// 客户端字典，映射客户端的socket和客户端对象
    std::unordered_map<int, Client*> clientMap;

	// 资源主机及文件系统列表
	std::vector<ResourceHost*> hostList;
    // 虚拟主机，映射请求的地址到不同的ResourceHost
	std::unordered_map<std::string, ResourceHost*> vhosts; 
    
    // 处理客户端连接
    // 包括：
    // 1. 更新kevent
    // 2. 接受连接
    // 3. 获取客户端对象
    // 4. 断开连接
    // 5. 处理客户端读取和写入事件
    void updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata);
    void acceptConnection();
	Client *getClient(int clfd);
    void disconnectClient(Client* cl, bool mapErase = true);
    void readClient(Client* cl, int data_len); 
    bool writeClient(Client* cl, int avail_bytes); 
    
    // 处理客户端请求
    void handleRequest(Client* cl, HTTPRequest* req);
	void handleGet(Client* cl, HTTPRequest* req, ResourceHost* resHost);
	void handleOptions(Client* cl, HTTPRequest* req);

	// 发送响应消息给客户端
	void sendStatusResponse(Client* cl, int status, std::string msg = "");
	void sendResponse(Client* cl, HTTPResponse* resp, bool disconnect);

public:

    // 服务器是否需要退出
	bool canRun;

public:

    // 构造函数与析构函数
    HTTPServer();
    ~HTTPServer();

    // 启动及停止Web服务器
	bool start(int port);
	void stop();

	// Web服务器主循环
	void process();
};

#endif
