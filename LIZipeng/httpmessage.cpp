#include "commom.h"
#include "httpmessage.h"
httpmessage::httpmessage()
{ 
      method.clear();
	  uri.clear();
	  version.clear();
	
}
int httpmessage::parseline(string q)
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
void httpmessage::parsehead(string p,int num)
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
httpmessage::~httpmessage(){}
