#pragma once

#include <string>

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

//类似于 C# , delphi 的同名类//内存分配模式为不足时成倍增长
class MemoryStream
{
public:
	char * buf;
	int buf_len;//为了高速安全地使用字符串,分配内存是多分配一个字节存放 '\0'
	int read_pos;//当前从流中读取了多少个字节

	//保存
	void MemoryStream::write(const char * in_buf, int in_buf_len);

	//清空
	void MemoryStream::clear(void);

	//读取一部分到另外一个流中
	int MemoryStream::read(MemoryStream & in_buf, int in_buf_len)
	{
		//int r = 0;

		int read_len = CanRead();
		read_len = min(read_len, in_buf_len);
		in_buf.write(buf + read_pos, read_len);

		read_pos += read_len;

		return read_len;
	}

	//读取一部分到缓冲中//不移动读取位置
	int MemoryStream::read_no_seek(char * in_buf, int in_buf_len)
	{
		//int r = 0;

		int read_len = CanRead();
		read_len = min(read_len, in_buf_len);

		memcpy(in_buf, buf + read_pos, read_len);

		//read_pos += read_len;

		return read_len;
	}

	//读取一部分到缓冲中
	int MemoryStream::read(char * in_buf, int in_buf_len)
	{
		//int r = 0;

		int read_len = read_no_seek(in_buf, in_buf_len);

		read_pos += read_len;

		return read_len;
	}



	//还有多少字节可读取
	int MemoryStream::CanRead()
	{
		return this->buf_len - read_pos;
	}

	//读取位置向后移动
	void seek_for_read(int len)
	{
		read_pos += len;
	}

private:
	int buf_len_alloc;//buf_len 是已经使用的大小,这个是预先分配的大小

	//检查是否还要写入这么多内容,不行的话就要重新分配
	void MemoryStream::check_space(int in_buf_len);

public:
	MemoryStream(void);

	~MemoryStream(void);

	//指定初始化大小的
	MemoryStream(int init_buf_len_alloc);

public://内存安全函数//前 n 个字节用于保存长度等信息

	void * malloc(size_t size)
	{
		int add_len = sizeof(int) * 2;
		int tag = 1976;//标志
		int len = size;

		char * r = (char *)::malloc(size + add_len);

		char * p = r;
		::memcpy(p, &tag, sizeof(int));
		p += sizeof(int);

		::memcpy(p, &len, sizeof(int));


		return r + add_len;
	}

	void free(void * in_buf)
	{
		int add_len = sizeof(int) * 2;

		char * p = (char *) in_buf;
		p -= add_len;//这才是真正的内存块起点

		//int tag = 1976;//标志
		//int len = size;
		int tag = 0;
		int len = 0;

		::memcpy(&tag, p, sizeof(int));
		p += sizeof(int);

		::memcpy(&len, p, sizeof(int));


		//--------------------------------------------------
		
		if (tag == 1976)//标志对得上才是你分配的内存
		{
			p = (char *) in_buf;
			p -= add_len;//这才是真正的内存块起点

			::free(p);
		}
		else
		{
			merror("分配的内存无效或者不是您分配的.");
		}
	}

	//仅用于调试
	void merror(const char * msg)
	{
		printf("merror:%s", msg);
	}

	//安全版的 memcpy //只效验写入区是否是有效果的
	void memcpy_safe(void *dest, const void *src, size_t count,
		void * buf_start_addr//附加传递一个内存块的起点位置// malloc() 调用后得到的内存地址 
		)
	{
		if (buf == NULL)
		{
			merror("目标内存块还未分配");
		}

		int add_len = sizeof(int) * 2;

		char * p = (char *) buf;
		p -= add_len;//这才是真正的内存块起点

		//int tag = 1976;//标志
		//int len = size;
		int tag = 0;
		int len = 0;

		::memcpy(&tag, p, sizeof(int));
		p += sizeof(int);

		::memcpy(&len, p, sizeof(int));


		//--------------------------------------------------
		char * in_buf = (char *)buf_start_addr;
		
		if (tag == 1976)//标志对得上才是你分配的内存
		{
			p = (char *) in_buf;
			p -= add_len;//这才是真正的内存块起点

			//--------------------------------------------------
			//判断起始位置是否在有效果范围内
			char * dest_p = (char *)dest; 
			if (dest_p < in_buf)//有效区前
			{
				merror("目标内存块起始位置越界");
			}
			else
			if (dest_p > (in_buf + len))//有效区后
			{
				merror("目标内存块起始位置越界");
			}
			else//写入块结束位置在有效区后//通常是内存区不够大
			if ((dest_p + count) > (in_buf + len))
			{
				merror("目标内存块结束位置越界");
			}


			//else
			{
				::memcpy(dest, src, count);				
			}
		}
		else
		{
			merror("分配的内存无效或者不是您分配的.");
		}
	
	}

};
