
#include "stdafx.h"

#include "thread_api.h"
#include "public_function.h"

#ifdef WIN32
  #include <process.h>    /* _beginthread, _endthread */
#endif

/*
 * ֪ʶ��:
 * һ����˵��VC�¶�����ϲ��_beginthread/_endthread.
 * ����CreateThread/CloseHandleӦ��Ҳ�������Ӧ�Ĺ���.
 * ��linux���ƺ�����ֻ��pthread_create,�������ͷž��ʲô��.
 * _endthread��д���߳����к����ĺ���,��CloseHandleӦ��Ҳ����,���������߳̽�����Ĵ����п��ܸ���ȫ?
 * ������д���߳����к�����ȷʵû������������ڴ��й©.
 *
 * --------------------------------------------------
 * MSDN �� CreateThread ����������������һ�λ�:
 * A thread that uses functions from the C run-time libraries should use the beginthread and endthread C run-time functions for thread management rather than CreateThread and ExitThread. Failure to do so results in small memory leaks when ExitThread is called. 
 * ����˵ CreateThread/ExitThread ���ڴ�й©? ��ʹ�� ExitThread �����Զ���ֹ�Ͳ����ڴ�й©����?
 * _endthread �İ�����˵�ø�����,������һ��Ȩ���Ե�����.
 * _beginthread �������о���ȷ˵�����߳��ڵ���_beginthreadʱ,�ǻ����߳̽���ʱ�����ص���_endthread��.���ƺ��Ӳ���֤���˲�ʹ��ExitThreadʱ��CreateThreadҲ�ǰ�ȫ��?
 * ���� _beginthread ʱ������ CloseHandle ���Ǻõ�.
 *
 * --------------------------------------------------
 * �ڡ�win32...���е� p249 �����зǳ��꾡��˵��,Ҫ�� _beginthreadex ������CreateThreadͬʱCloseHandleҲ��ȻҪ����,����CloseHandle�ǿ����������̺߳���κ�ʱ�̵���.
 */


//�����߳�//LPVOID��void *�ƺ������������
//thread_id create_thread(thread_func func,void * para)
thread_id create_thread(thread_func func,void * para)
{
#ifdef WIN32
	//thread_id r = ::CreateThread(0,0,(thread_func)func,LPVOID(this),0,0);
	//���õ�//thread_id r = ::CreateThread(0,0,(LPTHREAD_START_ROUTINE)func,para,0,0);
	thread_id r = (thread_id)_beginthreadex(0,0,func,para,0,0);
	::CloseHandle(r);//���������رգ���Ϊ�������ı��������յĻ� _beginthreadex �����Ѿ������ˣ�������ĵ��òŷ��ء�����������̺߳������Ѿ���������ô�ͻ�������(���ʵ��ǳ��ڴ�)��
#else
	thread_id r = pthread_create(&r,NULL,(void*(*)(void*))func,para);
#endif

  return r;
  
}

//˯��
void thread_sleep(int millis)
{  
#ifdef WIN32
  ::Sleep(millis);
#else
  //linux�µ���ͺ���Ҫ�ֿ���
  int p_sec = millis/1000;//ȡģ
  int p_usec = millis%1000;//ȡ��
  printfd3("p_sec:%d p_usec:%d\r\n",p_sec,p_usec);
  struct timespec delay;

  delay.tv_sec = p_sec;//��
  delay.tv_nsec = p_usec;//����

  nanosleep(&delay, NULL);
#endif

}

//�����߳���Դ//��Ҫ�������,Ӧ�����߳�����ʱ�رվ��
void clear_thread(thread_id thread)
{
#ifdef WIN32
	//::CloseHandle(thread);
	//::CloseHandle(::GetCurrentThread());
#else
  //linux��Ӧ���ǲ��õ�
#endif

}


