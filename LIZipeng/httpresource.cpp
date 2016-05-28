#include "httpresource.h"

httpresource::httpresource(string path,string fil,string look_mime)
{
	std::string baseDiskPath=path+fil;
	std::string mime=look_mime;
}
char* httpresource::readfile()
{
	std::ifstream is;
	is.open(baseDiskPath.c_str(),std::ifstream::in);
	is.seekg(0,is.end);
	int length=is.tellg();
	char *buffer=new char[length];
	is.getline(buffer,length);
	if(buffer!=NULL)
	return buffer;
    else  
     return NULL;
}
char* httpresource::getstatus(char* buf)
{
	if(buf!=NULL)
	{
		char*p=new char[3];
		strcpy(p,resstatus[1]);
		return p;
	}
	else
	{
		char*p=new char[3];
		strcpy(p,resstatus[4]);
		return p;
		
	}
}

char* httpresource::gettime()
{
	time_t cur_t;
	struct tm* cur_tm;
	time(&cur_t);
	cur_tm=localtime(&cur_t);
	char* time_buffer=asctime(cur_tm);
	string ret=time_buffer;
	istringstream is(ret);
	string ret1,ret2,ret3,ret4,ret5;
	is>>ret1>>ret2>>ret3>>ret4>>ret5;
	string ret6;
	ret6.clear();
	ret6+=ret1;
	ret6+=", ";
	ret6+=ret3;
	ret6+=" ";
	ret6+=ret2;
	ret6+=" ";
	ret6+=ret5;
	ret6+=" ";
	ret6+=ret4;
	ret6+=" GMT";
	int len=sizeof(ret6.c_str())/sizeof(char);
	char* buf=new char[len];
	strcpy(buf,ret6.c_str());
	return buf;
}

string httpresource::make_message(char* file,char* status,char* time,string ver)
{


		string resmessage;
		resmessage.clear();
		resmessage=ver;
		resmessage+=" ";
		resmessage+=status;
		resmessage+=" ";
		resmessage+="OK\r\n";
		resmessage+="Date:";
		resmessage+=" ";
		resmessage+=time;
		resmessage+="\r\n";
		resmessage+="Content-Type: ";
		resmessage+=mime;
		resmessage+="; ";
		resmessage+="charset";
		resmessage+="UTF-8";
		resmessage+="\r\n";
		resmessage+="\r\n";
		resmessage+=file;
		return resmessage;

	
	
}