#ifndef _DEBUG_NEW_H_
#define _DEBUG_NEW_H_

#include <malloc.h>
#include <errno.h>
#include <stdio.h>

#include "os.h"


//防止无效指针的算法都在这里//注意,只有调试时才使用,否则影响性能


//一个链表,使用折半查找法,这样可控制查询不到记录时的定位,另外使用 malloc/free 分配内存,因为我们要重载 new 操作符
//当然也要能在手工指定内存范围的情况下使用
//要使其能在多线程下使用,还要有个全局锁

//--------------------------------------------------
//锁相关
#ifdef WIN32
  extern HANDLE debug_new_lock_id;
#else
  extern pthread_mutex_t debug_new_lock_id;
#endif

void debug_new_init_lock();

void debug_new_lock();

void debug_new_unlock();

//--------------------------------------------------
//--------------------------------------------------
//链表相关
typedef struct debug_new_mem_node//内存信息节点
{
	long buf_addr;//指针位置
	long buf_len;//内存区的长度


	struct mem_node * next;
} debug_new_mem_node;

extern debug_new_mem_node * debug_new_list;
extern int debug_new_list_len;
extern int debug_new_list_max_len;

void debug_new_init_list();

//添加一个内存块位置
void debug_new_add(void * point, int len);

//查找一个内存块位置
int debug_new_find(void * point, int len);

//删除一个内存块位置
void debug_new_delete(void * point, int len);

//查找一个内存块位置,没找到就报警//is_free==true 时为检查是否释放了,没有就是内存泄漏了//
//is_free==false 就是检查这个指针是否合法
void debug_new_check(void * point, bool is_free);

//检查是否合法指针
void debug_new_check_point(void * point);

//检查内存是否已经释放了
void debug_new_check_free(void * point);

//用来记录当前行和当前函数//也可说是记录 堆栈
void log_stack(const char *file, int line, const char * function);

//用来记录当前行和当前函数//也可说是记录 堆栈
void log_stack_tofile(const char *file, int line, const char * function, FILE * f);

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------






//--------------------------------------------------
//全局的
void debug_new_init();

//类似于VC的_RPT0宏//只在调试状态下打印//vc下的

//当然还要对 __FUNCTION__ 宏作点修饰,因为这个宏只是在函数里面才起作用
//据说 VC6 也是不支持 __FUNCTION__ 的//定义到VC的工程文件中好了
#ifndef __FUNCTION__
//	#define __FUNCTION__ "Global"
#endif

#define DEBUG_NEW_HOOK

#ifdef DEBUG_NEW_HOOK
  //就是先写跟踪信息再实际调用函数
  #define  debug_new_check_point(a)  log_stack(__FILE__, __LINE__, __FUNCTION__); debug_new_check(a, false)
  #define  debug_new_check_free(a)  log_stack(__FILE__, __LINE__, __FUNCTION__); debug_new_check(a, true)

  //将当前状态记录到日志文件中//
  #define  log_cur(a)  log_stack_tofile(__FILE__, __LINE__, __FUNCTION__, a)

  //#define  printfd2(a,b)   printf(a,b)
  //#define  printfd3(a,b,c)   printf(a,b,c)
  //#define  printfd4(a,b,c,d)   printf(a,b,c,d)
#else
 
#endif




#endif
