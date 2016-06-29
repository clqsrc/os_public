
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

//取消VC中使用STL的警告
#pragma warning(disable:4503)//放到os.h中才行

//静态变量的初始化时要声明变量类型
std::string CMysqldb::m_Host = "";
std::string CMysqldb::m_User = "";
std::string CMysqldb::m_Pass = "";
std::string CMysqldb::m_DBName = "";
