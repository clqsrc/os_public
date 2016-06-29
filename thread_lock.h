#ifndef _THREAD_LOCK_H_
#define _THREAD_LOCK_H_


#include "os.h"

class thread_lock
{
public:
  thread_lock();
  virtual ~thread_lock();

private:
  bool is_locked;
#ifdef WIN32
  HANDLE lock_id;
#else
  pthread_mutex_t lock_id;
#endif
public:
  void lock();
  bool lock_non_block();
  void unlock();

};

#endif
