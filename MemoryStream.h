#pragma once

#include <string>

//--------------------------------------------------
#ifndef NOMINMAX

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#endif  /* NOMINMAX */

//--------------------------------------------------

//������ C# , delphi ��ͬ����//�ڴ����ģʽΪ����ʱ�ɱ�����
class MemoryStream
{
public:
	char * buf;
	int buf_len;//Ϊ�˸��ٰ�ȫ��ʹ���ַ���,�����ڴ��Ƕ����һ���ֽڴ�� '\0'
	int read_pos;//��ǰ�����ж�ȡ�˶��ٸ��ֽ�

	//����
	void MemoryStream::write(const char * in_buf, int in_buf_len);

	//���
	void MemoryStream::clear(void);

	//��ȡһ���ֵ�����һ������
	int MemoryStream::read(MemoryStream & in_buf, int in_buf_len)
	{
		//int r = 0;

		int read_len = CanRead();
		read_len = min(read_len, in_buf_len);
		in_buf.write(buf + read_pos, read_len);

		read_pos += read_len;

		return read_len;
	}

	//��ȡһ���ֵ�������//���ƶ���ȡλ��
	int MemoryStream::read_no_seek(char * in_buf, int in_buf_len)
	{
		//int r = 0;

		int read_len = CanRead();
		read_len = min(read_len, in_buf_len);

		memcpy(in_buf, buf + read_pos, read_len);

		//read_pos += read_len;

		return read_len;
	}

	//��ȡһ���ֵ�������
	int MemoryStream::read(char * in_buf, int in_buf_len)
	{
		//int r = 0;

		int read_len = read_no_seek(in_buf, in_buf_len);

		read_pos += read_len;

		return read_len;
	}



	//���ж����ֽڿɶ�ȡ
	int MemoryStream::CanRead()
	{
		return this->buf_len - read_pos;
	}

	//��ȡλ������ƶ�
	void seek_for_read(int len)
	{
		read_pos += len;
	}

private:
	int buf_len_alloc;//buf_len ���Ѿ�ʹ�õĴ�С,�����Ԥ�ȷ���Ĵ�С

	//����Ƿ�Ҫд����ô������,���еĻ���Ҫ���·���
	void MemoryStream::check_space(int in_buf_len);

public:
	MemoryStream(void);

	~MemoryStream(void);

	//ָ����ʼ����С��
	MemoryStream(int init_buf_len_alloc);

public://�ڴ氲ȫ����//ǰ n ���ֽ����ڱ��泤�ȵ���Ϣ

	void * malloc(size_t size)
	{
		int add_len = sizeof(int) * 2;
		int tag = 1976;//��־
		int len = size;

		char * r = (char *)::malloc(size + add_len);

		char * p = r;
		::memcpy(p, &tag, sizeof(int));
		p += sizeof(int);

		::memcpy(p, &len, sizeof(int));


		return r + add_len;
	}

	void free(void * in_buf)
	{
		int add_len = sizeof(int) * 2;

		char * p = (char *) in_buf;
		p -= add_len;//������������ڴ�����

		//int tag = 1976;//��־
		//int len = size;
		int tag = 0;
		int len = 0;

		::memcpy(&tag, p, sizeof(int));
		p += sizeof(int);

		::memcpy(&len, p, sizeof(int));


		//--------------------------------------------------
		
		if (tag == 1976)//��־�Ե��ϲ����������ڴ�
		{
			p = (char *) in_buf;
			p -= add_len;//������������ڴ�����

			::free(p);
		}
		else
		{
			merror("������ڴ���Ч���߲����������.");
		}
	}

	//�����ڵ���
	void merror(const char * msg)
	{
		printf("merror:%s", msg);
	}

	//��ȫ��� memcpy //ֻЧ��д�����Ƿ�����Ч����
	void memcpy_safe(void *dest, const void *src, size_t count,
		void * buf_start_addr//���Ӵ���һ���ڴ������λ��// malloc() ���ú�õ����ڴ��ַ 
		)
	{
		if (buf == NULL)
		{
			merror("Ŀ���ڴ�黹δ����");
		}

		int add_len = sizeof(int) * 2;

		char * p = (char *) buf;
		p -= add_len;//������������ڴ�����

		//int tag = 1976;//��־
		//int len = size;
		int tag = 0;
		int len = 0;

		::memcpy(&tag, p, sizeof(int));
		p += sizeof(int);

		::memcpy(&len, p, sizeof(int));


		//--------------------------------------------------
		char * in_buf = (char *)buf_start_addr;
		
		if (tag == 1976)//��־�Ե��ϲ����������ڴ�
		{
			p = (char *) in_buf;
			p -= add_len;//������������ڴ�����

			//--------------------------------------------------
			//�ж���ʼλ���Ƿ�����Ч����Χ��
			char * dest_p = (char *)dest; 
			if (dest_p < in_buf)//��Ч��ǰ
			{
				merror("Ŀ���ڴ����ʼλ��Խ��");
			}
			else
			if (dest_p > (in_buf + len))//��Ч����
			{
				merror("Ŀ���ڴ����ʼλ��Խ��");
			}
			else//д������λ������Ч����//ͨ�����ڴ���������
			if ((dest_p + count) > (in_buf + len))
			{
				merror("Ŀ���ڴ�����λ��Խ��");
			}


			//else
			{
				::memcpy(dest, src, count);				
			}
		}
		else
		{
			merror("������ڴ���Ч���߲����������.");
		}
	
	}

};
