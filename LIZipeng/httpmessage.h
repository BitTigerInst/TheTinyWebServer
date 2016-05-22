#ifndef HTTP_MESSAGE_H
#define HTTP_MESSAGE_H
#include "commom.h"
/*************信息类****************************/
/*成员变量：1.操作方法
          2.资源
		  3.版本号
成员函数：
************************************************/
class httpmessage
{
	public:
	httpmessage();
	int parseline(string q);//得到头信息的初始位置
	void parsehead(string p,int num);//分析头信息，将头信息加入到关联容器
	~httpmessage();   
	protected:
	   string method;
	   string uri;
	   string version;
	   string head;
	   int rpos;//读的位置
	   map<string,string> headermessage;
	   
	
	
	
};






#endif