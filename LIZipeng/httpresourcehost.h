#ifndef HTTPRESOURCE_H_HOST
#define HTTPRESOURCE_H_HOST
#include "commom.h"
#include "httpresource.h"

class httpresourcehost
{
	
	public:
	    httpresourcehost(string uri_4,string message,string ver_2);
		~httpresourcehost();
		string getextension();
	    string findMimeType(string ext);//参照服务器的MIMEtype，查看是否有这一类型的文件
	    string openfile(string mimetype,string file);//读取文件的内容，返回到stat file中，之后根据file的内容来判断是目录还是文件
        string getver();
		
	private:

        /*std::unordered_map<std::string, std::string> mimeMap = {
		#define STR_PAIR(K,V) std::pair<std::string, std::string>(K,V)
		#include "MimeTypes.inc"
	};*/
//构建扩展名和MimeTypes的映射表
	    std::string ext;//文件扩展名
	    std::string lookmime;//存放从MimeTypes中找到的类型名
	    std::string uri_3;
		std::string mess;
		std::string ver_3;
	
	
	
	
		
}; 



















#endif
