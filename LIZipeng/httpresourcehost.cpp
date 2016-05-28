#include "httpresourcehost.h"

httpresourcehost::httpresourcehost(string uri_4,string message,string ver_2)
{
		string mess=message;
		uri_3=uri_4;
		ver_3=ver_2;
} 

string httpresourcehost:: getextension()
{
     
     std::size_t pos=mess.find(",");
	 std::size_t startpos=1;
	 string exten=mess.substr(startpos,pos-startpos);
	 cout<<"The extension is"<<exten<<endl;
	 return exten;

}

string httpresourcehost::findMimeType(string ext)
{
	/*unordered_map<std::string,std::string>::const_iterator got=mimeMap.find(ext);
	if(got!=mimeMap.end())
	{
		lookmime=got->second£»
		string responsedata=openfile(got->second,uri);
		
	}
	else
	{
		cout<<"No one"<<endl;
		
	}*/
	return "text/html";
}

string httpresourcehost::openfile(string mimetype,string file)
{
	if(mimetype=="text/html")
	{
		string a=lookmime;
		httpresource *res=new httpresource("./htcos",file,a);
		char* buffer=res->readfile();
        char* sta_buffer=res->getstatus(buffer);
		char* time_buffer=res->gettime();
		string ver_4=getver();
		string responsemessage=res->make_message(buffer,sta_buffer,time_buffer,ver_4);
		delete res;
		return responsemessage;
	}
	else
	{
		
		
	}
	
}

string httpresourcehost::getver()
{
	
	return ver_3;
	
}