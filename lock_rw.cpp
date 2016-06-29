#include "os.h"
#include "thread_lock.h"
#include "lock_rw.h"
#include "public_function.h"
#include <stdio.h>

//原理：当有读线程1时锁定 level1 锁定，发现是第一个读取的时候还要锁 level2. 最后释放 level1 锁定
//当有读线程2时与上类似，但不锁定 level2 锁，只是读计数器加 1
//读取线程全部离开后才释放 level2 ,但是这样写锁就太靠后了，应该是当处理写状态时也要锁定 level2 然后才加记数器
//读取锁离开时只有在记数为 0 时才放开锁定
//其实就是读线程如何判断什么时候放开 level2 锁的问题，level1 只是为了使这个判断不会有线程访问问题而已

LockRW::LockRW()
{
	m_bHaveWrite = false;//level2 现在是被写锁锁定了，这个时候读线程必须也要去锁定 level2 锁定以等待写锁
}

LockRW::~LockRW()
{}


//读锁
void LockRW::LockForRead()
{
	//printfd("m_LockLevel1.lock()\r\n");
	m_LockLevel1.lock();
	//printfd("m_LockLevel1.lock() ok\r\n");


	//如果有写锁时也要锁定 level2 锁，目的是等待//也正因为只是等待,所以锁定完后马上放开
	if (m_bHaveWrite == true)
	{
		m_LockLevel1.unlock();
		
		printf("m_LockLevel2.lock()\r\n");
		m_LockLevel2.lock();
		printf("m_LockLevel2.lock() ok\r\n");

		m_LockLevel2.unlock();

		return;
	}

	m_iReadCount++;

	m_LockLevel1.unlock();

}


//void LockRW::UnLockForRead(int * iUnlockLevel2)
void LockRW::UnLockForRead()
{
	//printfd("m_LockLevel1.lock()\r\n");
	m_LockLevel1.lock();
	//printfd("m_LockLevel1.lock() ok\r\n");

	m_iReadCount--;

	m_LockLevel1.unlock();

}

//写锁
void LockRW::LockForWrite()
{
//	return;
	m_LockLevel1.lock();

	m_bHaveWrite = true;//有写请求在锁定队列中 = true;//写者优先的读写锁的设计//阻止 m_iReadCount 再增加

	m_LockLevel1.unlock();

	while(true)
	{
		m_LockLevel2.lock();
		if (m_iReadCount < 1)
		{
			m_LockLevel2.unlock();
			break;
		}

		thread_sleep(1);//有可能会浪费 1 毫秒的时间，不过写的动作是很少的情况下没关系
	}

		
	m_LockLevel2.lock();

}

void LockRW::UnLockForWrite()
{
//	return;

	m_LockLevel1.lock();

	m_bHaveWrite = false;
	
	m_LockLevel2.unlock();

	m_LockLevel1.unlock();

}

