
//#include "os.h"
#include <stdio.h>

#include "debug_new.h"


//防止无效指针的算法都在这里//注意,只有调试时才使用,否则影响性能


//一个链表,使用折半查找法,这样可控制查询不到记录时的定位,另外使用 malloc/free 分配内存,因为我们要重载 new 操作符
//当然也要能在手工指定内存范围的情况下使用
//要使其能在多线程下使用,还要有个全局锁

//--------------------------------------------------
//锁相关
#ifdef WIN32
  HANDLE debug_new_lock_id;
#else
  pthread_mutex_t debug_new_lock_id;
#endif

void debug_new_init_lock()
{
#ifdef WIN32
  debug_new_lock_id = ::CreateMutex(NULL, false, NULL);
#else
  pthread_mutex_init(&debug_new_lock_id, NULL);
#endif
}

void debug_new_lock()
{
#ifdef WIN32
  ::WaitForSingleObject(debug_new_lock_id, INFINITE);
#else
  //pthread_mutex_trylock是立即返回的，不论是否锁定成功
  pthread_mutex_lock(&debug_new_lock_id);
#endif

}

void debug_new_unlock()
{
#ifdef WIN32
  ::ReleaseMutex(debug_new_lock_id);
#else
  pthread_mutex_unlock(&debug_new_lock_id);
#endif
}

//--------------------------------------------------
//--------------------------------------------------
//链表相关

debug_new_mem_node * debug_new_list;
int debug_new_list_len;
int debug_new_list_max_len;

void debug_new_init_list()
{
	debug_new_list = NULL;
	debug_new_list_max_len = 100000;//最大值//基本上 100000 就是 2M 内存左右
	debug_new_list = (debug_new_mem_node *)malloc(debug_new_list_max_len * sizeof(debug_new_mem_node));
	
	debug_new_list_len = 0;
}

//添加一个内存块位置
void debug_new_add(void * point, int len)
{
	if (debug_new_list_len >= debug_new_list_max_len-1)
	{
		printf("error! debug_new_add()\r\n");
		return;
	}

	debug_new_list[debug_new_list_len].buf_addr = (long)point;
	debug_new_list[debug_new_list_len].buf_len = len;
	
	debug_new_list_len++;
}

//查找一个内存块位置
int debug_new_find(void * point, int len)
{
	debug_new_lock();

	int index = -1;//指针的位置
	int i = 0;

	for (i=0; i<debug_new_list_len; i++)
	{
		if (debug_new_list[i].buf_addr == (long)point)
			index = i;
	}

	debug_new_unlock();
	return index;
}

//删除一个内存块位置
void debug_new_delete(void * point, int len)
{
	debug_new_lock();

	int index = -1;//指针的位置
	int i = 0;

	index = debug_new_find(point, len);


	if (index == -1)//没有找到这个指针
	{
		printf("error! debug_new_delete()\r\n");
		return;
	}

	debug_new_list[debug_new_list_len].buf_addr = 0;
	debug_new_list[debug_new_list_len].buf_len = 0;

	//移动内存块,这个是比较重要的
	while(true)
	{
		debug_new_list[index] = debug_new_list[index+1];

		index++;
		if (index > debug_new_list_len) break;
	}
	
	debug_new_list_len--;

	debug_new_unlock();
}

//查找一个内存块位置,没找到就报警//is_free==true 时为检查是否释放了,没有就是内存泄漏了//
//is_free==false 就是检查这个指针是否合法
void debug_new_check(void * point, bool is_free)
{
	debug_new_lock();

	int index = -1;//指针的位置
	int i = 0;

	for (i=0; i<debug_new_list_len; i++)
	{
		//只比较指针的位置是不对的,它有可能是一块内存中的一个[比如一个数组中的一个]
		if (debug_new_list[i].buf_addr == (long)point)
		{
			index = i;
			break;
		}
		//在内存块内的
		if ((debug_new_list[i].buf_addr > (long)point ) && (debug_new_list[i].buf_addr < (long)point + debug_new_list[i].buf_len))
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{//指针不在了
		if (is_free == false)
			printf("error! debug_new_check() 指针内存不存在!\r\n");
	}
	else
	{//指针还在
		if (is_free == true)
			printf("error! debug_new_check() 未释放!\r\n");
	
	}

	debug_new_unlock();
}


//检查是否合法指针
/*
void debug_new_check_point(void * point)
{
	debug_new_check(point, false);
}

//检查内存是否已经释放了
void debug_new_check_free(void * point)
{
	debug_new_check(point, true);
}
*/

//用来记录当前行和当前函数//也可说是记录 堆栈
void log_stack(const char *file, int line, const char * function)
{
	printf("log_stack[堆栈信息] file:%s, line:%d, function:%s\r\n", file, line, function);
}

//用来记录当前行和当前函数//也可说是记录 堆栈
void log_stack_tofile(const char *file, int line, const char * function, FILE * f)
{
	fprintf(f, "log_stack[堆栈信息] file:%s, line:%d, function:%s\r\n", file, line, function);
	//fflush(f);
}



//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------






//--------------------------------------------------
void debug_new_init()
{
	debug_new_init_lock();
	debug_new_init_list();
}





