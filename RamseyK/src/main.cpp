/**
	httpserver
	main.cpp
	Copyright 2011-2012 Ramsey Kant

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

#include <iostream>
#include <string>
#include <signal.h>

#include "HTTPServer.h"

static HTTPServer* svr;

// 忽视 SIGPIPE "Broken pipe" 信号
void handleSigPipe(int snum) {
    return;
}

// 中断处理函数
void handleTermSig(int snum) {
    // 设置标志位表示停止服务器
	svr->canRun = false;
}

int main (int argc, const char * argv[])
{
    // 忽视 SIGPIPE "Broken pipe" 信号
    signal(SIGPIPE, handleSigPipe);

	// 注册中断处理
	signal(SIGABRT, &handleTermSig);
	signal(SIGINT, &handleTermSig);
	signal(SIGTERM, &handleTermSig);

    // 创建并启动HTTPServer实例
	svr = new HTTPServer();
	svr->start(8080);

	// 进入主循环
	svr->process();
	
	// 停止服务器
	svr->stop();
	delete svr;
    
    return 0;
}
