#ifndef _LOCK_RW_H_
#define _LOCK_RW_H_

//��д������ʵ��//���ںܶ�����ֻҪ��ʼ����ʱ����Ҫ����������ʱ����ֻҪ��ȡ�ģ������б�Ҫʵ��һ��//��д�ģ���������

#include "os.h"
#include "thread_lock.h"

class LockRW
{
public:
	LockRW();
	~LockRW();

private:
	//��������������ʵ��
	thread_lock m_LockLevel1;//���ֻ��Ϊ�˱������������Ķ�дͬ��
	thread_lock m_LockLevel2;
	
	int m_iReadCount;
	bool m_bHaveWrite;//��д����������������
	
public:
	//����
	void LockForRead();
	void UnLockForRead();
	//д��
	void LockForWrite();
	void UnLockForWrite();

};

#endif
