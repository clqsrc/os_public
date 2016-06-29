#ifndef _C_BUF_H_
#define _C_BUF_H_


#include "os.h"
#include <malloc.h>

//代替通信协议中的固定长度结构

//数据类型的定义

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
	int m_iBufLen;//操作的内存区长度

private:
	int m_iMemLen;//已经分配的内存大小//m_iMemLen 有可能是已经被修改过了的,所以要保证 m_iMemLen 必须在分配内存后才赋值 
	int m_iStepMemLen;//每次内存不足时新增加的内存大小
	//char * m_pCur;//读写内容时的当前位置指针
	int m_iCur;//读写内容时的当前位置
	
public:
	CBuf()
	{
		m_pBuf = NULL;
		m_iMemLen = 0;
		m_iStepMemLen = 256;//256 默认每次增加 256 个字节
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
			//释放前置空,这样可以在内存访问越界时报错//不过在 vc6 下似乎会自动设置为 EE FE 
			//是在 free 函数调用后设置的 
			memset(m_pBuf, 0, m_iMemLen);
			free(m_pBuf);
			m_pBuf = NULL;
		}
		m_iMemLen = 0;
	}

	char * NewMem(int len)
	{
		//先释放
		FreeMem();

		m_pBuf = (char *)malloc(len);
		memset(m_pBuf, 0, len);

		if (m_pBuf == NULL) 
		{//内存不足
			return NULL;
		}

		m_iMemLen = len;

		return m_pBuf;
	}

	//扩大内存区，原始内容不变
	char * ReSize(int len)
	{
		if (len == 0)
		{
			return NULL;
		}

		char * tmp = (char *)malloc(len);
		memset(tmp, 0, len);

		if (tmp == NULL)
		{//内存不足
			return NULL;
		}

		//有旧内容的话要先复制旧内容
		if (m_pBuf != NULL)
		{
			memcpy(tmp, m_pBuf, m_iBufLen);
		}

		//先释放
		//不用这个，涉及太多东西//FreeMem();
		if (m_pBuf != NULL)
		{
			//释放前置空,这样可以在内存访问越界时报错//不过在 vc6 下似乎会自动设置为 EE FE 
			//是在 free 函数调用后设置的 
			memset(m_pBuf, 0, m_iMemLen);//m_iMemLen 有可能是已经被修改过了的,所以要保证 m_iMemLen 必须在分配内存后才赋值 
			free(m_pBuf);
			m_pBuf = NULL;
		}


		//再赋值
		m_pBuf = tmp;
		m_iMemLen = len;

		return m_pBuf;
	}

	//写入一块内存
	void WriteBuf(const void * buf, const int len)
	{
		if (len == 0)
			return;

		//内存不足的话要再分配
		if ((m_iBufLen + len) > m_iMemLen)
		{
			int newlen = 0;//用一个临时变量，不要直接用 m_iStepMemLen，因为 m_iStepMemLen 在后面要用到，不能在这里就改变它的值
			
			newlen = m_iBufLen + len;
			if (newlen < m_iStepMemLen) newlen = this->m_iStepMemLen;
			//每次增加一倍
			ReSize(newlen * 2);
		}
	
		memcpy(m_pBuf + m_iBufLen, (const char *)buf, len);
		m_iBufLen += len;

	}

	//写入8字节
	void Write8Bit(const void * buf)
	{
		WriteBuf((const char *)buf, 8);
	}

	//写入16字节
	void Write16Bit(const void * buf)
	{
		WriteBuf((const char *)buf, 16);
	}

	//写入32字节
	void Write32Bit(const void * buf)
	{
		WriteBuf((const char *)buf, 32);
	}

	//移动读指针
	void Seek(const int pos)
	{
		m_iCur = pos;
	}

	//读取一块内存
	void ReadBuf(void * buf, const int len)
	{
		memcpy(buf, m_pBuf + m_iCur, len);
	    m_iCur = m_iCur + len;

	}

	//读取一块内存
	u8 ReadU8()
	{
		u8 r = 0;
		ReadBuf(&r, sizeof(r));

		return r;
	}

	//读取一块内存
	u16 ReadU16()
	{
		u16 r = 0;
		ReadBuf(&r, sizeof(r));

		return r;
	}

	//读取一块内存
	u32 ReadU32()
	{
		u32 r = 0;
		ReadBuf(&r, sizeof(r));

		return r;
	}


};

#endif
