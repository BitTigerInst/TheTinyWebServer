#include "commom.h"
//int k=0;
#define MAX_SIZE  10
#include "httprequest.h"
#include "server.h"
#include "httpmessage.h"
#include "httpresourcehost.h"
#include "httpresource.h"
/*server::server() {
      cout<<"welcome"<<endl;
}*/
void server::start()
{
    listensocket=socket(AF_INET,SOCK_STREAM,0);
	ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(8888);
    ser_addr.sin_addr.s_addr = INADDR_ANY;
	bind(listensocket,(struct sockaddr*)&ser_addr,sizeof(ser_addr));
	listen(listensocket,MAX_SIZE);
	epfd = epoll_create(MAX_SIZE);
    
    if(epfd < 0) {
        perror("epfd error");
        exit(-1);
    }

    addfd(epfd, listensocket);

}
void server::addfd(int epollfd, int fd)
{
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev);

    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0)| O_NONBLOCK);
    printf("fd added to epoll!\n\n");
}	
void server::process()
{
	 static struct epoll_event events[MAX_SIZE]; 
	while(1)
    {
        //epoll_events_count表示就绪事件的数目
        int epoll_events_count = epoll_wait(epfd, events,MAX_SIZE, -1);

        if(epoll_events_count < 0) {
            perror("epoll failure");
            break;
        }

        cout << "epoll_events_count =\n" << epoll_events_count << endl;

        //处理这epoll_events_count个就绪事件
        for(int i = 0; i < epoll_events_count; ++i)
        {
            int sockfd = events[i].data.fd;
            //新用户连接
            if(sockfd == listensocket)
            {
                struct sockaddr_in client_address;
                socklen_t client_addrLength = sizeof(struct sockaddr_in);
                int clientfd = accept( listensocket, ( struct sockaddr* )&client_address, &client_addrLength );
				addfd(epfd, clientfd);
				client.push_back(clientfd);
            }
			else 
			{
				    readClient(sockfd); 

			}

}
	}
}
server::~server()
{
	  cout<<"quit"<<endl;
	  
	
}
int server::analysereq(httprequest* k)
{
	if(k->parse()==1)//进行客户端的http的解释
	{

		cout<<"bingo"<<endl;
	}
	if(k->support_opt()>0)
	{
		cout<<"support"<<endl;
		string uri_2=k->geturi();
		string ver_5=k->getversion();
		string mess_2=k->getheadmessage();
		httpresourcehost* vhost=new httpresourcehost(uri_2,mess_2,ver_5);
		string ext_1=vhost->getextension();//获取请求文件的扩展名
		string mimestring=vhost->findMimeType(ext_1);//获取mime类型
		
		string responmessage=vhost->openfile(mimestring,uri_2);//获取响应信息
		int fd_2=k->getclient();
		int ret=sendresponse(fd_2,responmessage);
		delete vhost;
		if(ret>0)
		{
			cout<<"That is successful"<<endl<<"The response is"<<responmessage<<endl;
			
		}
		else
		{
			cout<<"That is an error"<<endl;
		}
	}

	
}
void server::readClient(int fd)
{
	// data_len为0时有可能是客户端要断开连接，先设置 data_len 为以太网默认最大的MTU值
    int	data_len = 1400;
    char* pData = new char[data_len];
    
    // 读取数据到pData
    int flags = 0; 
    ssize_t lenRecv = recv(fd, pData, data_len, flags);
    cout<<pData<<endl;
    // 判断客户端状态
    if(lenRecv == 0) {
        // 关闭连接
        close(fd);
    } else if(lenRecv < 0) {
        // 连接出错
          cout<<"error"<<endl;
    } else {
		cout<<pData<<endl;
        // 接收数据，创建HTTPRequest对象，并调用analysereq处理
		httprequest* req=new httprequest(pData,fd);//通过请求类来进行http解释
	    int a=analysereq(req);
		delete req;
    }

	delete [] pData;
	
}
int server::sendresponse(int fd,string responsemessage)
{
	const char* resmess=responsemessage.c_str();
	if(int b=write(fd,resmess,responsemessage.length())>0)
	{
		cout<<"The length is"<<b<<endl;
	}
	else
	{
		cout<<"That is an error"<<endl;
		
	}
}
int main(int argc,char*argv[])
{

	server a;
	a.start();
	a.process();
 
	return 0;
}