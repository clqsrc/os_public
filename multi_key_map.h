#ifndef _MULTI_KEY_MAP_H_
#define _MULTI_KEY_MAP_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include "base_socket.h"
#include <iostream>

//用 map 来构建一个多主键的map

//每个主键都是字符为索引的?
//std::multimap<std::string, ftp_client_socket *>

//模板参数是容器要容纳的类
template<class T> class multi_key_map
{
typedef std::multimap<int, T *> int_keys;//整数为索引的键值对
typedef std::multimap<std::string, T *> string_keys;//字符为索引的键值对
	
public://变量
	//这个 map 用来保存各个键值列表
	std::map<std::string, int_keys *>		int_keys_list;
	std::map<std::string, string_keys *>	string_keys_list;

public://函数

	//增加一个键值对
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
