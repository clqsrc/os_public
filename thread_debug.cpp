#include "StdAfx.h"
#include "thread_debug.h"

ThreadDebug g_threadDebug;

ThreadDebug::ThreadDebug(void)
{
}

ThreadDebug::~ThreadDebug(void)
{
}


std::string GetAppFilename()
{
	std::string r = "";

	tmem t(MAX_PATH);//自释放临时内存
	char * buf = t.buf;

	::GetModuleFileNameA(NULL, buf, MAX_PATH);
	r = buf;

	//printf("%s\r\n", r.c_str());

	return r;
}

void ThreadDebug::WriteLog(const char * s)
{
	if (s == NULL) return;

	int thread_id = (int)::GetCurrentThreadId();
	//std::string thread_id_s = int_to_str(thread_id);

	WriteLog(s, thread_id);

}

void ThreadDebug::WriteLog(const char * s, int thread_id)
{
	if (s == NULL) return;

	//int thread_id = (int)::GetCurrentThreadId();
	std::string thread_id_s = int_to_str(thread_id);

	FILE * logfile = NULL;

	if (logfile == NULL)
	{
		std::string path = extract_file_path(GetAppFilename());
		std::string fn = path + "/log_thread_" + thread_id_s + ".txt";
		logfile = fopen((fn).c_str(), "a+b");
	}

	if (logfile != NULL)
	{

		//高精度时间
		LARGE_INTEGER now_h;
		QueryPerformanceCounter(&now_h);

		std::string now = safe_std_string(GetMysqlDateTime());
		//fprintf(logfile, "%s\t%s\r\n", now.c_str(), s);
		//fprintf(logfile, "%s %d\t%s\r\n", now.c_str(), ::GetTickCount(), s);
		//fprintf(logfile, "%s %d %d %d\t%s\r\n", now.c_str(), ::GetTickCount(), now_h.HighPart, now_h.LowPart, s);
		fprintf(logfile, "%s %d (ms) %lu \t%s\r\n", now.c_str(), ::GetTickCount(), now_h.LowPart, s);//感觉没必要用高字节
		
		//fprintf(logfile, "%s\r\n", s);
		fflush(logfile);

		fclose(logfile);
	}

}

//清理当前线程的记录文件
void ThreadDebug::FreeLog()
{
	int thread_id = (int)::GetCurrentThreadId();
	
	FreeLog(thread_id);

}

//清理当前线程的记录文件
void ThreadDebug::FreeLog(int thread_id)
{
	//int thread_id = (int)::GetCurrentThreadId();
	std::string thread_id_s = int_to_str(thread_id);

	std::string path = extract_file_path(GetAppFilename());
	std::string fn = path + "\\log_thread_" + thread_id_s + ".txt";
	
	::DeleteFileA(fn.c_str());

}


