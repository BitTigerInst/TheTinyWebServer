
#include "httprequest.h"
httprequest::httprequest(char *p,int fd_1)
{
	req=p;
    clientfd=fd_1;
}
int httprequest::parse()
{
   int r_pos=parseline(req);
   parsehead(req,r_pos);
   return 1;
   
}
int httprequest::support_opt()
{

		if(method.compare("GET")==0)
		{
	if(version.compare("HTTP/1.1")==0)
		return 1;
	else
		return -1;
		}
}
httprequest::~httprequest(){}
int httprequest::getclient()
{
	return clientfd;
	
}
string httprequest::getmethod()
{
	return method;
}
string httprequest::geturi()
{
	return uri;
}
string httprequest::getversion()
{
	return version;
}
int httprequest::parseline(string q)
{
	cout<<q<<endl;
    rpos=0;
    istringstream is(q);
    is>>method>>uri>>version;
    cout<<method<<","<<uri<<","<<version<<endl;//测试method，uri,version是否被赋值
	while(q[rpos]!='\r')
	{
		rpos++;
	}
    rpos+=2;
	cout<<"rpos="<<rpos<<endl;
	return rpos;
	
}
void httprequest::parsehead(string p,int num)
{

	for(int i=1;i<=6;i++)
	{
	    int num_1=num;
	    int num_2=num;
	    while(p[num_2]!='\r')
		{
           num_2++;
		}
		while(p[num_1]!=':')
		{
			num_1++;
		}
		    cout<<p.substr(num,num_1-num)<<":"<<p.substr(num_1+1,num_2-num_1-1)<<endl;

			headermessage.insert(std::pair<string,string>(p.substr(num,num_1-num),p.substr(num_1+1,num_2-num_1-1)));
		
		num=num_2+2;

	}

}

string httprequest::getheadmessage()
{
	string mess=headermessage.find("Accept")->second;//获取Accept后的后缀
	return mess;
}