/**
   ByteBuffer
   ByteBuffer.h
   Copyright 2011 Ramsey Kant

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

// 定义字节流缓存
#ifndef _BYTEBUFFER_H_
#define _BYTEBUFFER_H_

// 默认为ByteBuffer分配的大小
#define DEFAULT_SIZE 4096

#include <cstdlib>
#include <cstring>
#include <vector>

typedef unsigned char byte;

// ByteBuffer：定义一个缓存队列用来存储HTTP访问中的请求及返回数据
// ByteBuffer的核心功能是：
// 1. 从队列头部读取数据
// 2. 向队列尾部写入新的数据
// 3. 当队列存储区域满的时候可以自动扩充
// 4. 随机读取和写入多种数据类型

class ByteBuffer {
private:

    // 读写位置索引
	unsigned int rpos, wpos;
    
    // 缓存内容使用容器存储
	std::vector<byte> buf;

    // 读取下一个元素
    template <typename T> T read() {
		T data = read<T>(rpos);
		rpos += sizeof(T);
		return data;
	}
	
    // 根据位置读取，读取index位置的元素
	template <typename T> T read(unsigned int index) const {
		if(index + sizeof(T) <= buf.size())
			return *((T*)&buf[index]);
		return 0;
	}

    // 添加新的元素，如果队列已满则需要resize扩充
	template <typename T> void append(T data) {
		unsigned int s = sizeof(data);

		if (size() < (wpos + s))
			buf.resize(wpos + s);
		memcpy(&buf[wpos], (byte*)&data, s);

		wpos += s;
	}
	
    // 插入元素到指定位置
	template <typename T> void insert(T data, unsigned int index) {
		if((index + sizeof(data)) > size())
			return;

		memcpy(&buf[index], (byte*)&data, sizeof(data));
		wpos = index+sizeof(data);
	}

public:

    // 构造函数
	ByteBuffer(unsigned int size = DEFAULT_SIZE);
	ByteBuffer(byte* arr, unsigned int size);
	virtual ~ByteBuffer();

    // 队列中剩余的元素数量
	unsigned int bytesRemaining(); 
    
    // 清空队列并重置读写索引
	void clear();
    
    // 拷贝当前ByteBuffer对象
	ByteBuffer* clone(); 
    
    // 对比两个ByteBuffer对象
	bool equals(ByteBuffer* other); 
    
    // 设置存储空间大小
	void resize(unsigned int newSize);
    
    // 返回存储空间大小
	unsigned int size(); 
    
    // 线性查找，根据key查找对象，返回对象所在的索引位置
    template <typename T> int find(T key, unsigned int start=0) {
        int ret = -1;
        unsigned int len = buf.size();
        for(unsigned int i = start; i < len; i++) {
            T data = read<T>(i);
            // 没有数据了，返回-1表示没有找到元素
            if((key != 0) && (data == 0))
                break;
            
            // 找到元素
            if(data == key) {
                ret = i;
                break;
            }
        }
        return ret;
    }
    
    // 替代函数
    void replace(byte key, byte rep, unsigned int start = 0, bool firstOccuranceOnly=false);
	
	// 读取函数列表

    // 读取队列头部的数据但不移动rpos
	byte peek(); 
    // 读取队列头部的数据同时移动rpos
	byte get();
    // 读取指定位置的数据
	byte get(unsigned int index); 
    // 读取队列头部指定长度的数据到buf中
	void getBytes(byte* buf, unsigned int len); 
    // 读取指定数据类型的元素：char，double，float，int，long，short
	char getChar(); 
	char getChar(unsigned int index);
	double getDouble();
	double getDouble(unsigned int index);
	float getFloat();
	float getFloat(unsigned int index);
	int getInt();
	int getInt(unsigned int index);
	long getLong();
	long getLong(unsigned int index);
	short getShort();
	short getShort(unsigned int index);

	// 写入函数列表

    // 将src写入队列
	void put(ByteBuffer* src);
    // 将b写入队列
	void put(byte b); 
    // 将b写入队列指定位置
	void put(byte b, unsigned int index); 
    // 将长度为len的缓存b写入队列头部
	void putBytes(byte* b, unsigned int len); 
    // 将长度为len的缓存b写入队列指定位置
	void putBytes(byte* b, unsigned int len, unsigned int index); 
    // 写入指定数据类型的元素：char，double，float，int，long，short
	void putChar(char value);
	void putChar(char value, unsigned int index); 
	void putDouble(double value);
	void putDouble(double value, unsigned int index);
	void putFloat(float value);
	void putFloat(float value, unsigned int index);
	void putInt(int value);
	void putInt(int value, unsigned int index);
	void putLong(long value);
	void putLong(long value, unsigned int index);
	void putShort(short value);
	void putShort(short value, unsigned int index);

	// 读取和写入位置设置及获取

	void setReadPos(unsigned int r) {
		rpos = r;
	}

	int getReadPos() {
		return rpos;
	}

	void setWritePos(unsigned int w) {
		wpos = w;
	}

	int getWritePos() {
		return wpos;
	}

};

#endif
