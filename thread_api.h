#ifndef _THREAD_API_H_
#define _THREAD_API_H_

#include "os.h"

//线程函数
//#define thread_func int
//typedef void (*thread_func)(void *);
//test//typedef void (*thread_func)(void *);

#ifdef WIN32
  typedef unsigned int (__stdcall*thread_func)(void *);
  //typedef void (*thread_func)(void *);//在windows用CreateThread时也可用这个
#else
  typedef void (*thread_func)(void *);//在windows用CreateThread时也可用这个
#endif

//线程创建函数的返回值//因为这个返回值在将来的系统中不一定等于int，所以要重定义
#ifdef WIN32
  #define thread_id HANDLE
#else
  #define thread_id pthread_t
#endif

//创建线程
thread_id create_thread(thread_func func,void * para);
//睡眠
void thread_sleep(int millis);
//清理线程资源
void clear_thread(thread_id thread);

#endif

