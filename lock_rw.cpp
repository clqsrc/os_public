#include "os.h"
#include "thread_lock.h"
#include "lock_rw.h"
#include "public_function.h"
#include <stdio.h>

//ԭ�����ж��߳�1ʱ���� level1 �����������ǵ�һ����ȡ��ʱ��Ҫ�� level2. ����ͷ� level1 ����
//���ж��߳�2ʱ�������ƣ��������� level2 ����ֻ�Ƕ��������� 1
//��ȡ�߳�ȫ���뿪����ͷ� level2 ,��������д����̫�����ˣ�Ӧ���ǵ�����д״̬ʱҲҪ���� level2 Ȼ��żӼ�����
//��ȡ���뿪ʱֻ���ڼ���Ϊ 0 ʱ�ŷſ�����
//��ʵ���Ƕ��߳�����ж�ʲôʱ��ſ� level2 �������⣬level1 ֻ��Ϊ��ʹ����жϲ������̷߳����������

LockRW::LockRW()
{
	m_bHaveWrite = false;//level2 �����Ǳ�д�������ˣ����ʱ����̱߳���ҲҪȥ���� level2 �����Եȴ�д��
}

LockRW::~LockRW()
{}


//����
void LockRW::LockForRead()
{
	//printfd("m_LockLevel1.lock()\r\n");
	m_LockLevel1.lock();
	//printfd("m_LockLevel1.lock() ok\r\n");


	//�����д��ʱҲҪ���� level2 ����Ŀ���ǵȴ�//Ҳ����Ϊֻ�ǵȴ�,��������������Ϸſ�
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

//д��
void LockRW::LockForWrite()
{
//	return;
	m_LockLevel1.lock();

	m_bHaveWrite = true;//��д���������������� = true;//д�����ȵĶ�д�������//��ֹ m_iReadCount ������

	m_LockLevel1.unlock();

	while(true)
	{
		m_LockLevel2.lock();
		if (m_iReadCount < 1)
		{
			m_LockLevel2.unlock();
			break;
		}

		thread_sleep(1);//�п��ܻ��˷� 1 �����ʱ�䣬����д�Ķ����Ǻ��ٵ������û��ϵ
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

