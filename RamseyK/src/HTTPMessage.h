/**
	ByteBuffer
	HTTPMessage.h
	Copyright 2011-2014 Ramsey Kant

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

	    http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef _HTTPMESSAGE_H_
#define _HTTPMESSAGE_H_

#include <map>
#include <string>
#include <sstream>

#include "ByteBuffer.h"

// HTTP消息头部信息常量
// 标志着HTTP消息的版本号
#define HTTP_VERSION_10 "HTTP/1.0"
#define HTTP_VERSION_11 "HTTP/1.1"
#define DEFAULT_HTTP_VERSION "HTTP/1.1"

// 支持的HTTP方法数量
#define NUM_METHODS 9

// HTTP请求方法
enum Method {
	HEAD = 0,
	GET = 1,
	POST = 2,
	PUT = 3,
	DEL = 4, 
	TRACE = 5,
	OPTIONS = 6,
	CONNECT = 7,
	PATCH = 8
};

const static char* const requestMethodStr[NUM_METHODS] = {
	"HEAD", // 0
	"GET", // 1
	"POST", // 2
	"PUT", // 3
	"DELETE", // 4
	"TRACE", // 5
	"OPTIONS", // 6
	"CONNECT", // 7
	"PATCH" // 8
};


// HTTP回应状态码
enum Status {
    // 1xx 消息类型
    CONTINUE = 100,
    
    // 2xx 成功
    OK = 200,
    
    // 3xx 重定向
    
    // 4xx 客户端请求错误
    BAD_REQUEST = 400,
    NOT_FOUND = 404,
    
    // 5xx 服务器错误
    SERVER_ERROR = 500,
    NOT_IMPLEMENTED = 501
};

// HTTPMessage：存储HTTP消息的一段缓存
// 
class HTTPMessage : public ByteBuffer {
private:

    // 消息头部信息
    std::map<std::string, std::string> *headers;

protected:
    // 解析错误信息
	std::string parseErrorStr;
	
    // HTTP版本号
	std::string version;
	
	// 消息数据
    // 响应消息中表示资源，请求消息中表示额外的参数
	byte* data;
    
    // 消息数据长度
	unsigned int dataLen;

protected:
    
    // 初始化消息
    virtual void init();
    
public:

    // 构造函数
    HTTPMessage();
    HTTPMessage(std::string sData);
    HTTPMessage(byte *pData, unsigned int len);
    virtual ~HTTPMessage();
    
    // 创建消息
	virtual byte* create() = 0;
    
    // 解析消息
	virtual bool parse() = 0;
    
    // 创建消息的功能函数
    void putLine(std::string str = "", bool crlf_end=true);
    void putHeaders();
    
    // 解析消息的功能函数
	std::string getLine();
    std::string getStrElement(char delim = 0x20); // 0x20 = "space"
    void parseHeaders();
    bool parseBody();
    
    // 消息头部管理函数
	void addHeader(std::string line);
    void addHeader(std::string key, std::string value);
	void addHeader(std::string key, int value);
    std::string getHeaderValue(std::string key);
	std::string getHeaderStr(int index);
	int getNumHeaders();
    void clearHeaders();
    
    // 获取及设置数据成员

	std::string getParseError() {
		return parseErrorStr;
	}
	
    void setVersion(std::string v) {
        version = v;
    }
    
    std::string getVersion() {
        return version;
    }
    
	void setData(byte* d, unsigned int len) {
		data = d;
		dataLen = len;
	}

	byte* getData() {
		return data;
	}

	unsigned int getDataLength() {
		return dataLen;
	}
};

#endif
