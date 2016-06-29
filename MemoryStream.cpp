#include "StdAfx.h"
//#include "os.h"
//#include <winnt.h>
//#include <Windef.h>
#include "MemoryStream.h"
//#include <stdlib.h>
//#include <math.h>

//--------------------------------------------------
#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

//--------------------------------------------------


MemoryStream::MemoryStream(void)
{
	buf = NULL;
	buf_len = 0;
	buf_len_alloc = 0;
	read_pos = 0;
	//MemoryStream(4096);//���ܵ������
}

MemoryStream::~MemoryStream(void)
{
	if (buf != NULL)
	{
		//memcpy(new_buf, buf, buf_len);
		free(buf);
		buf = NULL;
	}

}

MemoryStream::MemoryStream(int init_buf_len_alloc)
{
	//Ϊ�˸��ٰ�ȫ��ʹ���ַ���,�����ڴ��Ƕ����һ���ֽڴ�� '\0'
	buf = NULL;
	buf_len = 0;
	buf_len_alloc = 0;
	read_pos = 0;

	buf = (char *)malloc(init_buf_len_alloc+1);
	if (buf == NULL) return;

	buf_len_alloc = init_buf_len_alloc;
	memset(buf, 0, buf_len_alloc + 1);
}

//����
void MemoryStream::write(const char * in_buf, int in_buf_len)
{
	check_space(in_buf_len);//�ȼ��ռ��С�Ƿ��㹻
	memcpy_safe(buf + buf_len, in_buf, in_buf_len, buf);
	buf_len += in_buf_len;
}

//����Ƿ�Ҫд����ô������,���еĻ���Ҫ���·���
void MemoryStream::check_space(int in_buf_len)
{
	if (in_buf_len <= 0) return;

	if ((buf_len_alloc - buf_len) > in_buf_len) return;

	int new_len = buf_len + in_buf_len;//������Ҫ��ô��//ȱ�������,������д��Խ��,Ҫ��취����
	//int new_len = in_buf_len;//test

	new_len = max(new_len, buf_len * 2);//��ֹ���ȥ
	//int new_len = __max(in_buf_len, buf_len * 2) + 1;

	char * new_buf = (char *)malloc(new_len+1);//�����ʱ���  malloc ����ʧ��,��ô�����Ƿ���Խ���˶������ڴ���Ĳ��㹻
	memset(new_buf, 0, new_len+1);

	buf_len_alloc = new_len;

	if (buf != NULL)
	{
		memcpy_safe(new_buf, buf, buf_len, new_buf);
		free(buf);
	}

	buf = new_buf;

}

void MemoryStream::clear(void)
{
	//return;
	buf_len = 0;
	buf_len_alloc = 0;
	read_pos = 0;
	if (buf != NULL)
	{
		//memcpy(new_buf, buf, buf_len);
		free(buf);
		buf = NULL;
	}

}