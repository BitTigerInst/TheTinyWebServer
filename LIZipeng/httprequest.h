#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H
#include "commom.h"
#include "httpmessage.h"
/************************请求类************************************/
/*成员变量：1.解释的内容
          2.http的读位置记录
成员函数：1.解释*/	
/******************************************************************/	
class httprequest:public httpmessage{
	public:
	   httprequest(char* p);
	   ~httprequest();
	   int parse();
	 
	private:
	   string req;

	
	
	
};
















#endif