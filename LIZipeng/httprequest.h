#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#include "commom.h"
#include "httpmessage.h"
/************************请求类************************************/
/*成员变量：1.解释的内容
          2.http的读位置记录
成员函数：1.解释*/	
/******************************************************************/	
class httprequest{
	public:
	   httprequest(char* p,int fd_1);
	   ~httprequest();
	   int parse();//解释请求
	   int support_opt();//查看服务器是否支持该类型
	   int getclient();//获得客户端的文件描述符
	   string getmethod();//获得请求方式
	   string geturi();//获得请求资源
	   string getversion();//获得版本号
	   string getheadmessage();//获得关键的Accept相关信息
	   int parseline(string q);//得到头信息的初始位置
	   void parsehead(string p,int num);//分析头信息，将头信息加入到关联容器
	private:
	   string req;
	   int clientfd;
        map<string,string> headermessage;
        string method;
        string uri;
        string version;
	   string head;
	   int rpos;//读的位置

	
};
















#endif