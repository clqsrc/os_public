#ifndef _CMYSQL_DB_H_
#define _CMYSQL_DB_H_

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

//取消VC中使用STL的警告
#pragma warning(disable:4503)//放到os.h中才行

//高速返回集
typedef std::vector<std::string> MysqlLine;//一行记录

//高速返回集//注意,只适合字段中内容为 255 字节以下的表格
class CMysqlFastResult
{
public:
	std::map<std::string, int> fields;//字段
	std::vector<std::string> field_names;//字段
	std::vector<MysqlLine> rows;//内容
	int curRow;//当前行号

public:

	CMysqlFastResult()
	{
		curRow = 0;
	}

	~CMysqlFastResult()
	{
		Clear();
	}


	//清理内存等资源
	void Clear()
	{
		
		for(int i = 0; i < rows.size(); i++)
		{
			rows[i].clear();
		}
		rows.clear();
	}

	std::string GetValue(const std::string key)
	{
		int field = fields[key];

		//printf("%d,  %d\r\n", rows.size(), rows[curRow].size());

		return rows[curRow][field];
	}

	//向下一行
	void Next()
	{
		curRow++;
	}

	int GetRowCount()
	{
		return rows.size();
	}

};


//返回集
class CMysqlResult
{
public:
	std::vector<keys *> data;//结果集

public:

	CMysqlResult()
	{
	
	}

	~CMysqlResult()
	{
		Clear();
	}


	//清理内存等资源
	void Clear()
	{
		
		for(int i = 0; i < data.size(); i++)
		{
			keys * k = data[i];

			delete k;
			k = NULL;
			data[i] = NULL;
		}
		data.clear();
	}


};

//访问 mysql 的类


class CMysqldb
{
public://连接用的变量
	static std::string m_Host;//地址
	static std::string m_User;//用户名
	static std::string m_Pass;//口令
	static std::string m_DBName;//数据库名


private:
	std::map< std::string, keys * > data;//保存内容//必须有唯一主键
	thread_lock lock;//线程锁

private://长连接所用的变量，要特别注意长连接与短连接的区分，长连接的函数前加前缀 L
	//特别注意长连接不是线程安全的
	MYSQL lmysql;
	unsigned long LLastId;
	bool lconnected;//是否断开的标志
	int LRecordCount;//记录个数
	int LFieldCount;//字段个数
	
public:
	CMysqlFastResult LResult;//新的结果集

public:
	CMysqldb()
	{
		mysql_init(&lmysql);
		//mysql_options(&lmysql, MYSQL_OPT_RECONNECT, "1");//test 据说 5.0.13以后的API版本 可以用这个标志
		lconnected = false;
	}

	~CMysqldb()
	{
		LClose();

		Clear();
	}

	//清理内存等资源
	void Clear()
	{
		ClearResult();
	}


	//清理返回值
	void ClearResult()
	{
		std::map< std::string, keys * >::iterator	iter = data.begin();
		
		for(; iter != data.end(); iter++)
		{
			//uiID = m_iter->first;
			//pstCodeIndex = iter->second;

			delete iter->second;
			iter->second = NULL;
		}
		data.clear();

		LRecordCount = 0;
		LFieldCount = 0;
	}



	//mysql的二进制表示sql
	std::string GetSqlString(std::string s_from)
	{
		std::string s;
		char * buf = (char *)malloc(s_from.length() * 2 +1);//长度要求请见mysql手册
		mysql_escape_string(buf, s_from.c_str(), s_from.length()); 

		s = buf;
		free(buf);

		return s;
	}

	//执行一个SQL语句
	bool ExecuteMysqlSql(std::string sql)
	{
		return ExecuteMysqlSql(sql, NULL);
	}


	//////////////////////////////////////////////////////////////////////////////////////
	//长连接的函数，要特别注意

	//连接 mysql 服务器
	bool LConnect()
	{
		bool r = true;

		//if (!mysql_real_connect(&lmysql, "127.0.0.1", "root", "root", "ggtong", 0, NULL, 0))
		if (!mysql_real_connect(&lmysql, m_Host.c_str(), m_User.c_str(), m_Pass.c_str(), m_DBName.c_str(), 0, NULL, 0))
		{
			r = false;
			lconnected = false;
			printf("connect error:%s\n", mysql_error(&lmysql));
		}
		else
		{
			lconnected = true;
		}

		return r;
	}

	//执行一个SQL语句//怎么知道 mysql 服务器已经断开连接呢
	//unsigned int mysql_errno(MYSQL *mysql) 可取得错误代码列表在 http://dev.mysql.com/doc/refman/5.0/en/error-handling.html
	//感觉 2000 到 3000 之内的是与服务器的连接有问题
	bool LExecuteSql(const std::string sql)
	{
		//printfd2("%s\r\n", sql.c_str());

		bool r = true;

		//字符集不是 latin1 时要设置字符集
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )//不行
		if ( mysql_query(&lmysql, "SET NAMES 'gbk'" ) !=0 )
		{
			printf("mysql_query error:%s\n", mysql_error(&lmysql));

			unsigned int eo = mysql_errno(&lmysql);

			if ((eo < 3000)&&(eo >= 2000))//这种情况认为 mysql 服务器断开
			{
				LConnect();
				//重新连接一次
				if ( mysql_query(&lmysql, "SET NAMES 'gbk'" ) !=0 )
				{
					printf("mysql_query error:%s\n", mysql_error(&lmysql));
					return false;
				}
			}
			else
			{
				return false;
			}

		}
		
		if ( mysql_query(&lmysql, (sql).c_str() ) !=0 )
		{
			printf("mysql_query error:%s\n", mysql_error(&lmysql));
			return false;
		}
		//printf("ok!\n");

		LLastId = mysql_insert_id(&lmysql);

		return r;

	}

	//断开 mysql 服务器
	void LClose()
	{
		mysql_close(&lmysql);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	

	//执行一个SQL语句
	bool ExecuteMysqlSql(std::string sql, unsigned long * last_id)
	{
		MYSQL mysql;


		mysql_init(&mysql);
		//mysql_options(&mysql, MYSQL_OPT_RECONNECT, "1");//test 据说 5.0.13以后的API版本 可以用这个标志
		//if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "root", "ggtong", 0, NULL, 0))
		if (!mysql_real_connect(&mysql, m_Host.c_str(), m_User.c_str(), m_Pass.c_str(), m_DBName.c_str(), 0, NULL, 0))
		{
			printf("connect error:%s\n", mysql_error(&mysql));
		}

		//字符集不是 latin1 时要设置字符集
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )
		if ( mysql_query(&mysql, "SET NAMES 'gbk'" ) !=0 )
		{
			printf("mysql_query error:%s\n", mysql_error(&mysql));		
		}
		
		if ( mysql_query(&mysql, (sql).c_str() ) !=0 )
		{
			printf("mysql_query error:%s\n", mysql_error(&mysql));		
		}
		//printf("ok!\n");

		if (last_id != NULL)
		{
			(*last_id) = mysql_insert_id(&mysql);	
		}

		mysql_close(&mysql);

		return true;
	}


	//打开读取一个mysql表
	bool OpenMysqlSql(std::string sql, std::string id_field_name="")
	{
		printfd2("%s\r\n\r\n", sql.c_str());
	
	    MYSQL mysql;
		MYSQL_RES * result;
		MYSQL_ROW row;
		int num_fields;//字段个数
		int i;
		int line_id = 0;//行号
		unsigned long *lengths;//当前行的字段值长度
		MYSQL_FIELD *fields;//字段属性
		std::string key = "";
		std::string value = "";

		ClearResult();
		LRecordCount = 0;


		mysql_init(&mysql);
		//mysql_options(&mysql, MYSQL_OPT_RECONNECT, "1");//test 据说 5.0.13以后的API版本 可以用这个标志
		//if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "root", "ggtong", 0, NULL, 0))
		if (!mysql_real_connect(&mysql, m_Host.c_str(), m_User.c_str(), m_Pass.c_str(), m_DBName.c_str(), 0, NULL, 0))
		{//clq 这里其实会崩溃
			printf("connect error:%s\n", mysql_error(&mysql));
		}

		//字符集不是 latin1 时要设置字符集
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )
		if ( mysql_query(&mysql, "SET NAMES 'gbk'" ) !=0 )
		{
			printf("mysql_query error:%s\n", mysql_error(&mysql));		
		}

		if ( mysql_query(&mysql, (sql).c_str() ) !=0 )
		{//clq 这里也会崩溃//例如访问 http://127.0.0.1:8022/read.csp?tid=981'> 会出错
			printf("mysql_query error:%s\n", mysql_error(&mysql));		

			mysql_close(&mysql);
			return false;
		}
		//result = mysql_use_result(&mysql);
		result = mysql_store_result(&mysql);
		//用 mysql_store_result 更好
		//用 mysql_fetch_lengths 得到行的各个长度
		//printf (Number of rows: %lu\n", (unsigned long) mysql_num_rows(result));

		//得到表中记录数
		int numRecords = mysql_num_rows(result); 

		//遍历各行
		while((row = mysql_fetch_row(result)))
		{
			LRecordCount++;

			// do something with data
			//得到字段的个数
			num_fields = mysql_num_fields(result);


			//得到各个行的长度[注意这对各行的值是不同的]
			lengths = mysql_fetch_lengths(result);
			//得到字段名
			fields = mysql_fetch_fields(result);

			//printf("%s\r\n", row[0]);
			keys * k = new keys();//keys会被容器自动销毁的

			//遍历各字段
			for(i = 0; i < num_fields; i++)
			{
				//printf("Column %u is %lu bytes in length.\n", i, lengths[i]);
				//printf("Field %u is %s\n", i, fields[i].name);

				key = fields[i].name;//字段名
				//value = row[i];//字段值 

				if (row[i] == NULL) 
				{ 
					value = ""; 
				} 
				else 
				{ 
					value = row[i];//字段值 
				}

				(*k)[key] = value;

				continue;
			}

			if (id_field_name.size() > 0)
			{
				//std::string id = get_value((*k), "id");//主键
				std::string id = get_value((*k), id_field_name);//主键
				if (trim(id).length()==0)
				{
					continue;
				}
			}

			//(*k)["line_id"] = int_to_str(line_id);//人为加一个行号
			(*k)["line_id"] = int_to_str(line_id, 8);//人为加一个行号
			//加入到最后的数据集中
			//this->set_line(id, k, false);
			this->SetLine((*k)["line_id"], k, false);
			line_id++;//行号

		}
		mysql_free_result(result);//一定要释放
		//printf("ok!\n");
		mysql_close(&mysql);

		return true;
	}

	//生成一个错误结果集
	void MakeErrorResult(CMysqlFastResult & r, const std::string str)
	{
		//分配行
		r.rows.resize(1);

		//遍历各字段
		r.field_names.resize(1);

		std::string key = "error";//字段名
		r.fields[key] = 0;
		r.field_names[0] = "error";//字段名

		r.rows[0].resize(1);
		r.rows[0][0] = str.c_str();//用 c_str() 强制拷贝

	}


	//高速取得结果集的接口
	void fast(MYSQL_RES * result, CMysqlFastResult & r)
	{
		//CMysqlFastResult r;

		MYSQL_ROW row;
		int num_fields;//字段个数
		unsigned long *lengths;//当前行的字段值长度
		MYSQL_FIELD *fields;//字段属性
		std::string key = "";
		std::string value = "";


		//得到表中记录数
		int numRecords = mysql_num_rows(result);
		int rowIndex = 0;//行号
		int i = 0;

		//分配行
		r.rows.resize(numRecords);

		//得到字段名
		fields = mysql_fetch_fields(result);

		//得到字段的个数
		num_fields = mysql_num_fields(result);

		LFieldCount = num_fields;

		//得到各个行的长度[注意这对各行的值是不同的]
		lengths = mysql_fetch_lengths(result);

		//遍历各字段
		r.field_names.resize(num_fields);
		for(i = 0; i < num_fields; i++)
		{
			key = fields[i].name;//字段名
			r.fields[key] = i;
			r.field_names[i] = fields[i].name;//字段名
		}


		//遍历各行
		while((row = mysql_fetch_row(result)))
		{
			LRecordCount++;

			r.rows[rowIndex].resize(num_fields);

			//遍历各字段
			for(i = 0; i < num_fields; i++)
			{

				if (row[i] == NULL) 
				{ 
					value = ""; 
				} 
				else 
				{ 
					value = row[i];//字段值 
				}

				r.rows[rowIndex][i] = value;



			}

			rowIndex++;
		}

	}


	//打开读取一个mysql表
	bool LOpenSqlFast(std::string sql)
	{
		printf("%s\r\n", sql.c_str());//
		ClearResult();
		LRecordCount = 0;
	    
		MYSQL_RES * result;
		int line_id = 0;//行号
		std::string key = "";
		std::string value = "";


		std::string error = "";

		//字符集不是 latin1 时要设置字符集
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )
		if ( mysql_query(&lmysql, "SET NAMES 'gbk'" ) !=0 )
		{
			error = mysql_error(&lmysql);
			MakeErrorResult(LResult, error);
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		
		}

		if ( mysql_query(&lmysql, (sql).c_str() ) !=0 )
		{//clq 这里也会崩溃//例如访问 http://127.0.0.1:8022/read.csp?tid=981'> 会出错
			error = mysql_error(&lmysql);
			MakeErrorResult(LResult, error);
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		

			mysql_close(&lmysql);
			return false;
		}
		//result = mysql_use_result(&mysql);
		result = mysql_store_result(&lmysql);
		//用 mysql_store_result 更好
		//用 mysql_fetch_lengths 得到行的各个长度
		//printf (Number of rows: %lu\n", (unsigned long) mysql_num_rows(result));


		if (result != NULL)
		{
			fast(result, LResult);//test
			mysql_free_result(result);//一定要释放
		}
		else
		{
			MakeErrorResult(LResult, "no result");
		}

		
		return true;

	}
	
	//打开读取一个mysql表
	bool LOpenSql(std::string sql, std::string id_field_name)
	{
		printf("%s\r\n", sql.c_str());//
		ClearResult();
		LRecordCount = 0;
	    
		MYSQL_RES * result;
		MYSQL_ROW row;
		int num_fields;//字段个数
		int i;
		int line_id = 0;//行号
		unsigned long *lengths;//当前行的字段值长度
		MYSQL_FIELD *fields;//字段属性
		std::string key = "";
		std::string value = "";


		std::string error = "";

		//字符集不是 latin1 时要设置字符集
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )
		if ( mysql_query(&lmysql, "SET NAMES 'gbk'" ) !=0 )
		{
			error = mysql_error(&lmysql);
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		
		}

		if ( mysql_query(&lmysql, (sql).c_str() ) !=0 )
		{//clq 这里也会崩溃//例如访问 http://127.0.0.1:8022/read.csp?tid=981'> 会出错
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		

			mysql_close(&lmysql);
			return false;
		}
		//result = mysql_use_result(&mysql);
		result = mysql_store_result(&lmysql);
		//用 mysql_store_result 更好
		//用 mysql_fetch_lengths 得到行的各个长度
		//printf (Number of rows: %lu\n", (unsigned long) mysql_num_rows(result));

		//fast(result);//test

		//遍历各行
		while((row = mysql_fetch_row(result)))
		{
			LRecordCount++;

			// do something with data
			//得到字段的个数
			num_fields = mysql_num_fields(result);
			//得到各个行的长度[注意这对各行的值是不同的]
			lengths = mysql_fetch_lengths(result);
			//得到字段名
			fields = mysql_fetch_fields(result);

			//printf("%s\r\n", row[0]);
			keys * k = new keys();//keys会被容器自动销毁的

			//遍历各字段
			for(i = 0; i < num_fields; i++)
			{
				//printf("Column %u is %lu bytes in length.\n", i, lengths[i]);
				//printf("Field %u is %s\n", i, fields[i].name);

				key = fields[i].name;//字段名
				//value = row[i];//字段值 

				if (row[i] == NULL) 
				{ 
					value = ""; 
				} 
				else 
				{ 
					value = row[i];//字段值 
				}

				(*k)[key] = value;

				continue;
			}

			//std::string id = get_value((*k), "id");//主键
			std::string id = get_value((*k), id_field_name);//主键
			if (trim(id).length()==0)
			{
				continue;
			}

			//(*k)["line_id"] = int_to_str(line_id);//人为加一个行号
			(*k)["line_id"] = int_to_str(line_id, 8);//人为加一个行号
			//加入到最后的数据集中
			//this->set_line(id, k, false);
			this->SetLine((*k)["line_id"], k, false);
			line_id++;//行号

		}
		mysql_free_result(result);//一定要释放

		
		return true;
	}
	

	//打开读取一个数据文件的内容[mysql]
	bool OpenMysqlTable(std::string table_name, std::string id_field_name)
	{
		lock.lock();

		OpenMysqlSql(" select * from " + table_name + " ", id_field_name);

		lock.unlock();
		return true;
	}

	void SetLine(std::string key, keys * line, bool is_lock = true)
	{
		if (is_lock)
		lock.lock();//get_line中有锁,所以要放在它之后,或者是内部操作时调用不锁定的版本

		keys * p = this->GetLine(key,false);
		if (p!=NULL) 
		{
			delete p;
		}

		this->data[key] = line;

		if (is_lock)
		lock.unlock();

	}

	//得到记录个数
	int GetRecordCount(bool is_lock = true)
	{
		if (is_lock)
		lock.lock();

		int r = LRecordCount;//this->data.size();

		
		if (is_lock)
		lock.unlock();

		return r;
		
	}

	//得到字段个数
	int GetFieldCount(bool is_lock = true)
	{
		if (is_lock)
		lock.lock();

		int r = LFieldCount;//this->data.size();

		
		if (is_lock)
		lock.unlock();

		return r;
		
	}
	

	keys * GetLine(std::string key, bool is_lock = true)
	{
		if (is_lock)
		lock.lock();

		keys * p = NULL;
		std::map< std::string, keys * >::iterator iter;

		iter = this->data.find(key);
		if(iter != this->data.end())
		{	
			p = iter->second;
		}
		
		if (is_lock)
		lock.unlock();

		return p;
		
	}

	keys * GetNextLine(std::string key, bool is_lock = true)
	{
		if (is_lock)
		lock.lock();

		keys * p = NULL;
		std::map< std::string, keys * >::iterator iter;

		iter = this->data.find(key);
		if(iter != this->data.end())
		{	
			iter++;
			if(iter != this->data.end())
			{	
				p = iter->second;
			}
		}
		
		if (is_lock)
		lock.unlock();

		return p;
		
	}

	keys * GetFirstLine(bool is_lock = true)
	{
		if (is_lock)
		lock.lock();

		keys * p = NULL;
		std::map< std::string, keys * >::iterator iter;

		iter = this->data.begin();
		if(iter != this->data.end())
		{	

			p = iter->second;

		}
		
		if (is_lock)
		lock.unlock();

		return p;
		
	}


	std::string NewId()
	{
		std::string s = "";
		struct tm *newtime;
		time_t long_time;
		time( &long_time );                /* Get time as long integer. */
		newtime = localtime( &long_time ); /* Convert to local time. */

		s = int_to_str(mktime(newtime));

		return s;
	}

	//php的time函数与C一样//返回自从 Unix 新纪元（格林威治时间 1970 年 1 月 1 日 00:00:00）到当前时间的秒数。 
	std::string GetTime()
	{
		std::string s = "";
		struct tm *newtime;
		time_t long_time;
		time( &long_time );                /* Get time as long integer. */
		newtime = localtime( &long_time ); /* Convert to local time. */

		s = int_to_str(mktime(newtime));

		return s;
	}

	//得到结果
	void GetResult(CMysqlResult * r)
	{
		keys * k = NULL;
		k = this->GetFirstLine();
		while(k!=NULL)
		{
			std::string line_id = get_value((*k), "line_id");

			keys * rk = new keys();//keys会被容器自动销毁的

			keys::iterator iter = k->begin();
			for(; iter != k->end(); iter++)
			{
				std::string key = iter->first;
				std::string value = iter->second;

				(*rk)[key] = value;

			}
			r->data.push_back(rk);


			//k = f.get_next_line(id);//跳到下一组
			k = this->GetNextLine(line_id);//跳到下一组
		}

	}


};



#endif
