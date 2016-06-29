#pragma once

#include "public_function.h"
//#include "file_system.h"


class ThreadDebug
{
public:
	thread_lock lock;

	ThreadDebug(void);
	~ThreadDebug(void);

	//�ڵ�ǰ�߳���־�м�¼һ����Ϣ
	void ThreadDebug::WriteLog(const char * s);

	void ThreadDebug::WriteLog(const char * s, int thread_id);

	//����ǰ�̵߳ļ�¼�ļ�
	void ThreadDebug::FreeLog();
	void ThreadDebug::FreeLog(int thread_id);
};

extern ThreadDebug g_threadDebug;
