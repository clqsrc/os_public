
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
//linux�£���������ͬһ���߳���Ҳֻ������һ�Ρ���windows��mutex+WaitForSingleObjectȴ������ͬһ��
//�߳������룬����Ҫ��һ����־is_locked�������Ƿ��Ѿ�����ǰ�߳������ˣ�ʹ֮��ͬһ�߳���Ҳֻ��һ��
void thread_lock::lock()
{
#ifdef WIN32
  ::WaitForSingleObject(lock_id, INFINITE);
#else
  //pthread_mutex_trylock���������صģ������Ƿ������ɹ�
  pthread_mutex_lock(&lock_id);
#endif

  while(is_locked==true)
  {
    thread_sleep(1000);//��ʵ�������ˣ���linux���ǽ��벻�������//windows��Ӧ��Ҳֻ�б��̲߳��ܽ���[��windows���ǿ������]
  };
  is_locked = true;//Ҫ�ڽ�����ֵ
}

//������������,�����ǰ��������,��������//Ӧ����������,��Ϊ��ֲ����δ��֤
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
		::ReleaseMutex(lock_id);//��ΪWaitForSingleObject�ֱ�������һ��,����Ӧ������һ��ReleaseMutex
		return false;
	}
  }
#else
  //pthread_mutex_trylock���������صģ������Ƿ������ɹ�
  //pthread_mutex_lock(&lock_id);
  if (pthread_mutex_trylock(&lock_id)!=0)//��ʧ����,��������
  {
	  return false;
  }
#endif

  while(is_locked==true)
  {
    thread_sleep(1000);//��ʵ�������ˣ���linux���ǽ��벻�������
  };
  is_locked = true;//Ҫ�ڽ�����ֵ
  return true;
}

void thread_lock::unlock()
{
  is_locked = false;//Ҫ�ڽ���ǰ��ֵ
#ifdef WIN32
  ::ReleaseMutex(lock_id);
#else
  pthread_mutex_unlock(&lock_id);
#endif
}



