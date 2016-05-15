#ifndef SERVER_H
#define SERVER_H
#include<iostream>
using namespace std;
#include "commom.h"
class server
{
	public:
	   //server();//进行TCP的初始化
	   ~server();
	   void start();//监听端口，创建epoll结点
	   void process();//进行epoll多复用IO
	   void addfd(int epollfd, int fd);//将连接文件描述符注册到epoll
	   int analysereq(httprequest* k);//分析请求
	   void readClient(int fd); 
	private:
	   int listensocket;
	   struct sockaddr_in ser_addr;
	   int epfd;
	   vector<int> client;
   
	
	
};
#endif