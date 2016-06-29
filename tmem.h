#ifndef _T_MEM_H_
#define _T_MEM_H_


#include "os.h"

//���溯�����ڴ���࣬��Ϊ�ѵĴ�С������//��ʱ�ڴ�Ĳ����ࡣע��ֻ����һ���Ӻ�����ʹ��//�ô������ڳ���������ʱ�Զ��ͷ�

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
			//�ͷ�ǰ�ÿ�,�����������ڴ����Խ��ʱ����//������ vc6 ���ƺ����Զ�����Ϊ EE FE 
			//���� free �������ú����õ� 
			memset(buf, 0, buf_len);
			free(buf);
		}
		buf_len = 0;
	}

	char * new_mem(int len)
	{
		//���ͷ�
		free_mem();

		buf = (char *)malloc(len);
		memset(buf, 0, len);

		if (buf == NULL) 
		{//�ڴ治��
			return NULL;
		}

		buf_len = len;

		return buf;
	}

private:
	bool is_locked;


};

#endif
