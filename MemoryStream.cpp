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
	//MemoryStream(4096);//不能调用这个
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
	//为了高速安全地使用字符串,分配内存是多分配一个字节存放 '\0'
	buf = NULL;
	buf_len = 0;
	buf_len_alloc = 0;
	read_pos = 0;

	buf = (char *)malloc(init_buf_len_alloc+1);
	if (buf == NULL) return;

	buf_len_alloc = init_buf_len_alloc;
	memset(buf, 0, buf_len_alloc + 1);
}

//保存
void MemoryStream::write(const char * in_buf, int in_buf_len)
{
	check_space(in_buf_len);//先检查空间大小是否足够
	memcpy_safe(buf + buf_len, in_buf, in_buf_len, buf);
	buf_len += in_buf_len;
}

//检查是否还要写入这么多内容,不行的话就要重新分配
void MemoryStream::check_space(int in_buf_len)
{
	if (in_buf_len <= 0) return;

	if ((buf_len_alloc - buf_len) > in_buf_len) return;

	int new_len = buf_len + in_buf_len;//至少需要这么大//缺少了这个,发生过写入越界,要想办法处理
	//int new_len = in_buf_len;//test

	new_len = max(new_len, buf_len * 2);//奇怪过不去
	//int new_len = __max(in_buf_len, buf_len * 2) + 1;

	char * new_buf = (char *)malloc(new_len+1);//如果这时候的  malloc 返回失败,那么可能是访问越界了而不是内存真的不足够
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