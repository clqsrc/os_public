
#include "stdafx.h"

#include "thread_lock.h"
#include "thread_api.h"

thread_lock::thread_lock()
{
  is_locked = false;
#ifdef WIN32
  lock_id = ::CreateMutex(NULL, false, NULL);
#else
  pthread_mutex_init(&lock_id, NULL);
#endif
}

thread_lock::~thread_lock()
{
  unlock();
#ifdef WIN32
  CloseHandle(lock_id);
#else
  pthread_mutex_destroy(&lock_id);
#endif
}

////////////////////////////////////////////////////////////
//linux下，就算是在同一个线程中也只能锁定一次。而windows的mutex+WaitForSingleObject却可以在同一个
//线程中重入，所以要加一个标志is_locked，表明是否已经被当前线程锁定了，使之在同一线程中也只能一次
void thread_lock::lock()
{
#ifdef WIN32
  ::WaitForSingleObject(lock_id, INFINITE);
#else
  //pthread_mutex_trylock是立即返回的，不论是否锁定成功
  pthread_mutex_lock(&lock_id);
#endif

  while(is_locked==true)
  {
    thread_sleep(1000);//其实是死锁了，在linux下是进入不到这里的//windows下应该也只有本线程才能进入[即windows下是可重入的]
  };
  is_locked = true;//要在解锁后赋值
}

//无阻塞的锁定,如果当前不能锁定,立即返回//应当尽量少用,因为移植性尚未论证
bool thread_lock::lock_non_block()
{
#ifdef WIN32
  if (::WaitForSingleObject(lock_id, 0)!=WAIT_OBJECT_0)
  {
	return false;
  }
  else
  {
	if (is_locked==true)
	{
		::ReleaseMutex(lock_id);//因为WaitForSingleObject又被调用了一次,所以应当再有一次ReleaseMutex
		return false;
	}
  }
#else
  //pthread_mutex_trylock是立即返回的，不论是否锁定成功
  //pthread_mutex_lock(&lock_id);
  if (pthread_mutex_trylock(&lock_id)!=0)//锁失败了,立即返回
  {
	  return false;
  }
#endif

  while(is_locked==true)
  {
    thread_sleep(1000);//其实是死锁了，在linux下是进入不到这里的
  };
  is_locked = true;//要在解锁后赋值
  return true;
}

void thread_lock::unlock()
{
  is_locked = false;//要在解锁前赋值
#ifdef WIN32
  ::ReleaseMutex(lock_id);
#else
  pthread_mutex_unlock(&lock_id);
#endif
}



