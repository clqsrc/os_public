
#include "log.h"
#include "os.h"
#include "public_function.h"
#include "tmem.h"
#include "file_system.h"


//日志操作类,可自动截断太长的日志

CLog::CLog()
{
	m_LogFileName = "";//日志文件使用的文件名
	m_LogFileNameBak = "";//日志文件使用的备份文件名

	m_iWriteLineCount = 0;
	m_LogFile = NULL;

}

CLog::~CLog()
{

}


//记录信息
void CLog::Log(const std::string s)
{
	//return;//test
	m_Lock.lock();

	//如果文件写入太多了
	if (m_iWriteLineCount > 10000)
	{
		m_iWriteLineCount = 0;
		fclose(m_LogFile);
		m_LogFile = NULL;

		//复制文件
		file_system::copy_file(m_LogFileName, m_LogFileNameBak);
		//删除旧文件
		file_system::delete_file(m_LogFileName);
	
	}


	//打开文件
	if (m_LogFile == NULL)
	{
		//m_LogFile = fopen(m_LogFileName.c_str(), "a+b");//追加模式
		m_LogFile = open_log_file(m_LogFileName);
	}

	if (m_LogFile == NULL)
	{
		printf("CLog::Log() 日志文件:%打开失败.\r\n", m_LogFileName.c_str());
		m_Lock.unlock();
		return;
	}

	fprintf(m_LogFile, "%s", s.c_str());

	//写入时间
	tm m;
	memset(&m, 0, sizeof(tm));
	time_t t = time(NULL);
	tm * pm = localtime(&t);

	if (pm != NULL)
	{
		m = * pm;
	}

	fprintf(m_LogFile, "%d.%d.%d %d:%d\r\n", m.tm_year + 1900, m.tm_mon + 1, m.tm_mday, m.tm_hour, m.tm_min);

	m_iWriteLineCount++;

	m_Lock.unlock();
}


//打开日志文件
void CLog::OpenLog(const std::string fn)
{
	m_LogFileName = fn;//日志文件使用的文件名
	m_LogFileNameBak = fn + ".bak";//日志文件使用的备份文件名

	
}



	
	
	