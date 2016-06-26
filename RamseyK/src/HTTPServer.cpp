/**
	httpserver
	HTTPServer.cpp
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

#include "HTTPServer.h"

/**
 * 构造函数
 * 初始化数据变量，ResourceHost和虚拟主机
 */
HTTPServer::HTTPServer() {
	canRun = false;
	listenSocket = INVALID_SOCKET;

	// Create a resource host serving the base path ./htdocs on disk
    ResourceHost* resHost = new ResourceHost("./htdocs");
	hostList.push_back(resHost);

	// Setup the resource host serving htdocs to provide for the following vhosts:
	vhosts.insert(std::pair<std::string, ResourceHost*>("localhost:8080", resHost));
	vhosts.insert(std::pair<std::string, ResourceHost*>("127.0.0.1:8080", resHost));
}

/**
 * 析构函数
 * 释放所有ResourceHost和虚拟主机
 */
HTTPServer::~HTTPServer() {
	// Loop through hostList and delete all ResourceHosts
	while(!hostList.empty()) {
		ResourceHost* resHost = hostList.back();
		delete resHost;
		hostList.pop_back();
	}
	vhosts.clear();
}

/**
 * 启动服务器
 * 创建socket，绑定并监听
 *
 * @param port 监听的端口号
 * @return 启动成功返回True
 */
bool HTTPServer::start(int port) {
	canRun = false;    
	listenPort = port;

	// 创建监听的socket
	listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(listenSocket == INVALID_SOCKET) {
		std::cout << "Could not create socket!" << std::endl;
		return false;
	}
	
	// 设置socket为非阻塞
	fcntl(listenSocket, F_SETFL, O_NONBLOCK);
    
	// 设置服务器地址信息
	// modify to support multiple address families (bottom): http://eradman.com/posts/kqueue-tcp.html
    memset(&serverAddr, 0, sizeof(struct sockaddr_in)); // clear the struct
	serverAddr.sin_family = AF_INET; // Family: IP protocol
	serverAddr.sin_port = htons(listenPort); // Set the port (convert from host to netbyte order)
	serverAddr.sin_addr.s_addr = INADDR_ANY; // Let OS intelligently select the server's host address
    
	// 绑定到服务器地址及端口
	if(bind(listenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
		std::cout << "Failed to bind to the address!" << std::endl;
		return false;
	}
    
	// 监听状态等待客户端连接
	if(listen(listenSocket, SOMAXCONN) != 0) {
		std::cout << "Failed to put the socket in a listening state" << std::endl;
		return false;
	}
    
	// 创建 kqueue 描述符
	kqfd = kqueue();
	if(kqfd == -1) {
		std::cout << "Could not create the kernel event queue!" << std::endl;
		return false;
	}
	
	// 将监听的socket添加到kqueue中
	updateEvent(listenSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);

	canRun = true;
	std::cout << "Server ready. Listening on port " << listenPort << "..." << std::endl;
	return true;
}

/**
 * 停止服务
 * 断开所有客户端连接并清理所有启动阶段分配的资源
 */
void HTTPServer::stop() {
	canRun = false;

	if(listenSocket != INVALID_SOCKET) {
	    // 断开所有客户端连接
	    for(auto& x : clientMap)
	        disconnectClient(x.second, false);
	    
	    // 清理客户端列表
	    clientMap.clear();

		// 从kqueue中删除监听的socket
		updateEvent(listenSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	    
	    // 关闭监听的socket
		shutdown(listenSocket, SHUT_RDWR);
		close(listenSocket);
		listenSocket = INVALID_SOCKET;
	}

	if(kqfd != -1) {
		close(kqfd);
		kqfd = -1;
	}

	std::cout << "Server shutdown!" << std::endl;
}

/**
 * 更新 kqueue
 * 创建合适的kevent对象来更新kqueue
 * See kqueue documentation for parameter descriptions
 */
void HTTPServer::updateEvent(int ident, short filter, u_short flags, u_int fflags, int data, void *udata) {
	struct kevent kev;
	EV_SET(&kev, ident, filter, flags, fflags, data, udata);
	kevent(kqfd, &kev, 1, NULL, 0, NULL);
}

/**
 * 服务器主循环
 * 主循环中检查是否有新的客户端连接及新的客户端数据需要进行读取和写入
 */
void HTTPServer::process() {
    // kevent 返回的事件数量
	int nev = 0; 
	Client* cl = NULL;
    
    // 处理读和写的kevent对象
	struct kevent read_kev, write_kev;

	while(canRun) {
		// 获取触发的事件列表
		nev = kevent(kqfd, NULL, 0, evList, QUEUE_SIZE, &kqTimeout);
		
		if(nev <= 0)
			continue;

	    // 循环处理所有事件
	    for(int i = 0; i < nev; i++) {
            // 新的客户端连接
	        if(evList[i].ident == (unsigned int)listenSocket) { 
	            acceptConnection();
	        } else { 
                // 客户端连接中触发的读写事件
				cl = getClient(evList[i].ident); 
				if(cl == NULL) {
					std::cout << "Could not find client" << std::endl;
					continue;
				}

				// 客户端要断开连接
				if(evList[i].flags & EV_EOF) {
					disconnectClient(cl);
					continue;
				}

				// 初始化kevent对象
				memset(&read_kev, 0, sizeof(struct kevent));
				memset(&write_kev, 0, sizeof(struct kevent));

				if(evList[i].filter == EVFILT_READ) {
					// 读取并处理数据
	            	readClient(cl, evList[i].data); 

					// 设置读取事件处理完成，触发写入事件
					updateEvent(evList[i].ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
					updateEvent(evList[i].ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
				} else if(evList[i].filter == EVFILT_WRITE) {
					// 写入数据到客户端
					if(!writeClient(cl, evList[i].data)) { 
						// 如果队列中仍然有数据则需要继续出发读事件而关闭写事件
						updateEvent(evList[i].ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
						updateEvent(evList[i].ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
					}
				}
	        }
	    } // 事件处理循环
	} // 服务器主循环
}

/**
 * 接受客户端连接
 * 有客户端连接时该函数被调用，创建新的客户端对象并添加到字典中
 */
void HTTPServer::acceptConnection() {
    // 设置客户端地址信息和socket值
    sockaddr_in clientAddr;
    int clientAddrLen = sizeof(clientAddr);
    int clfd = INVALID_SOCKET;
    
    clfd = accept(listenSocket, (sockaddr*)&clientAddr, (socklen_t*)&clientAddrLen);
    if(clfd == INVALID_SOCKET)
        return;

	fcntl(clfd, F_SETFL, O_NONBLOCK);
    
    // 创建客户端对象
    Client *cl = new Client(clfd, clientAddr);
    
	// 添加客户端socket到kqueue中，读取和写入，注意写入初始状态是关闭
	updateEvent(clfd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	updateEvent(clfd, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL); 
    
    // 添加客户端对象到字典
    clientMap.insert(std::pair<int, Client*>(clfd, cl));
    
	std::cout << "[" << cl->getClientIP() << "] connected" << std::endl;
}

/**
 * 返回客户端对象
 * 根据socket查找并返回客户端对象
 *
 * @param clfd 客户端socket
 * @return 如果找到则返回客户端对象指针否则返回NULL
 */
Client* HTTPServer::getClient(int clfd) {
	std::unordered_map<int, Client*>::const_iterator it;
    it = clientMap.find(clfd);

	if(it == clientMap.end())
		return NULL;

    return it->second;
}

/**
 * 断开连接
 * 释放客户端socket及Client对象
 *
 * @param cl 客户端对象指针
 * @param mapErase 为True时从map中删除客户端对象
 */
void HTTPServer::disconnectClient(Client *cl, bool mapErase) {
    if(cl == NULL)
        return;

    std::cout << "[" << cl->getClientIP() << "] disconnected" << std::endl;

    // 清理kqueue中的数据
	updateEvent(cl->getSocket(), EVFILT_READ, EV_DELETE, 0, 0, NULL);
	updateEvent(cl->getSocket(), EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
    
    // 关闭socket
    close(cl->getSocket());
    
    // 从clientMap中清理
	if(mapErase)
    	clientMap.erase(cl->getSocket());
    
    // 释放内存
    delete cl;
}

/**
 * 从客户端读取数据
 * 从客户端读取数据并传递给 handleRequest()
 *
 * @param cl Client对象指针
 * @param data_len 等待读取的数据长度
 */
void HTTPServer::readClient(Client *cl, int data_len) {
    if (cl == NULL)
        return;

    // data_len为0时有可能是客户端要断开连接，先设置 data_len 为以太网默认最大的MTU值
    if(data_len <= 0)
    	data_len = 1400;
    
	HTTPRequest* req;
    char* pData = new char[data_len];
    
    // 读取数据到pData
    int flags = 0; 
    ssize_t lenRecv = recv(cl->getSocket(), pData, data_len, flags);
    
    // 判断客户端状态
    if(lenRecv == 0) {
        // 关闭连接
		std::cout << "[" << cl->getClientIP() << "] has opted to close the connection" << std::endl;
        disconnectClient(cl);
    } else if(lenRecv < 0) {
        // 连接出错
        disconnectClient(cl);
    } else {
        // 接收数据，创建HTTPRequest对象，并调用handleRequest处理
		req = new HTTPRequest((byte*)pData, lenRecv);
        handleRequest(cl, req);
		delete req;
    }

	delete [] pData;
}

/**
 * 写入客户端
 * 将发送队列中的数据写入客户端
 *
 * @param cl Client对象指针
 * @param avail_bytes 发送队列中等待写入的数据大小
 */
bool HTTPServer::writeClient(Client* cl, int avail_bytes) {
    if (cl == NULL)
        return false;

	int actual_sent = 0; // 实际发送给客户端的数据
	int attempt_sent = 0; // 尝试发送的数据
	int remaining = 0; // 等待发送的数据
	bool disconnect = false;
	byte* pData = NULL;

	// 调整要发送的数据大小
	if(avail_bytes > 1400) {
		// 设置为最大的MTU值
		avail_bytes = 1400;
	} else if(avail_bytes == 0) {
		// 有些操作系统显示为0但仍然会有数据发出
		avail_bytes = 64;
	}

    // 获取发送队列里的元素
	SendQueueItem* item = cl->nextInSendQueue();
	if(item == NULL)
		return false;

	pData = item->getData();
	remaining = item->getSize() - item->getOffset();
	disconnect = item->getDisconnect();

	if(avail_bytes >= remaining) {
		// 如果发送缓存大于需要发送的数据量，则将剩余数据都发出
		attempt_sent = remaining;
	} else {
		// 如果发送缓存小于需要发送的数据量，则发出发送缓存大小的数据
		attempt_sent = avail_bytes;
	}

	// 发送数据并根据实际发送的数据调整偏移
	actual_sent = send(cl->getSocket(), pData+(item->getOffset()), attempt_sent, 0);
	if(actual_sent >= 0)
		item->setOffset(item->getOffset() + actual_sent);
	else
		disconnect = true;

	// 如果偏移量大于了实际的数据大小，
    // 表示已经发送完毕不再需要这个SendQueueItem了则出队操作
	if(item->getOffset() >= item->getSize())
		cl->dequeueFromSendQueue();

	if(disconnect) {
		disconnectClient(cl);
		return false;
	}

	return true;
}

/**
 * 处理请求
 * 根据HTTP请求的类型进行分发给不同的处理函数处理
 *
 * @param cl Client对象指针
 * @param req HTTPRequest对象
 */
void HTTPServer::handleRequest(Client *cl, HTTPRequest* req) {
    // 解析请求数据，如果出错则返回BAD_REQUEST错误
    if(!req->parse()) {
		std::cout << "[" << cl->getClientIP() << "] There was an error processing the request of type: " << req->methodIntToStr(req->getMethod()) << std::endl;
		std::cout << req->getParseError() << std::endl;
		sendStatusResponse(cl, Status(BAD_REQUEST), req->getParseError());
		return;
    }

	std::cout << "[" << cl->getClientIP() << "] " << req->methodIntToStr(req->getMethod()) << " " << req->getRequestUri() << std::endl;
    
	// 选择合适的虚拟主机
	ResourceHost* resHost = NULL;
	std::string host = "";
    
    // 支持HTTP/1.1兼容性
    if(req->getVersion().compare(HTTP_VERSION_11) == 0) {
		host = req->getHeaderValue("Host");
		std::unordered_map<std::string, ResourceHost*>::const_iterator it = vhosts.find(host);

		if(it != vhosts.end())
			resHost = it->second;
	} else {
		// HTTP/1.0 默认提供第一个ResourceHost
		if(hostList.size() > 0)
			resHost = hostList[0];
	}

	// ResourceHost 无法找到时返回BAD_REQUEST
	if(resHost == NULL) {
		sendStatusResponse(cl, Status(BAD_REQUEST), "Invalid/No Host specified: " + host);
		return;
	}
    
    // 根据请求的类型发给不同的处理函数
    switch(req->getMethod()) {
        case Method(HEAD):
        case Method(GET):
            handleGet(cl, req, resHost);
            break;
		case Method(OPTIONS):
			handleOptions(cl, req);
			break;
        default:
			std::cout << "[" << cl->getClientIP() << "] Could not handle or determine request of type " << req->methodIntToStr(req->getMethod()) << std::endl;
			sendStatusResponse(cl, Status(NOT_IMPLEMENTED));
		break;
    }
}

/**
 * 处理GET和HEAD请求
 *
 * @param cl Client对象指针
 * @param req 请求对象
 * @param resHost 处理请求的ResourceHost对象
 */
void HTTPServer::handleGet(Client* cl, HTTPRequest* req, ResourceHost* resHost) {	
	// 获取请求的资源
	std::string uri = req->getRequestUri();
    Resource* r = resHost->getResource(uri);

	if(r != NULL) {
        // 资源存在
		HTTPResponse* resp = new HTTPResponse();
		resp->setStatus(Status(OK));
		resp->addHeader("Content-Type", r->getMimeType());
		resp->addHeader("Content-Length", r->getSize());
		
		// 只为GET方法返回的数据
		if(req->getMethod() == Method(GET))
			resp->setData(r->getData(), r->getSize());
		
		bool dc = false;

		// HTTP/1.0 默认关闭连接
		if(req->getVersion().compare(HTTP_VERSION_10) == 0)
			dc = true;

		// Connection: close 情况下需要在处理完请求后关闭连接
		std::string connection_val = req->getHeaderValue("Connection");
		if(connection_val.compare("close") == 0)
			dc = true;
			
		sendResponse(cl, resp, dc);
		delete resp;
		delete r;
	} else { 
        // 返回404，资源未找到
		sendStatusResponse(cl, Status(NOT_FOUND));
	}
}

/**
 * 处理 OPTIONS方法
 * OPTIONS: 返回服务端支持的方法
 *
 * @param cl Client对象指针
 * @param req 请求HTTPRequest对象
 */
void HTTPServer::handleOptions(Client* cl, HTTPRequest* req) {
	// 默认返回所有支持的方法
	std::string allow = "HEAD, GET, OPTIONS";

	HTTPResponse* resp = new HTTPResponse();
	resp->setStatus(Status(OK));
	resp->addHeader("Allow", allow.c_str());
	resp->addHeader("Content-Length", "0"); // Required

	sendResponse(cl, resp, true);
	delete resp;
}


/**
 * 发送状态响应给客户端
 * 发送HTTP响应状态码给客户端并关闭连接
 *
 * @param cl Client对象指针
 * @param status HTTP响应状态码
 * @param msg 额外的响应消息
 */
void HTTPServer::sendStatusResponse(Client* cl, int status, std::string msg) {
	HTTPResponse* resp = new HTTPResponse();
	resp->setStatus(Status(status));
	
	// 构建响应消息内容
	std::string body = resp->getReason() + ": " + msg;
	unsigned int slen = body.length();
	char* sdata = new char[slen];
	strncpy(sdata, body.c_str(), slen);
	
	resp->addHeader("Content-Type", "text/plain");
	resp->addHeader("Content-Length", slen);
	resp->setData((byte*)sdata, slen);
	
	sendResponse(cl, resp, true);
	
	delete resp;
}

/**
 * 发送HTTP响应给客户端
 *
 * @param cl Client对象指针
 * @param buf 响应数据ByteBuffer对象
 * @param disconnect 发送后是否关闭连接
 */
void HTTPServer::sendResponse(Client* cl, HTTPResponse* resp, bool disconnect) {
	// 服务器头部
	resp->addHeader("Server", "shiyanlouserver/1.0");
	
	// 响应的时间信息
	std::string tstr;
	char tbuf[36];
	time_t rawtime;
	struct tm* ptm;
	time(&rawtime);
	ptm = gmtime(&rawtime);
	// Ex: Fri, 31 Dec 1999 23:59:59 GMT
	strftime(tbuf, 36, "%a, %d %b %Y %H:%M:%S GMT", ptm);
	tstr = tbuf;
	resp->addHeader("Date", tstr);
	
	// 是否需要在头部中添加断开连接的信息
	if(disconnect)
		resp->addHeader("Connection", "close");
	
	// 创建响应数据
	byte* pData = resp->create();

	// 将数据添加到客户端的发送队列
	cl->addToSendQueue(new SendQueueItem(pData, resp->size(), disconnect));
}


