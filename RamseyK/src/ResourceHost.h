/**
	httpserver
	ResourceHost.h
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

#ifndef _RESOURCEHOST_H_
#define _RESOURCEHOST_H_

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unordered_map>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

#include "Resource.h"

// 访问目录时默认响应文件，目前仅支持html类型
const static char* const validIndexes[] = {
	"index.html", // 0
	"index.htm", // 1
};

// ResourceHost：资源主机，用来将服务器上的文件加载到Resource对象
class ResourceHost {
private:
    // 基础目录，其他的访问路径都是该目录下的相对路径
    std::string baseDiskPath;

	// 构建扩展名与MimeType对应的字典，从文件MimeTypes.inc中获取
	std::unordered_map<std::string, std::string> mimeMap = {
		#define STR_PAIR(K,V) std::pair<std::string, std::string>(K,V)
		#include "MimeTypes.inc"
	};
    
private:
	// 根据扩展名返回MIMEType
	std::string lookupMimeType(std::string ext);

	// 加载文件到资源对象
	Resource* readFile(std::string path, struct stat sb);
	
	// 加载目录列表或index文件到资源对象
	Resource* readDirectory(std::string path, struct stat sb);
	
	// 根据URL返回目录的列表字符串
    std::string generateDirList(std::string dirPath);
    
public:
    ResourceHost(std::string base);
    ~ResourceHost();

	// 将资源写入到文件系统
	void putResource(Resource* res, bool writeToDisk);
    
    // 根据URI获取Resource资源对象
    Resource* getResource(std::string uri);
};

#endif
