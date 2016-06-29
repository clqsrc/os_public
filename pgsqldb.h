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

//ȡ��VC��ʹ��STL�ľ���
#pragma warning(disable:4503)//�ŵ�os.h�в���

class CPgsqlResult
{
public:
	std::vector<keys *> data;//�����

public:

	CPgsqlResult()
	{
	
	}

	~CPgsqlResult()
	{
		Clear();
	}


	//�����ڴ����Դ
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

//���� mysql ����


class CPgsqldb
{
public://�����õı���
	static std::string m_Host;//��ַ
	static std::string m_User;//�û���
	static std::string m_Pass;//����
	static std::string m_DBName;//���ݿ���


private:
	std::map< std::string, keys * > data;//��������//������Ψһ����
	thread_lock lock;//�߳���

private://���������õı�����Ҫ�ر�ע�ⳤ����������ӵ����֣������ӵĺ���ǰ��ǰ׺ L
	//�ر�ע�ⳤ���Ӳ����̰߳�ȫ��
	PGconn * lconn;
	unsigned long LLastId;
	bool lconnected;//�Ƿ�Ͽ��ı�־
	int LRecordCount;//��¼����

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

	//�����ڴ����Դ
	void Clear()
	{
		ClearResult();
	}


	//������ֵ
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
	//�����ӵĺ�����Ҫ�ر�ע��

	//���� mysql ������
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

	//ִ��һ��SQL���//��ô֪�� mysql �������Ѿ��Ͽ�������
	//unsigned int mysql_errno(MYSQL *mysql) ��ȡ�ô�������б��� http://dev.mysql.com/doc/refman/5.0/en/error-handling.html
	//�о� 2000 �� 3000 ֮�ڵ����������������������
	bool LExecuteSql(const std::string sql)
	{
		//printfd2("%s\r\n", sql.c_str());

		bool r = true;

		PGresult * res = NULL;

		if (lconn == NULL)
		{
			printf("δ�������ݿ�.\r\n");
			//printf_error("δ�������ݿ�.\r\n");
			return false;
		}

		res = PQexec(lconn, sql.c_str());
		if (res != NULL)
		{
			int status = PQresultStatus(res);
			if (status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK)
			{
				//���Ӧ����ȡ�ֶε�ֵ
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

	//�Ͽ� mysql ������
	void LClose()
	{
		if (lconn != NULL)
		{
			PQfinish(lconn);
		}
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	
	
	//�򿪶�ȡһ��mysql��
	//bool LOpenSql(std::string sql, std::string id_field_name)
	bool LOpenSql(std::string sql)
	{
		printf("%s\r\n", sql.c_str());//
		ClearResult();
		LRecordCount = 0;
		bool r = true;

		int line_id = 0;//�к�
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
				int rcount = PQntuples(res);//��¼��
				int fcount =  PQnfields(res);//�ֶθ���

				//��������
				for (int i = 0; i < rcount; i++)
				{
					LRecordCount++;

					keys * k = new keys();//keys�ᱻ�����Զ����ٵ�

					//�������ֶ�
					for (int j = 0; j < fcount; j++)
					{
						key = PQfname(res, j);//�ֶ���
						value = PQgetvalue(res, i, j);//�ֶ�ֵ//����Ӧ���Ǽ�¼�������ֶ�����

						(*k)[key] = value;
					}

					(*k)["line_id"] = int_to_str(line_id, 8);//��Ϊ��һ���к�
					//���뵽�������ݼ���
					//this->set_line(id, k, false);
					this->SetLine((*k)["line_id"], k, false);
					line_id++;//�к�

				}


				//���Ӧ����ȡ�ֶε�ֵ
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
		lock.lock();//get_line������,����Ҫ������֮��,�������ڲ�����ʱ���ò������İ汾

		keys * p = this->GetLine(key,false);
		if (p!=NULL) 
		{
			delete p;
		}

		this->data[key] = line;

		if (is_lock)
		lock.unlock();

	}

	//�õ���¼����
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


	//�õ����
	void GetResult(CPgsqlResult * r)
	{
		keys * k = NULL;
		k = this->GetFirstLine();
		while(k!=NULL)
		{
			std::string line_id = get_value((*k), "line_id");

			keys * rk = new keys();//keys�ᱻ�����Զ����ٵ�

			keys::iterator iter = k->begin();
			for(; iter != k->end(); iter++)
			{
				std::string key = iter->first;
				std::string value = iter->second;

				(*rk)[key] = value;

			}
			r->data.push_back(rk);


			//k = f.get_next_line(id);//������һ��
			k = this->GetNextLine(line_id);//������һ��
		}

	}


};



#endif
