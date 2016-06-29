#ifndef _C_MAP_H_
#define _C_MAP_H_

//C����ʵ�ֵļ� map ��������,����Ч�ʺ��ڴ�Ҫ��ߵĵط�

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


//int = int ���͵�
struct map_int
{
	int key;//Ҫת���Ĵ���
	int value;//Ҫת���Ľ��
};


//--------------------------------------------------
//������һ������,��һ����ͷ,�Ϳ���Խ C ���Ժ����ĺ����ظ�������
class cmap
{
public:
//--------------------------------------------------


//���ļ���װ��//ΪҪЧ�ʻὫ buf �е�ĳЩ�ַ�ֱ�Ӹĳ� \0 ,����������øı�Ϊ��д��
//void LoadKey(map_int * data, const char * buf, const int buf_len)
static void LoadKey(map_int * data, char * buf, const int buf_len, const int MAX)
{
	if (data == NULL) return;
	
	char c;
	int i = 0;
	int dindex = 0;//map_int �е�����
	char * s = NULL;
	char * key = NULL;
	char *value = NULL;
	int newline = 0;//�Ƿ�������,�����ж��ļ�δû�� \n �ᶪʧ���һ�����ݵ����
	
	s = buf;
	buf[buf_len-1] = '\0';//Ϊ�˷���C��������

	for (i = 0; i < buf_len-1; i++)//���һ���ַ�������
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
			buf[i] = '\0';//Ϊ�˷���C��������
			
			if ((key != NULL)&&(value != NULL))
			{
				data[dindex].key = atoi(key);
				data[dindex].value = atoi(value);
				dindex++;
				
				//���ܳ������ֵ,��Ϊ data �пռ����Ѿ������˵�
				if (dindex >= MAX)
				{
					break;
				}
			}
			
			//���ñ���
			key = NULL;
			value = NULL;
			
		}
	
		if ((c == ' ')||(c == '=')||(c == '\t')||(c == '\r')||(c == '\n'))
		{
			buf[i] = '\0';//Ϊ�˷���C��������
			
			s = &buf[i+1];//�õ���һ����
		}

	}

	//--------------------------------------------------
	//���һ��û�� \n �����
	if (newline == 1)
	{
		//���ܳ������ֵ,��Ϊ data �пռ����Ѿ������˵�
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


//ȡ��һ��ֵ//ע�ⷵ��ֵ��������ʾ�Ƿ���ҳɹ���
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
