#ifndef _LOG_H__
#define _LOG_H__

//取消VC中使用STL的警告
#pragma warning(disable:4503)
#pragma warning(disable:4786)

#include <string>

#include "os.h"
#include "public_function.h"
#include "tmem.h"

//日志操作类,可自动截断太长的日志
class CLog
{
public://变量

private:
	std::string m_LogFileName;//日志文件使用的文件名
	std::string m_LogFileNameBak;//日志文件使用的备份文件名

	int m_iWriteLineCount;//当前已经写入的行数
	FILE * m_LogFile;//用来操作的文件指针

	thread_lock m_Lock;

public://函数
	CLog::CLog();
	CLog::~CLog();

	//记录信息
	void CLog::Log(const std::string s);
	
	//打开日志文件
	void CLog::OpenLog(const std::string fn);

};


#endif


