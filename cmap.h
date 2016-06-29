#ifndef _C_MAP_H_
#define _C_MAP_H_

//C语言实现的简单 map 操作集合,用于效率和内存要求高的地方

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//int = int 类型的
struct map_int
{
	int key;//要转换的代码
	int value;//要转换的结果
};


//--------------------------------------------------
//这里有一个技巧,加一个类头,就可以越 C 语言函数的函数重复定义了
class cmap
{
public:
//--------------------------------------------------


//从文件中装载//为要效率会将 buf 中的某些字符直接改成 \0 ,所以其参数得改变为可写的
//void LoadKey(map_int * data, const char * buf, const int buf_len)
static void LoadKey(map_int * data, char * buf, const int buf_len, const int MAX)
{
	if (data == NULL) return;
	
	char c;
	int i = 0;
	int dindex = 0;//map_int 中的索引
	char * s = NULL;
	char * key = NULL;
	char *value = NULL;
	int newline = 0;//是否有新行,用来判断文件未没有 \n 会丢失最后一行数据的情况
	
	s = buf;
	buf[buf_len-1] = '\0';//为了方便C操作函数

	for (i = 0; i < buf_len-1; i++)//最后一个字符不操作
	{
		c = buf[i];
		
		if (c == '=')
		{
			key = s;
			value = &buf[i+1];
			newline = 1;
		}

		if (c == '\n')
		{
			newline = 0;
			buf[i] = '\0';//为了方便C操作函数
			
			if ((key != NULL)&&(value != NULL))
			{
				data[dindex].key = atoi(key);
				data[dindex].value = atoi(value);
				dindex++;
				
				//不能超过最大值,因为 data 有空间是已经决定了的
				if (dindex >= MAX)
				{
					break;
				}
			}
			
			//重置变量
			key = NULL;
			value = NULL;
			
		}
	
		if ((c == ' ')||(c == '=')||(c == '\t')||(c == '\r')||(c == '\n'))
		{
			buf[i] = '\0';//为了方便C操作函数
			
			s = &buf[i+1];//得到了一个串
		}

	}

	//--------------------------------------------------
	//最后一行没有 \n 的情况
	if (newline == 1)
	{
		//不能超过最大值,因为 data 有空间是已经决定了的
		if (dindex >= MAX)
		{
			//break;
		}
		else
		{
			value = s;
			
			if ((key != NULL)&&(value != NULL))
			{
				data[dindex].key = atoi(key);
				data[dindex].value = atoi(value);
				
				dindex++;
			}
			
		}
		
	}

}


//取得一个值//注意返回值是用来表示是否查找成功的
static int GetValue(map_int * data, int key, int * value, const int MAX)
{
	for (int i = 0; i < MAX; i++)
	{
		if (data[i].key == key)
		{
			(* value) = data[i].value;
			return 1;
		}
		
	}
	
	return 0;
}


//--------------------------------------------------
};

#endif
