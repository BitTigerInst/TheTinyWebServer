#ifndef HTTP_RESOURCE_H
#define HTTP_RESOURCE_H
#include "commom.h"


class httpresource
{
	public:
	    httpresource(string path,string fil,string look_mime);//初始化路径
	   ~httpresource();
        
	    char* readfile();//读取文件
		char* getstatus(char* buf);//获取状态码
		char* gettime();//获取当前系统时间
		string make_message(char* file,char* status,char* time,string ver);//整合好反馈信息
		
	    
	private:
	    std::string baseDiskPath;//表示文件路径
		std::string mime;//存放映射表的类型名
		const char* resstatus[5]={"101","202","303","404","505"};
};

#endif