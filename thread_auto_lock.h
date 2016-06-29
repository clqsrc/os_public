#ifndef _THREAD_AUTO_LOCK_H_
#define _THREAD_AUTO_LOCK_H_


//#include "os.h"
#include "thread_lock.h"

//要与 thread_lock 搭配使用

class thread_auto_lock
{
public:
  thread_auto_lock(thread_lock * lock)
  {
  	this->lock = lock;
  	this->lock->lock();	
  }
  
  ~thread_auto_lock()
  {
  	this->lock->unlock();	
  }

private:
	thread_lock * lock;


};

#endif
