#ifndef _CPgsql_DB_H_
#define _CPgsql_DB_H_

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

//取消VC中使用STL的警告
#pragma warning(disable:4503)//放到os.h中才行

class CPgsqlResult
{
public:
	std::vector<keys *> data;//结果集

public:

	CPgsqlResult()
	{
	
	}

	~CPgsqlResult()
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


class CPgsqldb
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
	PGconn * lconn;
	unsigned long LLastId;
	bool lconnected;//是否断开的标志
	int LRecordCount;//记录个数

public:
	CPgsqldb()
	{
		lconn = NULL;

		lconnected = false;
	}

	~CPgsqldb()
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
	}



	//////////////////////////////////////////////////////////////////////////////////////
	//长连接的函数，要特别注意

	//连接 mysql 服务器
	bool LConnect()
	{
		bool r = true;

		//lconn = PQconnectdb("host=127.0.0.1 user=root password=root dbname=ggtong port=5432");
		lconn = PQconnectdb(("host=" + m_Host + " user=" +m_User+ " password=" +m_Pass+ " dbname=" +m_DBName+ " port=5432").c_str());
		if (PQstatus(lconn) == CONNECTION_OK)
		{
			lconnected = true;
		}
		else
		{
			r = false;
			lconnected = false;
			//printf("connect error:%s\n", mysql_error(&lmysql));
			printf("pgsql connect failed: %s", PQerrorMessage(lconn));
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

		PGresult * res = NULL;

		if (lconn == NULL)
		{
			printf("未连接数据库.\r\n");
			//printf_error("未连接数据库.\r\n");
			return false;
		}

		res = PQexec(lconn, sql.c_str());
		if (res != NULL)
		{
			int status = PQresultStatus(res);
			if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK)
			{
				//这个应该是取字段的值
				//char * res_userid = PQgetvalue(res, 0, 0);
				//char * res_password = PQgetvalue(res, 0, 1);
				//....
			}
			else
			{
				r = false;
				printf("pgsql failed: %s", PQerrorMessage(lconn));
				PQclear(res);			
			}
			PQclear(res);
		}
		

		//printf("ok!\n");

		//LLastId = mysql_insert_id(&lmysql);

		return r;

	}

	//断开 mysql 服务器
	void LClose()
	{
		if (lconn != NULL)
		{
			PQfinish(lconn);
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	
	
	//打开读取一个mysql表
	//bool LOpenSql(std::string sql, std::string id_field_name)
	bool LOpenSql(std::string sql)
	{
		printf("%s\r\n", sql.c_str());//
		ClearResult();
		LRecordCount = 0;
		bool r = true;

		int line_id = 0;//行号
		std::string key = "";
		std::string value = "";

		std::string error = "";


		PGresult * res = NULL;
		res = PQexec(lconn, sql.c_str());
		if (res != NULL)
		{
			int status = PQresultStatus(res);
			if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK)
			{
				int rcount = PQntuples(res);//记录数
				int fcount =  PQnfields(res);//字段个数

				//遍历各行
				for (int i = 0; i < rcount; i++)
				{
					LRecordCount++;

					keys * k = new keys();//keys会被容器自动销毁的

					//遍历各字段
					for (int j = 0; j < fcount; j++)
					{
						key = PQfname(res, j);//字段名
						value = PQgetvalue(res, i, j);//字段值//参数应该是记录索引的字段索引

						(*k)[key] = value;
					}

					(*k)["line_id"] = int_to_str(line_id, 8);//人为加一个行号
					//加入到最后的数据集中
					//this->set_line(id, k, false);
					this->SetLine((*k)["line_id"], k, false);
					line_id++;//行号

				}


				//这个应该是取字段的值
				//char * res_userid = PQgetvalue(res, 0, 0);
				//char * res_password = PQgetvalue(res, 0, 1);
				//....
			}
			else
			{
				r = false;
				printf("pgsql failed: %s", PQerrorMessage(lconn));
				PQclear(res);			
			}
			PQclear(res);
		}
	    
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


	//得到结果
	void GetResult(CPgsqlResult * r)
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
