#pragma once

#include <string>

//���ַ������������������

class nstring//�����ַ�����//numeric string
{
public:
	nstring(void)
	{
		data = "";
	}

	~nstring(void);

private:

	std::string data;

	//����ת��Ϊ�ַ���
	std::string int_to_str(long value)
	{
		char s[256];
		memset(s, 0, sizeof(s));
		//ltoa(value,s,10);//�ƺ�������linux��ʹ��
		//sprintf(s, "%d", value);
		sprintf(s, "%ld", value);//������//sprintf(s, "%08X", 4567); //������"000011D7"

		std::string r = s;
		return r;
	}

	//����ת��Ϊ�ַ���
	std::string int_to_str(long value, int width)
	{
		char f[256];//��ʽ
		char s[256];
		memset(f, 0, sizeof(s));
		memset(s, 0, sizeof(s));

		//ltoa(value,s,10);//�ƺ�������linux��ʹ��
		//sprintf(s, "%d", value);
		sprintf(f, "%%0%dld", width);//��"%08ld"
		//sprintf(s, "%ld", value);//������//sprintf(s, "%08X", 4567); //������"000011D7"
		sprintf(s, f, value);//������//sprintf(s, "%08X", 4567); //������"000011D7"

		std::string r = s;
		return r;
	}

	//������ת��Ϊ�ַ���
	std::string float_to_str(double value, int width)
	{
		char f[256];//��ʽ
		char s[256];
		memset(f, 0, sizeof(s));
		memset(s, 0, sizeof(s));

		//�ο� http://www.cnblogs.com/wqlblogger/archive/2007/01/09/615525.aspx
		//sprintf(f, "%%*.*f");
		sprintf(f, "%%.*f");
		sprintf(s, f, width, value);

		std::string r = s;
		return r;
	}


	//������ת��Ϊ�ַ���
	std::string float_to_str(double value)
	{
		char f[256];//��ʽ
		char s[256];
		memset(f, 0, sizeof(s));
		memset(s, 0, sizeof(s));

		//�ο� http://www.cnblogs.com/wqlblogger/archive/2007/01/09/615525.aspx
		sprintf(f, "%%f");//Ĭ�ϵ�С����λ��Ϊ 6
		//sprintf(f, "%%*.*f");//�������
		sprintf(s, f, value);

		std::string r = s;
		return r;
	}


	int str_to_int(std::string v)
	{
		return atoi( v.c_str() );
	}//

public:



	//�������� operator ��������� (����˵��)
	//{
	////��������ڲ�ʵ��
	//} 

	std::string operator + (int v)
	{
		//��������ڲ�ʵ��
		int r = str_to_int(this->data) + v;

		return int_to_str(r);
	}//
	std::string operator - (int v)
	{
		//��������ڲ�ʵ��
		int r = str_to_int(this->data) - v;

		return int_to_str(r);
	}//

	nstring(std::string v)
	{
		data = v;
	}//

	////���� System::String ��ֵ�ĵط�//���� this->Caption = dstring;
	////���԰� System::String ����һ���������������//����д�Ϸ���ֵ
	//operator System::String();

	//���� System::String ��ֵ�ĵط�//���� this->Caption = dstring;
	//���԰� System::String ����һ���������������//����д�Ϸ���ֵ
	operator std::string &()//�� operator std::string () �Ƿ����������,������û���ٸ���һ��ʵ��?
	{
		return data;
	}

	operator const char *()
	{
		return data.c_str();
	}

};
