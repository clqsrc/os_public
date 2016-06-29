#ifndef _DEBUG_NEW_H_
#define _DEBUG_NEW_H_

#include <malloc.h>
#include <errno.h>
#include <stdio.h>

#include "os.h"


//��ֹ��Чָ����㷨��������//ע��,ֻ�е���ʱ��ʹ��,����Ӱ������


//һ������,ʹ���۰���ҷ�,�����ɿ��Ʋ�ѯ������¼ʱ�Ķ�λ,����ʹ�� malloc/free �����ڴ�,��Ϊ����Ҫ���� new ������
//��ȻҲҪ�����ֹ�ָ���ڴ淶Χ�������ʹ��
//Ҫʹ�����ڶ��߳���ʹ��,��Ҫ�и�ȫ����

//--------------------------------------------------
//�����
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
//�������
typedef struct debug_new_mem_node//�ڴ���Ϣ�ڵ�
{
	long buf_addr;//ָ��λ��
	long buf_len;//�ڴ����ĳ���


	struct mem_node * next;
} debug_new_mem_node;

extern debug_new_mem_node * debug_new_list;
extern int debug_new_list_len;
extern int debug_new_list_max_len;

void debug_new_init_list();

//���һ���ڴ��λ��
void debug_new_add(void * point, int len);

//����һ���ڴ��λ��
int debug_new_find(void * point, int len);

//ɾ��һ���ڴ��λ��
void debug_new_delete(void * point, int len);

//����һ���ڴ��λ��,û�ҵ��ͱ���//is_free==true ʱΪ����Ƿ��ͷ���,û�о����ڴ�й©��//
//is_free==false ���Ǽ�����ָ���Ƿ�Ϸ�
void debug_new_check(void * point, bool is_free);

//����Ƿ�Ϸ�ָ��
void debug_new_check_point(void * point);

//����ڴ��Ƿ��Ѿ��ͷ���
void debug_new_check_free(void * point);

//������¼��ǰ�к͵�ǰ����//Ҳ��˵�Ǽ�¼ ��ջ
void log_stack(const char *file, int line, const char * function);

//������¼��ǰ�к͵�ǰ����//Ҳ��˵�Ǽ�¼ ��ջ
void log_stack_tofile(const char *file, int line, const char * function, FILE * f);

//--------------------------------------------------
//--------------------------------------------------
//--------------------------------------------------






//--------------------------------------------------
//ȫ�ֵ�
void debug_new_init();

//������VC��_RPT0��//ֻ�ڵ���״̬�´�ӡ//vc�µ�

//��Ȼ��Ҫ�� __FUNCTION__ ����������,��Ϊ�����ֻ���ں��������������
//��˵ VC6 Ҳ�ǲ�֧�� __FUNCTION__ ��//���嵽VC�Ĺ����ļ��к���
#ifndef __FUNCTION__
//	#define __FUNCTION__ "Global"
#endif

#define DEBUG_NEW_HOOK

#ifdef DEBUG_NEW_HOOK
  //������д������Ϣ��ʵ�ʵ��ú���
  #define  debug_new_check_point(a)  log_stack(__FILE__, __LINE__, __FUNCTION__); debug_new_check(a, false)
  #define  debug_new_check_free(a)  log_stack(__FILE__, __LINE__, __FUNCTION__); debug_new_check(a, true)

  //����ǰ״̬��¼����־�ļ���//
  #define  log_cur(a)  log_stack_tofile(__FILE__, __LINE__, __FUNCTION__, a)

  //#define  printfd2(a,b)   printf(a,b)
  //#define  printfd3(a,b,c)   printf(a,b,c)
  //#define  printfd4(a,b,c,d)   printf(a,b,c,d)
#else
 
#endif




#endif
