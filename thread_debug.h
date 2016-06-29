#pragma once

#include "public_function.h"
//#include "file_system.h"


class ThreadDebug
{
public:
	thread_lock lock;

	ThreadDebug(void);
	~ThreadDebug(void);

	//在当前线程日志中记录一段信息
	void ThreadDebug::WriteLog(const char * s);

	void ThreadDebug::WriteLog(const char * s, int thread_id);

	//清理当前线程的记录文件
	void ThreadDebug::FreeLog();
	void ThreadDebug::FreeLog(int thread_id);
};

extern ThreadDebug g_threadDebug;
