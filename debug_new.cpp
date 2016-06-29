
//#include "os.h"
#include <stdio.h>

#include "debug_new.h"


//��ֹ��Чָ����㷨��������//ע��,ֻ�е���ʱ��ʹ��,����Ӱ������


//һ������,ʹ���۰���ҷ�,�����ɿ��Ʋ�ѯ������¼ʱ�Ķ�λ,����ʹ�� malloc/free �����ڴ�,��Ϊ����Ҫ���� new ������
//��ȻҲҪ�����ֹ�ָ���ڴ淶Χ�������ʹ��
//Ҫʹ�����ڶ��߳���ʹ��,��Ҫ�и�ȫ����

//--------------------------------------------------
//�����
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
  //pthread_mutex_trylock���������صģ������Ƿ������ɹ�
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
//�������

debug_new_mem_node * debug_new_list;
int debug_new_list_len;
int debug_new_list_max_len;

void debug_new_init_list()
{
	debug_new_list = NULL;
	debug_new_list_max_len = 100000;//���ֵ//������ 100000 ���� 2M �ڴ�����
	debug_new_list = (debug_new_mem_node *)malloc(debug_new_list_max_len * sizeof(debug_new_mem_node));
	
	debug_new_list_len = 0;
}

//���һ���ڴ��λ��
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

//����һ���ڴ��λ��
int debug_new_find(void * point, int len)
{
	debug_new_lock();

	int index = -1;//ָ���λ��
	int i = 0;

	for (i=0; i<debug_new_list_len; i++)
	{
		if (debug_new_list[i].buf_addr == (long)point)
			index = i;
	}

	debug_new_unlock();
	return index;
}

//ɾ��һ���ڴ��λ��
void debug_new_delete(void * point, int len)
{
	debug_new_lock();

	int index = -1;//ָ���λ��
	int i = 0;

	index = debug_new_find(point, len);


	if (index == -1)//û���ҵ����ָ��
	{
		printf("error! debug_new_delete()\r\n");
		return;
	}

	debug_new_list[debug_new_list_len].buf_addr = 0;
	debug_new_list[debug_new_list_len].buf_len = 0;

	//�ƶ��ڴ��,����ǱȽ���Ҫ��
	while(true)
	{
		debug_new_list[index] = debug_new_list[index+1];

		index++;
		if (index > debug_new_list_len) break;
	}
	
	debug_new_list_len--;

	debug_new_unlock();
}

//����һ���ڴ��λ��,û�ҵ��ͱ���//is_free==true ʱΪ����Ƿ��ͷ���,û�о����ڴ�й©��//
//is_free==false ���Ǽ�����ָ���Ƿ�Ϸ�
void debug_new_check(void * point, bool is_free)
{
	debug_new_lock();

	int index = -1;//ָ���λ��
	int i = 0;

	for (i=0; i<debug_new_list_len; i++)
	{
		//ֻ�Ƚ�ָ���λ���ǲ��Ե�,���п�����һ���ڴ��е�һ��[����һ�������е�һ��]
		if (debug_new_list[i].buf_addr == (long)point)
		{
			index = i;
			break;
		}
		//���ڴ���ڵ�
		if ((debug_new_list[i].buf_addr > (long)point ) && (debug_new_list[i].buf_addr < (long)point + debug_new_list[i].buf_len))
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{//ָ�벻����
		if (is_free == false)
			printf("error! debug_new_check() ָ���ڴ治����!\r\n");
	}
	else
	{//ָ�뻹��
		if (is_free == true)
			printf("error! debug_new_check() δ�ͷ�!\r\n");
	
	}

	debug_new_unlock();
}


//����Ƿ�Ϸ�ָ��
/*
void debug_new_check_point(void * point)
{
	debug_new_check(point, false);
}

//����ڴ��Ƿ��Ѿ��ͷ���
void debug_new_check_free(void * point)
{
	debug_new_check(point, true);
}
*/

//������¼��ǰ�к͵�ǰ����//Ҳ��˵�Ǽ�¼ ��ջ
void log_stack(const char *file, int line, const char * function)
{
	printf("log_stack[��ջ��Ϣ] file:%s, line:%d, function:%s\r\n", file, line, function);
}

//������¼��ǰ�к͵�ǰ����//Ҳ��˵�Ǽ�¼ ��ջ
void log_stack_tofile(const char *file, int line, const char * function, FILE * f)
{
	fprintf(f, "log_stack[��ջ��Ϣ] file:%s, line:%d, function:%s\r\n", file, line, function);
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





