/**
 ByteBuffer
 HTTPResponse.h
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

#ifndef _HTTPRESPONSE_H_
#define _HTTPRESPONSE_H_

#include "HTTPMessage.h"

// HTTPResponse：HTTP响应消息，从服务器发给客户端的消息
class HTTPResponse : public HTTPMessage {
private:    
    // 响应状态码和信息
    int status;
    std::string reason;

    // 判断响应状态及响应消息
	void determineReasonStr();
	void determineStatusCode();

protected:
	virtual void init();
    
public:
    HTTPResponse();
    HTTPResponse(std::string sData);
    HTTPResponse(byte *pData, unsigned int len);
    virtual ~HTTPResponse();
    
    virtual byte* create();
    virtual bool parse();
    
    // 获取和设置响应状态码
    
    void setStatus (int scode) {
        status = scode;
		determineReasonStr();
    }
    
    std::string getReason() {
    	return reason;
    }
};

#endif
