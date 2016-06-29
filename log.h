#ifndef _LOG_H__
#define _LOG_H__

//ȡ��VC��ʹ��STL�ľ���
#pragma warning(disable:4503)
#pragma warning(disable:4786)

#include <string>

#include "os.h"
#include "public_function.h"
#include "tmem.h"

//��־������,���Զ��ض�̫������־
class CLog
{
public://����

private:
	std::string m_LogFileName;//��־�ļ�ʹ�õ��ļ���
	std::string m_LogFileNameBak;//��־�ļ�ʹ�õı����ļ���

	int m_iWriteLineCount;//��ǰ�Ѿ�д�������
	FILE * m_LogFile;//�����������ļ�ָ��

	thread_lock m_Lock;

public://����
	CLog::CLog();
	CLog::~CLog();

	//��¼��Ϣ
	void CLog::Log(const std::string s);
	
	//����־�ļ�
	void CLog::OpenLog(const std::string fn);

};


#endif


