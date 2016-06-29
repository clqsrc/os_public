#ifndef _LOCK_RW_H_
#define _LOCK_RW_H_

//读写锁定的实现//对于很多数据只要初始化的时候需要锁定，其余时间是只要读取的，所以有必要实现一个//新写的，尽量少用

#include "os.h"
#include "thread_lock.h"

class LockRW
{
public:
	LockRW();
	~LockRW();

private:
	//必须由两层锁来实现
	thread_lock m_LockLevel1;//这个只是为了保护各个变量的读写同步
	thread_lock m_LockLevel2;
	
	int m_iReadCount;
	bool m_bHaveWrite;//有写请求在锁定队列中
	
public:
	//读锁
	void LockForRead();
	void UnLockForRead();
	//写锁
	void LockForWrite();
	void UnLockForWrite();

};

#endif
