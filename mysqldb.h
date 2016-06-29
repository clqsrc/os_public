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

//ȡ��VC��ʹ��STL�ľ���
#pragma warning(disable:4503)//�ŵ�os.h�в���

//���ٷ��ؼ�
typedef std::vector<std::string> MysqlLine;//һ�м�¼

//���ٷ��ؼ�//ע��,ֻ�ʺ��ֶ�������Ϊ 255 �ֽ����µı��
class CMysqlFastResult
{
public:
	std::map<std::string, int> fields;//�ֶ�
	std::vector<std::string> field_names;//�ֶ�
	std::vector<MysqlLine> rows;//����
	int curRow;//��ǰ�к�

public:

	CMysqlFastResult()
	{
		curRow = 0;
	}

	~CMysqlFastResult()
	{
		Clear();
	}


	//�����ڴ����Դ
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

	//����һ��
	void Next()
	{
		curRow++;
	}

	int GetRowCount()
	{
		return rows.size();
	}

};


//���ؼ�
class CMysqlResult
{
public:
	std::vector<keys *> data;//�����

public:

	CMysqlResult()
	{
	
	}

	~CMysqlResult()
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


class CMysqldb
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
	MYSQL lmysql;
	unsigned long LLastId;
	bool lconnected;//�Ƿ�Ͽ��ı�־
	int LRecordCount;//��¼����
	int LFieldCount;//�ֶθ���
	
public:
	CMysqlFastResult LResult;//�µĽ����

public:
	CMysqldb()
	{
		mysql_init(&lmysql);
		//mysql_options(&lmysql, MYSQL_OPT_RECONNECT, "1");//test ��˵ 5.0.13�Ժ��API�汾 �����������־
		lconnected = false;
	}

	~CMysqldb()
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
		LFieldCount = 0;
	}



	//mysql�Ķ����Ʊ�ʾsql
	std::string GetSqlString(std::string s_from)
	{
		std::string s;
		char * buf = (char *)malloc(s_from.length() * 2 +1);//����Ҫ�����mysql�ֲ�
		mysql_escape_string(buf, s_from.c_str(), s_from.length()); 

		s = buf;
		free(buf);

		return s;
	}

	//ִ��һ��SQL���
	bool ExecuteMysqlSql(std::string sql)
	{
		return ExecuteMysqlSql(sql, NULL);
	}


	//////////////////////////////////////////////////////////////////////////////////////
	//�����ӵĺ�����Ҫ�ر�ע��

	//���� mysql ������
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

	//ִ��һ��SQL���//��ô֪�� mysql �������Ѿ��Ͽ�������
	//unsigned int mysql_errno(MYSQL *mysql) ��ȡ�ô�������б��� http://dev.mysql.com/doc/refman/5.0/en/error-handling.html
	//�о� 2000 �� 3000 ֮�ڵ����������������������
	bool LExecuteSql(const std::string sql)
	{
		//printfd2("%s\r\n", sql.c_str());

		bool r = true;

		//�ַ������� latin1 ʱҪ�����ַ���
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )//����
		if ( mysql_query(&lmysql, "SET NAMES 'gbk'" ) !=0 )
		{
			printf("mysql_query error:%s\n", mysql_error(&lmysql));

			unsigned int eo = mysql_errno(&lmysql);

			if ((eo < 3000)&&(eo >= 2000))//���������Ϊ mysql �������Ͽ�
			{
				LConnect();
				//��������һ��
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

	//�Ͽ� mysql ������
	void LClose()
	{
		mysql_close(&lmysql);
	}
	
	//////////////////////////////////////////////////////////////////////////////////////
	

	//ִ��һ��SQL���
	bool ExecuteMysqlSql(std::string sql, unsigned long * last_id)
	{
		MYSQL mysql;


		mysql_init(&mysql);
		//mysql_options(&mysql, MYSQL_OPT_RECONNECT, "1");//test ��˵ 5.0.13�Ժ��API�汾 �����������־
		//if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "root", "ggtong", 0, NULL, 0))
		if (!mysql_real_connect(&mysql, m_Host.c_str(), m_User.c_str(), m_Pass.c_str(), m_DBName.c_str(), 0, NULL, 0))
		{
			printf("connect error:%s\n", mysql_error(&mysql));
		}

		//�ַ������� latin1 ʱҪ�����ַ���
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


	//�򿪶�ȡһ��mysql��
	bool OpenMysqlSql(std::string sql, std::string id_field_name="")
	{
		printfd2("%s\r\n\r\n", sql.c_str());
	
	    MYSQL mysql;
		MYSQL_RES * result;
		MYSQL_ROW row;
		int num_fields;//�ֶθ���
		int i;
		int line_id = 0;//�к�
		unsigned long *lengths;//��ǰ�е��ֶ�ֵ����
		MYSQL_FIELD *fields;//�ֶ�����
		std::string key = "";
		std::string value = "";

		ClearResult();
		LRecordCount = 0;


		mysql_init(&mysql);
		//mysql_options(&mysql, MYSQL_OPT_RECONNECT, "1");//test ��˵ 5.0.13�Ժ��API�汾 �����������־
		//if (!mysql_real_connect(&mysql, "127.0.0.1", "root", "root", "ggtong", 0, NULL, 0))
		if (!mysql_real_connect(&mysql, m_Host.c_str(), m_User.c_str(), m_Pass.c_str(), m_DBName.c_str(), 0, NULL, 0))
		{//clq ������ʵ�����
			printf("connect error:%s\n", mysql_error(&mysql));
		}

		//�ַ������� latin1 ʱҪ�����ַ���
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )
		if ( mysql_query(&mysql, "SET NAMES 'gbk'" ) !=0 )
		{
			printf("mysql_query error:%s\n", mysql_error(&mysql));		
		}

		if ( mysql_query(&mysql, (sql).c_str() ) !=0 )
		{//clq ����Ҳ�����//������� http://127.0.0.1:8022/read.csp?tid=981'> �����
			printf("mysql_query error:%s\n", mysql_error(&mysql));		

			mysql_close(&mysql);
			return false;
		}
		//result = mysql_use_result(&mysql);
		result = mysql_store_result(&mysql);
		//�� mysql_store_result ����
		//�� mysql_fetch_lengths �õ��еĸ�������
		//printf (Number of rows: %lu\n", (unsigned long) mysql_num_rows(result));

		//�õ����м�¼��
		int numRecords = mysql_num_rows(result); 

		//��������
		while((row = mysql_fetch_row(result)))
		{
			LRecordCount++;

			// do something with data
			//�õ��ֶεĸ���
			num_fields = mysql_num_fields(result);


			//�õ������еĳ���[ע����Ը��е�ֵ�ǲ�ͬ��]
			lengths = mysql_fetch_lengths(result);
			//�õ��ֶ���
			fields = mysql_fetch_fields(result);

			//printf("%s\r\n", row[0]);
			keys * k = new keys();//keys�ᱻ�����Զ����ٵ�

			//�������ֶ�
			for(i = 0; i < num_fields; i++)
			{
				//printf("Column %u is %lu bytes in length.\n", i, lengths[i]);
				//printf("Field %u is %s\n", i, fields[i].name);

				key = fields[i].name;//�ֶ���
				//value = row[i];//�ֶ�ֵ 

				if (row[i] == NULL) 
				{ 
					value = ""; 
				} 
				else 
				{ 
					value = row[i];//�ֶ�ֵ 
				}

				(*k)[key] = value;

				continue;
			}

			if (id_field_name.size() > 0)
			{
				//std::string id = get_value((*k), "id");//����
				std::string id = get_value((*k), id_field_name);//����
				if (trim(id).length()==0)
				{
					continue;
				}
			}

			//(*k)["line_id"] = int_to_str(line_id);//��Ϊ��һ���к�
			(*k)["line_id"] = int_to_str(line_id, 8);//��Ϊ��һ���к�
			//���뵽�������ݼ���
			//this->set_line(id, k, false);
			this->SetLine((*k)["line_id"], k, false);
			line_id++;//�к�

		}
		mysql_free_result(result);//һ��Ҫ�ͷ�
		//printf("ok!\n");
		mysql_close(&mysql);

		return true;
	}

	//����һ����������
	void MakeErrorResult(CMysqlFastResult & r, const std::string str)
	{
		//������
		r.rows.resize(1);

		//�������ֶ�
		r.field_names.resize(1);

		std::string key = "error";//�ֶ���
		r.fields[key] = 0;
		r.field_names[0] = "error";//�ֶ���

		r.rows[0].resize(1);
		r.rows[0][0] = str.c_str();//�� c_str() ǿ�ƿ���

	}


	//����ȡ�ý�����Ľӿ�
	void fast(MYSQL_RES * result, CMysqlFastResult & r)
	{
		//CMysqlFastResult r;

		MYSQL_ROW row;
		int num_fields;//�ֶθ���
		unsigned long *lengths;//��ǰ�е��ֶ�ֵ����
		MYSQL_FIELD *fields;//�ֶ�����
		std::string key = "";
		std::string value = "";


		//�õ����м�¼��
		int numRecords = mysql_num_rows(result);
		int rowIndex = 0;//�к�
		int i = 0;

		//������
		r.rows.resize(numRecords);

		//�õ��ֶ���
		fields = mysql_fetch_fields(result);

		//�õ��ֶεĸ���
		num_fields = mysql_num_fields(result);

		LFieldCount = num_fields;

		//�õ������еĳ���[ע����Ը��е�ֵ�ǲ�ͬ��]
		lengths = mysql_fetch_lengths(result);

		//�������ֶ�
		r.field_names.resize(num_fields);
		for(i = 0; i < num_fields; i++)
		{
			key = fields[i].name;//�ֶ���
			r.fields[key] = i;
			r.field_names[i] = fields[i].name;//�ֶ���
		}


		//��������
		while((row = mysql_fetch_row(result)))
		{
			LRecordCount++;

			r.rows[rowIndex].resize(num_fields);

			//�������ֶ�
			for(i = 0; i < num_fields; i++)
			{

				if (row[i] == NULL) 
				{ 
					value = ""; 
				} 
				else 
				{ 
					value = row[i];//�ֶ�ֵ 
				}

				r.rows[rowIndex][i] = value;



			}

			rowIndex++;
		}

	}


	//�򿪶�ȡһ��mysql��
	bool LOpenSqlFast(std::string sql)
	{
		printf("%s\r\n", sql.c_str());//
		ClearResult();
		LRecordCount = 0;
	    
		MYSQL_RES * result;
		int line_id = 0;//�к�
		std::string key = "";
		std::string value = "";


		std::string error = "";

		//�ַ������� latin1 ʱҪ�����ַ���
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )
		if ( mysql_query(&lmysql, "SET NAMES 'gbk'" ) !=0 )
		{
			error = mysql_error(&lmysql);
			MakeErrorResult(LResult, error);
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		
		}

		if ( mysql_query(&lmysql, (sql).c_str() ) !=0 )
		{//clq ����Ҳ�����//������� http://127.0.0.1:8022/read.csp?tid=981'> �����
			error = mysql_error(&lmysql);
			MakeErrorResult(LResult, error);
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		

			mysql_close(&lmysql);
			return false;
		}
		//result = mysql_use_result(&mysql);
		result = mysql_store_result(&lmysql);
		//�� mysql_store_result ����
		//�� mysql_fetch_lengths �õ��еĸ�������
		//printf (Number of rows: %lu\n", (unsigned long) mysql_num_rows(result));


		if (result != NULL)
		{
			fast(result, LResult);//test
			mysql_free_result(result);//һ��Ҫ�ͷ�
		}
		else
		{
			MakeErrorResult(LResult, "no result");
		}

		
		return true;

	}
	
	//�򿪶�ȡһ��mysql��
	bool LOpenSql(std::string sql, std::string id_field_name)
	{
		printf("%s\r\n", sql.c_str());//
		ClearResult();
		LRecordCount = 0;
	    
		MYSQL_RES * result;
		MYSQL_ROW row;
		int num_fields;//�ֶθ���
		int i;
		int line_id = 0;//�к�
		unsigned long *lengths;//��ǰ�е��ֶ�ֵ����
		MYSQL_FIELD *fields;//�ֶ�����
		std::string key = "";
		std::string value = "";


		std::string error = "";

		//�ַ������� latin1 ʱҪ�����ַ���
		//if ( mysql_query(&mysql, "SET character_set_connection='gbk'" ) !=0 )
		if ( mysql_query(&lmysql, "SET NAMES 'gbk'" ) !=0 )
		{
			error = mysql_error(&lmysql);
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		
		}

		if ( mysql_query(&lmysql, (sql).c_str() ) !=0 )
		{//clq ����Ҳ�����//������� http://127.0.0.1:8022/read.csp?tid=981'> �����
			printf("mysql_query error:%s\n", mysql_error(&lmysql));		

			mysql_close(&lmysql);
			return false;
		}
		//result = mysql_use_result(&mysql);
		result = mysql_store_result(&lmysql);
		//�� mysql_store_result ����
		//�� mysql_fetch_lengths �õ��еĸ�������
		//printf (Number of rows: %lu\n", (unsigned long) mysql_num_rows(result));

		//fast(result);//test

		//��������
		while((row = mysql_fetch_row(result)))
		{
			LRecordCount++;

			// do something with data
			//�õ��ֶεĸ���
			num_fields = mysql_num_fields(result);
			//�õ������еĳ���[ע����Ը��е�ֵ�ǲ�ͬ��]
			lengths = mysql_fetch_lengths(result);
			//�õ��ֶ���
			fields = mysql_fetch_fields(result);

			//printf("%s\r\n", row[0]);
			keys * k = new keys();//keys�ᱻ�����Զ����ٵ�

			//�������ֶ�
			for(i = 0; i < num_fields; i++)
			{
				//printf("Column %u is %lu bytes in length.\n", i, lengths[i]);
				//printf("Field %u is %s\n", i, fields[i].name);

				key = fields[i].name;//�ֶ���
				//value = row[i];//�ֶ�ֵ 

				if (row[i] == NULL) 
				{ 
					value = ""; 
				} 
				else 
				{ 
					value = row[i];//�ֶ�ֵ 
				}

				(*k)[key] = value;

				continue;
			}

			//std::string id = get_value((*k), "id");//����
			std::string id = get_value((*k), id_field_name);//����
			if (trim(id).length()==0)
			{
				continue;
			}

			//(*k)["line_id"] = int_to_str(line_id);//��Ϊ��һ���к�
			(*k)["line_id"] = int_to_str(line_id, 8);//��Ϊ��һ���к�
			//���뵽�������ݼ���
			//this->set_line(id, k, false);
			this->SetLine((*k)["line_id"], k, false);
			line_id++;//�к�

		}
		mysql_free_result(result);//һ��Ҫ�ͷ�

		
		return true;
	}
	

	//�򿪶�ȡһ�������ļ�������[mysql]
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

	//�õ��ֶθ���
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

	//php��time������Cһ��//�����Դ� Unix �¼�Ԫ����������ʱ�� 1970 �� 1 �� 1 �� 00:00:00������ǰʱ��������� 
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

	//�õ����
	void GetResult(CMysqlResult * r)
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
