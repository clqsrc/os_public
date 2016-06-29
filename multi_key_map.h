#ifndef _MULTI_KEY_MAP_H_
#define _MULTI_KEY_MAP_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include "base_socket.h"
#include <iostream>

//�� map ������һ����������map

//ÿ�����������ַ�Ϊ������?
//std::multimap<std::string, ftp_client_socket *>

//ģ�����������Ҫ���ɵ���
template<class T> class multi_key_map
{
typedef std::multimap<int, T *> int_keys;//����Ϊ�����ļ�ֵ��
typedef std::multimap<std::string, T *> string_keys;//�ַ�Ϊ�����ļ�ֵ��
	
public://����
	//��� map �������������ֵ�б�
	std::map<std::string, int_keys *>		int_keys_list;
	std::map<std::string, string_keys *>	string_keys_list;

public://����

	//����һ����ֵ��
	void add_int_keys(std::string key_name)
	{

	}
	void add_string_keys(std::string key_name)
	{

	
	}

	//std::map< std::string, keys * >::iterator iter;
	//int_keys

};

#endif
