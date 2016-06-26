/**
	httpserver
	SendQueueItem.h
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

#ifndef _SENDQUEUEITEM_H_
#define _SENDQUEUEITEM_H_

#include <cstdlib>

typedef unsigned char byte;

// SendQueueItem：在HTTP客户端发送队列中的一段数据
// 其中包含三个核心数据：
// 1. 指向发送数据的指针 
// 2. 数据块的长度
// 3. 当前已发送的数据长度
class SendQueueItem {

private:
    // 指向发送数据的指针
	byte* sendData;
    
    // 发送的数据量
	unsigned int sendSize;
    
    // 数据偏移
	unsigned int sendOffset;
    
    // 表示当前数据出队后，客户端是否需要断开
	bool disconnect; 

public:

    // 构造函数
	SendQueueItem(byte* data, unsigned int size, bool dc) {
		sendData = data;
		sendSize = size;
		disconnect = dc;
		sendOffset = 0;
	}

    // 析构函数，释放数据空间
	~SendQueueItem() {
		if(sendData != NULL) {
			delete [] sendData;
			sendData = NULL;
		}
	}

    // 获得及设置对象的数据成员
    
	void setOffset(unsigned int off) {
		sendOffset = off;
	}

	byte* getData() {
		return sendData;
	}

	unsigned int getSize() {
		return sendSize;
	}

	bool getDisconnect() {
		return disconnect;
	}

	unsigned int getOffset() {
		return sendOffset;
	}

};

#endif
