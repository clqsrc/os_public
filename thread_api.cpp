
#include "stdafx.h"

#include "thread_api.h"
#include "public_function.h"

#ifdef WIN32
  #include <process.h>    /* _beginthread, _endthread */
#endif

/*
 * 知识点:
 * 一般来说在VC下多数人喜欢_beginthread/_endthread.
 * 不过CreateThread/CloseHandle应该也能完成相应的工作.
 * 而linux下似乎可以只用pthread_create,而无需释放句柄什么的.
 * _endthread是写在线程运行函数的后面,而CloseHandle应该也可以,不过放在线程结束后的代码中可能更安全?
 * 不过我写在线程运行函数后确实没发生句柄或是内存的泄漏.
 *
 * --------------------------------------------------
 * MSDN 的 CreateThread 帮助中有下面这样一段话:
 * A thread that uses functions from the C run-time libraries should use the beginthread and endthread C run-time functions for thread management rather than CreateThread and ExitThread. Failure to do so results in small memory leaks when ExitThread is called. 
 * 就是说 CreateThread/ExitThread 会内存泄漏? 不使用 ExitThread 而是自动终止就不会内存泄漏了吗?
 * _endthread 的帮助中说得更明白,并且有一个权威性的例子.
 * _beginthread 的例子中就明确说明了线程在调用_beginthread时,是会在线程结束时隐含地调用_endthread的.这似乎从侧面证明了不使用ExitThread时的CreateThread也是安全的?
 * 不过 _beginthread 时不用再 CloseHandle 总是好的.
 *
 * --------------------------------------------------
 * 在《win32...》中的 p249 左右有非常详尽的说明,要用 _beginthreadex 来代替CreateThread同时CloseHandle也仍然要调用,而且CloseHandle是可以在启动线程后的任何时刻调用.
 */


//创建线程//LPVOID与void *似乎还是有区别的
//thread_id create_thread(thread_func func,void * para)
thread_id create_thread(thread_func func,void * para)
{
#ifdef WIN32
	//thread_id r = ::CreateThread(0,0,(thread_func)func,LPVOID(this),0,0);
	//可用的//thread_id r = ::CreateThread(0,0,(LPTHREAD_START_ROUTINE)func,para,0,0);
	thread_id r = (thread_id)_beginthreadex(0,0,func,para,0,0);
	::CloseHandle(r);//最好在这里关闭，因为如果用类的变量来接收的话 _beginthreadex 可能已经返回了，而这里的调用才返回。如果类是在线程函数中已经销毁了那么就会有问题(访问到非常内存)。
#else
	thread_id r = pthread_create(&r,NULL,(void*(*)(void*))func,para);
#endif

  return r;
  
}

//睡眠
void thread_sleep(int millis)
{  
#ifdef WIN32
  ::Sleep(millis);
#else
  //linux下的秒和毫秒要分开算
  int p_sec = millis/1000;//取模
  int p_usec = millis%1000;//取余
  printfd3("p_sec:%d p_usec:%d\r\n",p_sec,p_usec);
  struct timespec delay;

  delay.tv_sec = p_sec;//秒
  delay.tv_nsec = p_usec;//毫秒

  nanosleep(&delay, NULL);
#endif

}

//清理线程资源//不要调用这个,应该在线程启动时关闭句柄
void clear_thread(thread_id thread)
{
#ifdef WIN32
	//::CloseHandle(thread);
	//::CloseHandle(::GetCurrentThread());
#else
  //linux下应当是不用的
#endif

}


