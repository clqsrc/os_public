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

//dbf��ͷ//ע��û���ֶ���Ŀ���ֶ�,��Ҫͨ���ֶ���Ϊ����ʾ������
typedef struct
{
	unsigned char  dbf_id;			//�������
									// dbf_id=03, memo field does not exist
									// dbf_id=83, memo field exists( foxbase )
									// dbf_id=F5, memo field exists ( foxpro )

	char   last_update[3];			//�޸�����// year,month,date of the file last_update
	long   rec_count;				//��¼��//4-7   32-bit      Number of records in the table.
	unsigned short data_offset;		//���ݵĿ�ʼ// the start address of the data item//Ҳ�г��� Number of bytes in the header. ��
	unsigned short one_rec_size;	//һ����¼ռ�����ֽ�,�������һλΪ����// the record size = all fields size + 1
	char   filler[20];				// unused
} DBF_HEAD;

//ÿ���ֶε���Ϣ//�ֶ���,���͵�//�����ļ��е�λ��Ӧ����������һ��
typedef struct
{
	char field_name[11];			//�ֶ���
	char field_type;				//�ֶ�����
	char dummy[4];					// unused
	//union
	//{
		//unsigned short wlen;		// when the field type is not numeric
		//struct
		//{
			unsigned char len;		//�ֶεĳ���
			unsigned char dec;
		//}N;						// when the field type is numeric
	//} linfo;
	char filler[14];				// unused
}FIELD_REC;

#pragma pack()//һ�����������,����ᵼ�·����ڴ����

class cell//��ʾһ������ֵ������
{

public://����
	//std::string  s_value;//�ַ���ֵ//��̬�����ڴ�̫��cpu
	char  s_value[255];//�ַ���ֵ
	int i_value;//����ֵ
	double f_value;//������ֵ
	bool type;//�����������������

};

enum DATA_TYPE 
{ TYPE_INT, TYPE_STRING, TYPE_FLOAT };

class field//��ʾһ���ֶ�
{

public://����
	std::string  name;
	int len;
	DATA_TYPE type;//�����������������

};


typedef std::vector<cell> line;//һ��
typedef std::vector<line> table;//���ű�

typedef std::vector<cell> dbf_line;//һ��
typedef std::vector<line> dbf_table;//���ű�

//dbf��ȡ����
class dbf
{

public://����

	keys config;//������Ϣ
	FILE * handle;
	DBF_HEAD head;
	FIELD_REC rec_info[256];
	int field_count;//�ֶ���
	int record_count;//��¼��
	int record_len;//һ����¼����������
	int file_len;
	char * file_buf;//�ļ�����
	std::string file_name;
	field fields[255];//�ֶ�
	table lines;//��ǰֵ
	//table old_lines;//�ϴ�ɨ���ֵ
	bool lock_file;//����dbfʱ�Ƿ������ļ�

public://����

	//���캯��
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
			printf("dbf�ļ�[%s]��ʧ��!\r\n", fn.c_str());
			return false;
		}
		fread(&head, 1, sizeof(head), handle);

		record_count = head.rec_count;

		field_count = 0;
		record_len = 0;
		//�ֶ���Ϣ���ж����
		while(true)
		{
			fread(&rec_info[field_count], 1, sizeof(FIELD_REC), handle);

			if(rec_info[field_count].field_name[0] == 0x0d)	//�ֶν���
			break;

			fields[field_count].name = rec_info[field_count].field_name;//�ֶ���
			fields[field_count].len = rec_info[field_count].len;//�ֶγ���

			//У�鳤��
			if ( (fields[field_count].len > 250) || (fields[field_count].len < 0) )
			{
				while(true)
					printf("dbf �ֶγ��ȴ��� dbf::load_from_file().\r\n");
			}

			/*//�����ж���,�����ַ���
			//��������
			if (rec_info[field_count].field_type)
			{
				fields[field_count].type = TYPE_INT;
			}
			*/

			record_len += rec_info[field_count].len;//�����ֶγ���
			field_count++;//�����ֶ���

		}

		record_len += 1;//ÿ����¼ǰ����һ��ɾ����ʶ

		fclose(handle);

		//�ļ�����
		file_len = file_system::get_file_length(fn.c_str());
		//file_buf = new char(file_len);

		if (file_buf != NULL) 
			free(file_buf);

		file_buf = (char *)malloc(file_len);

		if (file_buf == NULL)
		{
			while (true)
				printf("�ڴ治��,dbf::load_from_file()[%d,%s]\r\n", file_len, fn.c_str());
		}

		//��¼���п����Ǵ��//Ҫ����//���� pb �ṩ�� dbf �����������
		record_count = (file_len - sizeof(DBF_HEAD) - field_count * sizeof(FIELD_REC))/record_len;

		//Ϊ�˰�ȫ���
		if (record_count < head.rec_count)
		{
			record_count = record_count - 1;
		}

		//��֮��Ҫȡ��Сֵ
		if (record_count > head.rec_count)
		{
			record_count = head.rec_count;
		}

		return true;

	}


	bool dbf::read_to_buf()//��ȡ�����ļ����ڴ���//�ļ���200K����
	{
		handle = fopen(file_name.c_str(), "rb");
		if (handle == NULL)
		{
			printf("dbf�ļ�[%s]��ʧ��!", file_name.c_str());
			return false;
		}

		int r = fread(file_buf, 1, file_len, handle);

		if (r != file_len)
			return false;

		fclose(handle);
		return true;
	}

	//���ڴ��ж�
	bool dbf::load_from_mem()
	{
		char * p = file_buf;

		memcpy(&head, p, sizeof(head));
		p += sizeof(head);

		record_count = head.rec_count;

		field_count = 0;
		record_len = 0;
		//�ֶ���Ϣ���ж����
		while(true)
		{
			//fread(&rec_info[field_count], 1, sizeof(FIELD_REC), handle);
			memcpy(&rec_info[field_count], p, sizeof(FIELD_REC));
			p += sizeof(FIELD_REC);


			if(rec_info[field_count].field_name[0] == 0x0d)	//�ֶν���
			break;

			fields[field_count].name = rec_info[field_count].field_name;//�ֶ���
			fields[field_count].len = rec_info[field_count].len;//�ֶγ���

			//У�鳤��
			if ( (fields[field_count].len > 250) || (fields[field_count].len < 0) )
			{
				while(true)
					printf("dbf �ֶγ��ȴ��� dbf::load_from_file().\r\n");
			}

			/*//�����ж���,�����ַ���
			//��������
			if (rec_info[field_count].field_type)
			{
				fields[field_count].type = TYPE_INT;
			}
			*/

			record_len += rec_info[field_count].len;//�����ֶγ���
			field_count++;//�����ֶ���

		}

		record_len += 1;//ÿ����¼ǰ����һ��ɾ����ʶ

		//��¼���п����Ǵ��//Ҫ����//���� pb �ṩ�� dbf �����������
		//record_count = (file_len - field_count * sizeof(FIELD_REC))/record_len;
		record_count = (file_len - sizeof(DBF_HEAD) - field_count * sizeof(FIELD_REC))/record_len;

		//Ϊ�˰�ȫ���
		if (record_count < head.rec_count)
		{
			record_count = record_count - 1;
		}

		//��֮��Ҫȡ��Сֵ
		if (record_count > head.rec_count)
		{
			record_count = head.rec_count;
		}

		//fclose(handle);


		return true;

	}

	//��ȡȫ������
	bool dbf::load_file_all_data(std::string fn)
	{

		file_name = fn;

		handle = fopen(fn.c_str(), "rb");
		if (handle == NULL)
		{
			printf("dbf�ļ�[%s]��ʧ��!\r\n", fn.c_str());
			return false;
		}

		if (lock_file == true)
		{
			//�����ļ�
			file_system::lock_file(handle);
		}

		//�ļ�����
		file_len = file_system::get_file_length(handle);

		if (file_len < 1)
		{
			printf("dbf�ļ�[%s]��ʧ��!δȡ���ļ����ȡ�\r\n", fn.c_str());

			file_system::unlock_file(handle);
			return false;
		}


		if (file_buf != NULL) 
			free(file_buf);

		file_buf = (char *)malloc(file_len);

		if (file_buf == NULL)
		{
			while (true)
				printf("�ڴ治��,dbf::load_from_mem()\r\n");
		}


		int r = fread(file_buf, 1, file_len, handle);

		if (lock_file == true)
		{
			//�����ļ�//������ windows �²������Ҳ�ǿ��Եģ��ļ�����ʱ�Զ�����?
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


	//�����ڴ�
	bool dbf::malloc_table(table & t)
	{
		int i = 0;

		try
		{

			t.resize(record_count);//���ü�¼��//��¼��̫���ǲ��е� ����538968845

			for (i=0; i<record_count; i++)
			{
				t[i].resize(field_count);//�����ֶ���

			}

		}
		catch(...)
		{
			std::string err = "��СԽ��! dbf::malloc_table()[" +int_to_str(record_count)+ "][" +int_to_str(field_count)+ "]\r\n";
			printf_error(err.c_str());
		}

		return true;

	}

	//У�鳤��//��ǰ��ȡλ���Ƿ񳬹��ļ�����
	bool dbf::check_point(char * p)
	{
		if (p > file_buf + file_len)
		{
			printf_error(("�ڴ�Խ�� dbf::check_point()file_len:" +int_to_str(file_len)+ ".\r\n").c_str());
			return true;
		}
		else
		{
			return false;
		}
	}


	//��ȡ���ڴ��ļ����������б�ʾ
	void dbf::buf_to_table(table & t)
	{
		int i,j;
		//std::vector<int> table1;
		std::string s;
		char v_buf[255];//
		char * cur_data_buf;
		//int i_value;
		//s.resize();

		char * data_buf = file_buf + head.data_offset;//����λ��
		
		//����,̫��CPU//t.resize(record_count);//���ü�¼��

		for (i=0; i<record_count; i++)
		{
			//����,̫��CPU//t[i].resize(field_count);//�����ֶ���

			//У�鳤��
			check_point(data_buf + record_len);

			cur_data_buf = data_buf +1;
			//data_buf += 1;//��һ����ɾ����־,���Ի�Ҫ��������

			for (j=0; j<field_count; j++)
			{

				//У�鳤��
				check_point(cur_data_buf);

				int min_len = 0;//��ȫ����С����
				min_len = sizeof(v_buf);
				if (fields[j].len < min_len) min_len = fields[j].len;
				if (sizeof(t[i][j].s_value) < min_len) min_len = sizeof(t[i][j].s_value);

				memset(&v_buf, 0, sizeof(v_buf));

				//memcpy(&v_buf, cur_data_buf, fields[j].len);
				//t[i][j].s_value = v_buf;//ȡ��һ����Ԫ���ֵ//��̬�����ڴ�̫��cpu
				//strcpy(t[i][j].s_value, v_buf);//ȡ��һ����Ԫ���ֵ//��̬�����ڴ�̫��cpu

				memcpy(&v_buf, cur_data_buf, min_len);
				//ǰ���� memset �����ǿ����� strcpy ��//strncpy(t[i][j].s_value, v_buf, );//ȡ��һ����Ԫ���ֵ//��̬�����ڴ�̫��cpu
				strcpy(t[i][j].s_value, v_buf);//ȡ��һ����Ԫ���ֵ//��̬�����ڴ�̫��cpu

				/*//�����ж���,�����ַ���
				if (fields[j].type == TYPE_INT)
				{
					i_value = *((int *)v_buf);
					t[i][j].i_value = i_value;
				}
				*/
				//printfd2("value:%s\t", t[i][j].s_value);
				//printfd2("%s\t", t[i][j].s_value);

				cur_data_buf += fields[j].len;//������һ���ֶε�����λ��
			}
			//printfd("\r\n\r\n");



			//data_buf += (record_len-1);//������һ����¼������λ��
			data_buf += record_len;//������һ����¼������λ��
		}
	
	}


	//һ���Ե�װ��ȫ������
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
