#ifndef _DBF_H_
#define _DBF_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include "file_system.h"
#include <iostream>

//#include "server_socket.h"
//#include "ftp_client_socket.h"
#include "public_function.h"
//#include "app_public_function.h"

#pragma pack(1)

//dbf的头//注意没有字段数目的字段,它要通过字段名为来表示结束了
typedef struct
{
	unsigned char  dbf_id;			//表的类型
									// dbf_id=03, memo field does not exist
									// dbf_id=83, memo field exists( foxbase )
									// dbf_id=F5, memo field exists ( foxpro )

	char   last_update[3];			//修改日期// year,month,date of the file last_update
	long   rec_count;				//记录数//4-7   32-bit      Number of records in the table.
	unsigned short data_offset;		//数据的开始// the start address of the data item//也有称做 Number of bytes in the header. 的
	unsigned short one_rec_size;	//一条记录占几个字节,其中最后一位为保留// the record size = all fields size + 1
	char   filler[20];				// unused
} DBF_HEAD;

//每个字段的信息//字段名,类型等//它在文件中的位置应当紧贴着上一个
typedef struct
{
	char field_name[11];			//字段名
	char field_type;				//字段类型
	char dummy[4];					// unused
	//union
	//{
		//unsigned short wlen;		// when the field type is not numeric
		//struct
		//{
			unsigned char len;		//字段的长度
			unsigned char dec;
		//}N;						// when the field type is numeric
	//} linfo;
	char filler[14];				// unused
}FIELD_REC;

#pragma pack()//一定不能少这个,否则会导致访问内存混乱

class cell//表示一个数据值的类型
{

public://变量
	//std::string  s_value;//字符串值//动态分配内存太费cpu
	char  s_value[255];//字符串值
	int i_value;//整数值
	double f_value;//浮点数值
	bool type;//这个数据是整数类型

};

enum DATA_TYPE 
{ TYPE_INT, TYPE_STRING, TYPE_FLOAT };

class field//表示一个字段
{

public://变量
	std::string  name;
	int len;
	DATA_TYPE type;//这个数据是整数类型

};


typedef std::vector<cell> line;//一行
typedef std::vector<line> table;//整张表

typedef std::vector<cell> dbf_line;//一行
typedef std::vector<line> dbf_table;//整张表

//dbf读取的类
class dbf
{

public://变量

	keys config;//配置信息
	FILE * handle;
	DBF_HEAD head;
	FIELD_REC rec_info[256];
	int field_count;//字段数
	int record_count;//记录数
	int record_len;//一条记录的整个长度
	int file_len;
	char * file_buf;//文件缓冲
	std::string file_name;
	field fields[255];//字段
	table lines;//当前值
	//table old_lines;//上次扫描的值
	bool lock_file;//操作dbf时是否锁定文件

public://函数

	//构造函数
	dbf::dbf()
	{
		field_count = 0;
		record_count = 0;
		file_buf = NULL;
	}

	dbf::~dbf()
	{
		if (file_buf != NULL) 
			free(file_buf);
	}

	bool dbf::load_from_file(std::string fn)
	{
		file_name = fn;

		handle = fopen(fn.c_str(), "rb");
		if (handle == NULL)
		{
			printf("dbf文件[%s]打开失败!\r\n", fn.c_str());
			return false;
		}
		fread(&head, 1, sizeof(head), handle);

		record_count = head.rec_count;

		field_count = 0;
		record_len = 0;
		//字段信息是有多个的
		while(true)
		{
			fread(&rec_info[field_count], 1, sizeof(FIELD_REC), handle);

			if(rec_info[field_count].field_name[0] == 0x0d)	//字段结束
			break;

			fields[field_count].name = rec_info[field_count].field_name;//字段名
			fields[field_count].len = rec_info[field_count].len;//字段长度

			//校验长度
			if ( (fields[field_count].len > 250) || (fields[field_count].len < 0) )
			{
				while(true)
					printf("dbf 字段长度错误 dbf::load_from_file().\r\n");
			}

			/*//不用判断了,都是字符串
			//数据类型
			if (rec_info[field_count].field_type)
			{
				fields[field_count].type = TYPE_INT;
			}
			*/

			record_len += rec_info[field_count].len;//计算字段长度
			field_count++;//计算字段数

		}

		record_len += 1;//每条记录前还有一个删除标识

		fclose(handle);

		//文件长度
		file_len = file_system::get_file_length(fn.c_str());
		//file_buf = new char(file_len);

		if (file_buf != NULL) 
			free(file_buf);

		file_buf = (char *)malloc(file_len);

		if (file_buf == NULL)
		{
			while (true)
				printf("内存不足,dbf::load_from_file()[%d,%s]\r\n", file_len, fn.c_str());
		}

		//记录数有可能是错的//要修正//比如 pb 提供的 dbf 就有这种情况
		record_count = (file_len - sizeof(DBF_HEAD) - field_count * sizeof(FIELD_REC))/record_len;

		//为了安全起见
		if (record_count < head.rec_count)
		{
			record_count = record_count - 1;
		}

		//总之是要取最小值
		if (record_count > head.rec_count)
		{
			record_count = head.rec_count;
		}

		return true;

	}


	bool dbf::read_to_buf()//读取整个文件到内存中//文件在200K左右
	{
		handle = fopen(file_name.c_str(), "rb");
		if (handle == NULL)
		{
			printf("dbf文件[%s]打开失败!", file_name.c_str());
			return false;
		}

		int r = fread(file_buf, 1, file_len, handle);

		if (r != file_len)
			return false;

		fclose(handle);
		return true;
	}

	//从内存中读
	bool dbf::load_from_mem()
	{
		char * p = file_buf;

		memcpy(&head, p, sizeof(head));
		p += sizeof(head);

		record_count = head.rec_count;

		field_count = 0;
		record_len = 0;
		//字段信息是有多个的
		while(true)
		{
			//fread(&rec_info[field_count], 1, sizeof(FIELD_REC), handle);
			memcpy(&rec_info[field_count], p, sizeof(FIELD_REC));
			p += sizeof(FIELD_REC);


			if(rec_info[field_count].field_name[0] == 0x0d)	//字段结束
			break;

			fields[field_count].name = rec_info[field_count].field_name;//字段名
			fields[field_count].len = rec_info[field_count].len;//字段长度

			//校验长度
			if ( (fields[field_count].len > 250) || (fields[field_count].len < 0) )
			{
				while(true)
					printf("dbf 字段长度错误 dbf::load_from_file().\r\n");
			}

			/*//不用判断了,都是字符串
			//数据类型
			if (rec_info[field_count].field_type)
			{
				fields[field_count].type = TYPE_INT;
			}
			*/

			record_len += rec_info[field_count].len;//计算字段长度
			field_count++;//计算字段数

		}

		record_len += 1;//每条记录前还有一个删除标识

		//记录数有可能是错的//要修正//比如 pb 提供的 dbf 就有这种情况
		//record_count = (file_len - field_count * sizeof(FIELD_REC))/record_len;
		record_count = (file_len - sizeof(DBF_HEAD) - field_count * sizeof(FIELD_REC))/record_len;

		//为了安全起见
		if (record_count < head.rec_count)
		{
			record_count = record_count - 1;
		}

		//总之是要取最小值
		if (record_count > head.rec_count)
		{
			record_count = head.rec_count;
		}

		//fclose(handle);


		return true;

	}

	//读取全部内容
	bool dbf::load_file_all_data(std::string fn)
	{

		file_name = fn;

		handle = fopen(fn.c_str(), "rb");
		if (handle == NULL)
		{
			printf("dbf文件[%s]打开失败!\r\n", fn.c_str());
			return false;
		}

		if (lock_file == true)
		{
			//锁定文件
			file_system::lock_file(handle);
		}

		//文件长度
		file_len = file_system::get_file_length(handle);

		if (file_len < 1)
		{
			printf("dbf文件[%s]打开失败!未取得文件长度。\r\n", fn.c_str());

			file_system::unlock_file(handle);
			return false;
		}


		if (file_buf != NULL) 
			free(file_buf);

		file_buf = (char *)malloc(file_len);

		if (file_buf == NULL)
		{
			while (true)
				printf("内存不足,dbf::load_from_mem()\r\n");
		}


		int r = fread(file_buf, 1, file_len, handle);

		if (lock_file == true)
		{
			//解锁文件//好象在 windows 下不用这个也是可以的，文件会在时自动解锁?
			file_system::unlock_file(handle);
		}

		if (r != file_len)
		{
			fclose(handle);
			return false;
		}
		fclose(handle);

		return true;
	}


	//分配内存
	bool dbf::malloc_table(table & t)
	{
		int i = 0;

		try
		{

			t.resize(record_count);//设置记录数//记录数太大是不行的 比如538968845

			for (i=0; i<record_count; i++)
			{
				t[i].resize(field_count);//设置字段数

			}

		}
		catch(...)
		{
			std::string err = "大小越界! dbf::malloc_table()[" +int_to_str(record_count)+ "][" +int_to_str(field_count)+ "]\r\n";
			printf_error(err.c_str());
		}

		return true;

	}

	//校验长度//当前读取位置是否超过文件长度
	bool dbf::check_point(char * p)
	{
		if (p > file_buf + file_len)
		{
			printf_error(("内存越界 dbf::check_point()file_len:" +int_to_str(file_len)+ ".\r\n").c_str());
			return true;
		}
		else
		{
			return false;
		}
	}


	//读取到内存文件内容用序列表示
	void dbf::buf_to_table(table & t)
	{
		int i,j;
		//std::vector<int> table1;
		std::string s;
		char v_buf[255];//
		char * cur_data_buf;
		//int i_value;
		//s.resize();

		char * data_buf = file_buf + head.data_offset;//数据位置
		
		//不行,太费CPU//t.resize(record_count);//设置记录数

		for (i=0; i<record_count; i++)
		{
			//不行,太费CPU//t[i].resize(field_count);//设置字段数

			//校验长度
			check_point(data_buf + record_len);

			cur_data_buf = data_buf +1;
			//data_buf += 1;//第一个是删除标志,所以还要先跳过它

			for (j=0; j<field_count; j++)
			{

				//校验长度
				check_point(cur_data_buf);

				int min_len = 0;//安全的最小长度
				min_len = sizeof(v_buf);
				if (fields[j].len < min_len) min_len = fields[j].len;
				if (sizeof(t[i][j].s_value) < min_len) min_len = sizeof(t[i][j].s_value);

				memset(&v_buf, 0, sizeof(v_buf));

				//memcpy(&v_buf, cur_data_buf, fields[j].len);
				//t[i][j].s_value = v_buf;//取得一个单元格的值//动态分配内存太费cpu
				//strcpy(t[i][j].s_value, v_buf);//取得一个单元格的值//动态分配内存太费cpu

				memcpy(&v_buf, cur_data_buf, min_len);
				//前面有 memset 所以是可以用 strcpy 的//strncpy(t[i][j].s_value, v_buf, );//取得一个单元格的值//动态分配内存太费cpu
				strcpy(t[i][j].s_value, v_buf);//取得一个单元格的值//动态分配内存太费cpu

				/*//不用判断了,都是字符串
				if (fields[j].type == TYPE_INT)
				{
					i_value = *((int *)v_buf);
					t[i][j].i_value = i_value;
				}
				*/
				//printfd2("value:%s\t", t[i][j].s_value);
				//printfd2("%s\t", t[i][j].s_value);

				cur_data_buf += fields[j].len;//跳到下一个字段的数据位置
			}
			//printfd("\r\n\r\n");



			//data_buf += (record_len-1);//跳到下一条记录的数据位置
			data_buf += record_len;//跳到下一条记录的数据位置
		}
	
	}


	//一次性的装载全部内容
	bool dbf::load_from_file_in_mem(std::string fn)
	{
		bool r = false;

		if (load_file_all_data(fn) == false)
		{
			return false;
		}

		if (load_from_mem() == false)
		{
			return false;
		}

		if (malloc_table(lines) == false)
		{
			return false;
		}

		buf_to_table(lines);

		return true;
	}



};


#endif
