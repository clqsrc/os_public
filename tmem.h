#ifndef _T_MEM_H_
#define _T_MEM_H_


#include "os.h"

//代替函数堆内存的类，因为堆的大小的限制//临时内存的操作类。注意只能在一个子函数中使用//好处是能在超过作用域时自动释放

class tmem
{
public:
	char * buf;
	int buf_len;
	
public:
	tmem()
	{
		buf = NULL;
		buf_len = 0;
	}

	tmem(int len)
	{
		buf = NULL;
		buf_len = 0;

		new_mem(len);
	}

	~tmem()
	{
		free_mem();
	}

	void free_mem()
	{
		if (buf != NULL)
		{
			//释放前置空,这样可以在内存访问越界时报错//不过在 vc6 下似乎会自动设置为 EE FE 
			//是在 free 函数调用后设置的 
			memset(buf, 0, buf_len);
			free(buf);
		}
		buf_len = 0;
	}

	char * new_mem(int len)
	{
		//先释放
		free_mem();

		buf = (char *)malloc(len);
		memset(buf, 0, len);

		if (buf == NULL) 
		{//内存不足
			return NULL;
		}

		buf_len = len;

		return buf;
	}

private:
	bool is_locked;


};

#endif
