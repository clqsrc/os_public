
#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>

#include <libpq-fe.h>//pgsql

#include "file_system.h"
#include "public_function.h"
#include "pgsqldb.h"

//ȡ��VC��ʹ��STL�ľ���
#pragma warning(disable:4503)//�ŵ�os.h�в���

//��̬�����ĳ�ʼ��ʱҪ������������
std::string CPgsqldb::m_Host = "";
std::string CPgsqldb::m_User = "";
std::string CPgsqldb::m_Pass = "";
std::string CPgsqldb::m_DBName = "";
