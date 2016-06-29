#ifndef _THREAD_API_H_
#define _THREAD_API_H_

#include "os.h"

//�̺߳���
//#define thread_func int
//typedef void (*thread_func)(void *);
//test//typedef void (*thread_func)(void *);

#ifdef WIN32
  typedef unsigned int (__stdcall*thread_func)(void *);
  //typedef void (*thread_func)(void *);//��windows��CreateThreadʱҲ�������
#else
  typedef void (*thread_func)(void *);//��windows��CreateThreadʱҲ�������
#endif

//�̴߳��������ķ���ֵ//��Ϊ�������ֵ�ڽ�����ϵͳ�в�һ������int������Ҫ�ض���
#ifdef WIN32
  #define thread_id HANDLE
#else
  #define thread_id pthread_t
#endif

//�����߳�
thread_id create_thread(thread_func func,void * para);
//˯��
void thread_sleep(int millis);
//�����߳���Դ
void clear_thread(thread_id thread);

#endif

