
#include "log.h"
#include "os.h"
#include "public_function.h"
#include "tmem.h"
#include "file_system.h"


//��־������,���Զ��ض�̫������־

CLog::CLog()
{
	m_LogFileName = "";//��־�ļ�ʹ�õ��ļ���
	m_LogFileNameBak = "";//��־�ļ�ʹ�õı����ļ���

	m_iWriteLineCount = 0;
	m_LogFile = NULL;

}

CLog::~CLog()
{

}


//��¼��Ϣ
void CLog::Log(const std::string s)
{
	//return;//test
	m_Lock.lock();

	//����ļ�д��̫����
	if (m_iWriteLineCount > 10000)
	{
		m_iWriteLineCount = 0;
		fclose(m_LogFile);
		m_LogFile = NULL;

		//�����ļ�
		file_system::copy_file(m_LogFileName, m_LogFileNameBak);
		//ɾ�����ļ�
		file_system::delete_file(m_LogFileName);
	
	}


	//���ļ�
	if (m_LogFile == NULL)
	{
		//m_LogFile = fopen(m_LogFileName.c_str(), "a+b");//׷��ģʽ
		m_LogFile = open_log_file(m_LogFileName);
	}

	if (m_LogFile == NULL)
	{
		printf("CLog::Log() ��־�ļ�:%��ʧ��.\r\n", m_LogFileName.c_str());
		m_Lock.unlock();
		return;
	}

	fprintf(m_LogFile, "%s", s.c_str());

	//д��ʱ��
	tm m;
	memset(&m, 0, sizeof(tm));
	time_t t = time(NULL);
	tm * pm = localtime(&t);

	if (pm != NULL)
	{
		m = * pm;
	}

	fprintf(m_LogFile, "%d.%d.%d %d:%d\r\n", m.tm_year + 1900, m.tm_mon + 1, m.tm_mday, m.tm_hour, m.tm_min);

	m_iWriteLineCount++;

	m_Lock.unlock();
}


//����־�ļ�
void CLog::OpenLog(const std::string fn)
{
	m_LogFileName = fn;//��־�ļ�ʹ�õ��ļ���
	m_LogFileNameBak = fn + ".bak";//��־�ļ�ʹ�õı����ļ���

	
}



	
	
	