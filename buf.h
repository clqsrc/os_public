#ifndef _C_BUF_H_
#define _C_BUF_H_


#include "os.h"
#include <malloc.h>

//����ͨ��Э���еĹ̶����Ƚṹ

//�������͵Ķ���

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned int
#define u64 unsigned double long

#define int8 signed char
#define int16 signed short
#define int32 signed int
#define int64 signed double long


class CBuf
{
public:
	char * m_pBuf;
	int m_iBufLen;//�������ڴ�������

private:
	int m_iMemLen;//�Ѿ�������ڴ��С//m_iMemLen �п������Ѿ����޸Ĺ��˵�,����Ҫ��֤ m_iMemLen �����ڷ����ڴ��Ÿ�ֵ 
	int m_iStepMemLen;//ÿ���ڴ治��ʱ�����ӵ��ڴ��С
	//char * m_pCur;//��д����ʱ�ĵ�ǰλ��ָ��
	int m_iCur;//��д����ʱ�ĵ�ǰλ��
	
public:
	CBuf()
	{
		m_pBuf = NULL;
		m_iMemLen = 0;
		m_iStepMemLen = 256;//256 Ĭ��ÿ������ 256 ���ֽ�
		m_iBufLen = 0;
		m_iCur = 0;
	}

	CBuf(int len)
	{
		m_pBuf = NULL;
		m_iMemLen = 0;
		m_iStepMemLen = len;
		m_iBufLen = 0;
		m_iCur = 0;

		NewMem(len);
	}

	~CBuf()
	{
		FreeMem();
	}

	void FreeMem()
	{
		if (m_pBuf != NULL)
		{
			//�ͷ�ǰ�ÿ�,�����������ڴ����Խ��ʱ����//������ vc6 ���ƺ����Զ�����Ϊ EE FE 
			//���� free �������ú����õ� 
			memset(m_pBuf, 0, m_iMemLen);
			free(m_pBuf);
			m_pBuf = NULL;
		}
		m_iMemLen = 0;
	}

	char * NewMem(int len)
	{
		//���ͷ�
		FreeMem();

		m_pBuf = (char *)malloc(len);
		memset(m_pBuf, 0, len);

		if (m_pBuf == NULL) 
		{//�ڴ治��
			return NULL;
		}

		m_iMemLen = len;

		return m_pBuf;
	}

	//�����ڴ�����ԭʼ���ݲ���
	char * ReSize(int len)
	{
		if (len == 0)
		{
			return NULL;
		}

		char * tmp = (char *)malloc(len);
		memset(tmp, 0, len);

		if (tmp == NULL)
		{//�ڴ治��
			return NULL;
		}

		//�о����ݵĻ�Ҫ�ȸ��ƾ�����
		if (m_pBuf != NULL)
		{
			memcpy(tmp, m_pBuf, m_iBufLen);
		}

		//���ͷ�
		//����������漰̫�ණ��//FreeMem();
		if (m_pBuf != NULL)
		{
			//�ͷ�ǰ�ÿ�,�����������ڴ����Խ��ʱ����//������ vc6 ���ƺ����Զ�����Ϊ EE FE 
			//���� free �������ú����õ� 
			memset(m_pBuf, 0, m_iMemLen);//m_iMemLen �п������Ѿ����޸Ĺ��˵�,����Ҫ��֤ m_iMemLen �����ڷ����ڴ��Ÿ�ֵ 
			free(m_pBuf);
			m_pBuf = NULL;
		}


		//�ٸ�ֵ
		m_pBuf = tmp;
		m_iMemLen = len;

		return m_pBuf;
	}

	//д��һ���ڴ�
	void WriteBuf(const void * buf, const int len)
	{
		if (len == 0)
			return;

		//�ڴ治��Ļ�Ҫ�ٷ���
		if ((m_iBufLen + len) > m_iMemLen)
		{
			int newlen = 0;//��һ����ʱ��������Ҫֱ���� m_iStepMemLen����Ϊ m_iStepMemLen �ں���Ҫ�õ�������������͸ı�����ֵ
			
			newlen = m_iBufLen + len;
			if (newlen < m_iStepMemLen) newlen = this->m_iStepMemLen;
			//ÿ������һ��
			ReSize(newlen * 2);
		}
	
		memcpy(m_pBuf + m_iBufLen, (const char *)buf, len);
		m_iBufLen += len;

	}

	//д��8�ֽ�
	void Write8Bit(const void * buf)
	{
		WriteBuf((const char *)buf, 8);
	}

	//д��16�ֽ�
	void Write16Bit(const void * buf)
	{
		WriteBuf((const char *)buf, 16);
	}

	//д��32�ֽ�
	void Write32Bit(const void * buf)
	{
		WriteBuf((const char *)buf, 32);
	}

	//�ƶ���ָ��
	void Seek(const int pos)
	{
		m_iCur = pos;
	}

	//��ȡһ���ڴ�
	void ReadBuf(void * buf, const int len)
	{
		memcpy(buf, m_pBuf + m_iCur, len);
	    m_iCur = m_iCur + len;

	}

	//��ȡһ���ڴ�
	u8 ReadU8()
	{
		u8 r = 0;
		ReadBuf(&r, sizeof(r));

		return r;
	}

	//��ȡһ���ڴ�
	u16 ReadU16()
	{
		u16 r = 0;
		ReadBuf(&r, sizeof(r));

		return r;
	}

	//��ȡһ���ڴ�
	u32 ReadU32()
	{
		u32 r = 0;
		ReadBuf(&r, sizeof(r));

		return r;
	}


};

#endif
