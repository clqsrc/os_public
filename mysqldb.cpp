
#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include <iostream>
#include <map>
#include <vector>
#include <string>

extern "C" {
#include <mysql.h>
}

#include "file_system.h"
#include "public_function.h"
#include "mysqldb.h"

//ȡ��VC��ʹ��STL�ľ���
#pragma warning(disable:4503)//�ŵ�os.h�в���

//��̬�����ĳ�ʼ��ʱҪ������������
std::string CMysqldb::m_Host = "";
std::string CMysqldb::m_User = "";
std::string CMysqldb::m_Pass = "";
std::string CMysqldb::m_DBName = "";
