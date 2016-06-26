/**
	httpserver
	Resource.h
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

#ifndef _RESOURCE_H_
#define _RESOURCE_H_

#include <string>

typedef unsigned char byte;

// Resource：HTTP响应消息中的文件资源数据
class Resource {

private:

    // 文件资源
    byte* data;
    // 资源大小
	unsigned int size;
    // 资源类型
	std::string mimeType;
    // 资源在服务器上存储的路径
    std::string location;
    // 是否是目录类型
	bool directory;

public:
    // 构造与析构函数
    Resource(std::string loc, bool dir = false);
    ~Resource();
    
    // 设置资源及资源类型
    
    void setData(byte* d, unsigned int s) {
        data = d;
		size = s;
    }

	void setMimeType(std::string mt) {
		mimeType = mt;
	}
    
    // 获取资源路径及资源类型

	std::string getMimeType() {
		return mimeType;
	}
    
    std::string getLocation() {
        return location;
    }

    // 是否是目录类型
	bool isDirectory() {
		return directory;
	}

    // 返回资源数据及大小
	byte* getData() {
		return data;
	}
	
	unsigned int getSize() {
		return size;
	}
	
	// 获取文件的扩展名，可以根据扩展名判断MimeType
	std::string getExtension() {
		std::string ext = "";
		size_t ext_pos = location.find_last_of(".");
		if(ext_pos != std::string::npos)
			ext = location.substr(ext_pos+1);
		return ext;
	}
};

#endif
