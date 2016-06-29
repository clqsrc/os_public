
#include "stdafx.h"

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include "file_system.h"
#include "public_function.h"

//--------------------------------------------------
//���Ҫ�� md5.h
#ifdef __USE_MD5__
#include "md5.h"
#endif
//--------------------------------------------------

//#include "md5.h"
#include <math.h>

//#include <gd.h>
//#include <gdfontg.h>

extern FILE * log_file;//��־�ļ�


/* ���ú���/���ú��� */

//�õ��ļ�·��//��������[/]
std::string extract_file_path(const std::string fn)
{
	std::string s = "";
	char c = '\0';

	int count = 0;
	for (int i = 0; i < fn.size(); i++)
	{
		c = fn[fn.size()-i-1];

		count++;

		if (c == '\\' || c == '/')
			break;

		//s = c + s;
		//count++;
	}

	s = fn.substr(0, fn.length()-count);

	return s;
}


//�õ� mysql ��ʽ�ĵ�ǰʱ��
std::string GetMysqlDateTime()
{
	std::string s = "";

	tm m = GetLocalTM();

	s += int_to_str(m.tm_year+1900)  + "-";
	s += int_to_str(m.tm_mon+1) + "-";
	s += int_to_str(m.tm_mday);
	s += " ";
	s += int_to_str(m.tm_hour) + ":";
	s += int_to_str(m.tm_min)  + ":";
	s += int_to_str(m.tm_sec);

	return s;
}

//��һ������д����ļ�// log ���͵��ص���ļ�
FILE * open_log_file(const std::string fn)
{
	FILE * f = fopen(fn.c_str(), "a+b");
	if(f == NULL)
	{
		printf("%s", "error! log file can't open!\r\n");
		return NULL;
	}
	setbuf(f, NULL);//����д���ļ�

	return f;

}

//�õ����ص� tm �ṹ
tm GetLocalTM()
{
	time_t t = time(NULL);
	tm * pm = NULL;
	tm m;
	memset(&m, 0, sizeof(m));

	pm = localtime(&t);

	if (pm != NULL)
	{
		m = (* pm);
	}
	

	return m;
}


//�õ� gm �� tm �ṹ
tm GetTM()
{
	time_t t = time(NULL);
	tm * pm = NULL;
	tm m;
	memset(&m, 0, sizeof(m));

	pm = gmtime(&t);

	m = (* pm);

	return m;
}


//ȡ�õ�ǰʱ��
int GetTimeZone()
{
	time_t t = 0;//time(NULL);
	tm * m = NULL;

	tm gtm;//��������
	tm ltm;//����
	memset(&gtm, 0, sizeof(tm));
	memset(&ltm, 0, sizeof(tm));


	m = gmtime(&t);
	if (m == NULL) return 0;
	gtm = *m;

	m = localtime(&t);
	if (m == NULL) return 0;
	ltm = *m;


	int r = ltm.tm_hour - gtm.tm_hour;

	return r;
}

//mktime �ǰ� tm �е�ʱ�䵱������ʱ�����õ���������ʱ���,����Ҫ�� tm �е�ʱ��ֱ�ӵ�����������ʱ������//�㷨�Ͼ���Ҫ����ʱ�������
time_t gm_mktime(struct tm * m)
{
	time_t t = mktime(m);//��ʱ�� t ��Ҫ�� 8 ��Сʱ��,Ҫ����8Сʱ��ʱ��//����������������˵��ô������ʱ����.

	//t = 8 * 60 * 60;

	int iTimeZone = GetTimeZone();

	t = t + iTimeZone * 60 * 60;

	return t;
}


//�Ƚϸ������Ƿ����//�������ǲ���ֱ��д�ȺűȽϵģ�Ҫһ�����ȷ�Χ�ڲ���
bool float_cmp(const float f1, const float f2)
{
	bool r = false;

	if (fabs(f1 - f2) < 0.000001)
	{
		r = true;
	}

	return r;
}

//���ļ���װ������//�����Ҫ�ͷŷ��ص��ַ����ڴ澡���ú�����Ǹ�
char * load_from_file(const char * fn, long & len)
{
	if ((fn == NULL)||(strlen(fn) == 0))
	{
		return NULL;
	}

	len = file_system::get_file_length(fn);

	if (0 >= len) return NULL;

	//char * buf = new char[len];
	char * buf = (char *)malloc(len);//ͳһ���� malloc/free
	if (buf == NULL)
	{
		return NULL;
	}


	memset(buf, 0, len);
	//BYTE buf[50000];
	FILE * f = fopen(fn, "rb");
	if (f == NULL)
	{
		free(buf);
		return NULL;
	}

	long r = fread(buf, 1, len, f);//�����fread(buf, len, 1, f)�����������buf�㹻���ʱ�����ǶԵģ�

	if (r != len)
	{
		//printf("error at ftp_client_socket::load_from_file!\r\n");
		free(buf);
		fclose(f);
		return NULL;
	}

	fclose(f);

	return (char *)buf;
}



std::string load_from_file(const char * fn)//ע��,����ķ���ֵ�ǲ��� delete��
{
	long len;
	char * buf = load_from_file(fn, len);

	if (buf == NULL)
		return "";

	std::string s(buf, len);

	//delete [] buf;
	free(buf);//ͳһ���� malloc/free
	return s;
}

void load_from_file(lines & l, const char * fn)
{
	std::string buf = load_from_file(fn);

	load_from_string(l, buf);
}

void load_from_string(lines & l, std::string & buf)
{
	//std::string buf = load_from_file(fn);
	std::string key = "";
	std::string value = "";
	std::string line = "";

	for (long i=0; i<buf.size(); i++)
	{
		if (buf[i]=='\r') continue;
		if (buf[i]=='\n')
		{
			l.push_back(line);
			line = "";

			continue;
		}
		line += buf[i];
	}

	//����\n֮���������
	if (line.size() > 0)
	{
		l.push_back(line);
		line = "";
	}

}

void load_from_file(lines & l, const char * fn, char sp)
{
	std::string buf = load_from_file(fn);

	load_from_string(l, buf, sp);
}


void load_from_string(lines & l, std::string & buf, char sp)
{
	//std::string buf = load_from_file(fn);
	//std::string key = "";
	//std::string value = "";
	std::string line = "";

	for (long i=0; i<buf.size(); i++)
	{
		//if (buf[i]=='\r') continue;
		if (buf[i] == sp)
		{
			l.push_back(line);
			line = "";

			continue;
		}
		line += buf[i];
	}

	//����\n֮���������
	if (line.size() > 0)
	{
		l.push_back(line);
		line = "";
	}

}

//����һ�����������ݵ��ļ���
bool save_to_file(const char * fn, const char * buf, long len)
{
	//д���ļ�
	FILE * f = fopen(fn, "wb");
	if (f == NULL)
	{
		//fclose(f);//���� �ļ���û��
		return false;
	}
	long r = fwrite(buf, 1, len, f);

	if (r != len)
	{
		fclose(f);
		return true;
	
	}

	fclose(f);
	return true;
}

//����һ�����������ݵ��ļ���
bool save_to_file(const std::string fn, const std::string buf)
{
	//д���ļ�
	return save_to_file(fn.c_str(), buf.data(), buf.size());
}

//����һ�����������ݵ��ļ���
bool save_to_file(const std::string fn, lines & l)
{
	std::string buf = "";

	for (int i = 0; i < l.size(); i++)
	{
		buf += l[i] + "\r\n";
	}

	//д���ļ�
	return save_to_file(fn.c_str(), buf.data(), buf.size());
}


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
}

//ģ�庯��
std::string get_template_string(std::string s, keys & k)
{
	std::string r = "";
	std::string key = "";
	std::string value = "";

	keys::iterator iter;
	
	for (iter = k.begin(); iter != k.end();)
	{
		
		key = iter->first;
		value = iter->second;
		
		s = str_replace(s, key, value);
		
		iter++;
		
	}

	//s = str_replace(s, key, value);
	

	return s;

}

std::string get_template_string_from_file(std::string fn, keys & k)
{
	std::string r = "";
	std::string fs = load_from_file(fn.c_str());
	
	r = get_template_string(fs, k);
	
	return r;
	
}

//ģ�庯��
std::string make_string_3(const std::string src, std::string sp_begin1, std::string sp_end1, std::string & l, std::string & r)
{
	std::string m = "";
	
	std::string in1 = lcase(src);//��in1�������ַ�����λ��,����ȡʱ��Ȼ���ʵ���src//�������Ժ��Դ�Сд�ֲ���ԭ�е�����
	sp_begin1 = lcase(sp_begin1);
	sp_end1 = lcase(sp_end1);
	
	
	int pos_b1,pos_e1; //Ҫ�õ����ַ�������βλ��
	
	//����pos_b1
	if (sp_begin1.length()==0)//�����1���ָ���Ϊ�վͱ�ʾȡ��2���ָ���֮ǰ���ַ���
	{
        pos_b1=0;
	}
	else//��һ���ָ�����Ϊ��ʱ
	{
		pos_b1=in1.find(sp_begin1);
		if (pos_b1 == -1) return "";//��һ���ָ�����Ϊ�գ������Ҳ���ʱ����ֵ���ǿ�
		
		pos_b1 += sp_begin1.length();//��Ҫ���Ϸָ�������ĳ���
	}
	
	//�Ӻ�pos_b1��ʼ��pos_e1
	pos_e1=in1.find(sp_end1, pos_b1);
	if (sp_end1.length()==0)//�����2���ָ���Ϊ�վͱ�ʾȡ��1���ָ���֮����ַ���
	{
        pos_e1=in1.length();
	}
	
	//��������ַ�����������,�򷵻ؿմ�
	if (
		((pos_b1==-1)&&(pos_e1==-1))
		||((pos_b1==-1)&&(sp_begin1.size() > 0))
		||((pos_e1==-1)&&(sp_end1.size() > 0))
		)
	{
        return "";
	}
	
	//r = src.substr(pos_b1, pos_e1-pos_b1);
	m = substring_JAVA(src, pos_b1, pos_e1);
	l = substring_JAVA(src, 0, pos_b1);
	r = substring_JAVA(src, pos_e1, src.length());
	
	return m;
	
	
	
	return m;
	
}

//�滻�ַ���
std::string str_replace(const std::string s, const std::string old_s, const std::string new_s)
{
	size_t pos = 0;
	std::string r = s;
	while(true)
	{
		pos = r.find(old_s, pos);
		if (pos == std::string::npos) break;

		//c++��replace������������������ͬ��
		//r.replace(pos, pos + old_s.length(), new_s);
		//ע��ڶ�������������
		r.replace(pos, old_s.length(), new_s);
		pos += new_s.length();//����λ��Ҫ������
	}
  
	return r;
}



std::string trim(std::string s)
{
	if (s.size()==0) return "";
	//û�뵽��ͦ��д
	//��Ϊ����int ibpos = strbuf[i].find_first_not_of(" "); int iepos = strbuf[i].find_last_not_of(" ");ʵ��
	size_t ibpos = 0;
	size_t iepos = s.size();
	char c;
	size_t i;

	for (i = 0; i<s.size(); i++)
	{
		c = s[i];
		if ((c == ' ')||(c == '\0')||(c == '\t')||(c == '\r')||(c == '\n'))
		{
			continue;
		}
		ibpos = i;
		break;
	}

	for (i = s.size()-1; ; i--)//�����s.size()����Ϊ0,�������ѭ��
	{
		c = s[i];
		if ((c == ' ')||(c == '\0')||(c == '\t')||(c == '\r')||(c == '\n'))
		{
			iepos = i;
			if (i==0) break;
			continue;
		}
		break;
	}

	return s.substr(ibpos, iepos - ibpos);

}

//����ҪЧ�ʵĵط����� trim//ȥ���ұߵĿո�
void trimr(char * buf, int buf_len)
{
	char c;
	int i = 0;

	for (i = buf_len-1; ; i--)//�����s.size()����Ϊ0,�������ѭ��
	{
		c = buf[i];
		if ((c == ' ')||(c == '\0')||(c == '\t')||(c == '\r')||(c == '\n'))
		{
			buf[i] = '\0';
			if (i==0) break;
			continue;
		}
		break;
	}


}

//����ҪЧ�ʵĵط����� std::string ��ʵ��
void lcase(char * buf, int buf_len)
{
	for (int i=0; i < buf_len; i++)
	{
		buf[i] = tolower(buf[i]);
	}

}

//Сд//ע��������ַ���������ȫ
std::string lcase(std::string s)
{
	std::string r = "";
	for (int i=0;i<s.size();i++)
	{
		r += tolower(s[i]);
	}
	return r;
}

//����ҪЧ�ʵĵط����� std::string ��ʵ��
void ucase(char * buf, int buf_len)
{
	for (int i=0; i < buf_len; i++)
	{
		buf[i] = toupper(buf[i]);
	}

}


//��д//ע��������ַ���������ȫ
std::string ucase(std::string s)
{
	std::string r = "";
	for (int i=0;i<s.size();i++)
	{
		r += toupper(s[i]);
	}
	return r;

}

//����//[��Сд������]
//std::string uncase_find(std::string s1, std::string s2)
bool ufind(std::string s, std::string sub_s)
{
	//
	if (lcase(s).find(lcase(sub_s)) != std::string::npos)
	{
		return true;
	}

	return false;

}

//��java��substring��ʵ�ֵ�,��Ҫ��Ϊjava�Ĵ�����ֲ��C++��,����VC��std::string��substr�з��ʵĲ���Խ��Ļ����쳣[�����Ϊ���ؿ��ַ���]
std::string substring_JAVA(std::string s, int beginIndex, int endIndex)
{
	std::string r = "";
    r = substring_STL(s, beginIndex, endIndex-beginIndex);//java�е�substring�ĵ�2��������ָ��ֹ�ַ���λ��;��stl������Ҫ���Ƶĳ���,����������̫��ͻ��쳣

	return r;
}

//����VC��STL�쳣,�μ�substring_JAVA��˵��
std::string substring_STL(std::string s, int pos, int n)
{
	std::string r = "";

	//�����ʼλ��̫��,��ֱ�ӷ��ؿ�
	if (s.size()<=pos)
	{
		return "";
	}

	//�����ʼλ��̫��,��ֱ�ӷ��ؿ�
	if (pos<0)
	{
		pos = 0;
	}

	//�����ֹ��ʼλ��̫��,��ֱ��ȡ�����ַ�λ��
	if (s.size() <= pos+n )
	{
		n = s.size()-pos;
	}
    r = s.substr(pos, n);//vc��stl��pos_b1����С��pos_e1//java�е�substring�ĵ�2��������ָ��ֹ�ַ���λ��;��stl������Ҫ���Ƶĳ���,����������̫��ͻ��쳣

	return r;
}

//����VC��STL�쳣,�μ�substring_JAVA��˵��
std::wstring substring_STL(std::wstring s, int pos, int n)
{

	//std::wstring r = NULL;
	std::wstring r;
	r.resize(0);//��ֲ���ֱ��д r = "";

	//�����ʼλ��̫��,��ֱ�ӷ��ؿ�
	if (s.size()<=pos)
	{
		return r;
	}

	//�����ʼλ��̫��,��ֱ�ӷ��ؿ�
	if (pos<0)
	{
		pos = 0;
	}

	//�����ֹ��ʼλ��̫��,��ֱ��ȡ�����ַ�λ��
	if (s.size() <= pos+n )
	{
		n = s.size()-pos;
	}
    r = s.substr(pos, n);//vc��stl��pos_b1����С��pos_e1//java�е�substring�ĵ�2��������ָ��ֹ�ַ���λ��;��stl������Ҫ���Ƶĳ���,����������̫��ͻ��쳣

	return r;
}


//����http://www.alhem.net/Sockets/html/Utility_8cpp-source.html//ȥ��IE��UTF8��������ʹ��
 std::string rfc1738_decode_old(const std::string & src)
 {
         std::string dst;
         for (size_t i = 0; i < src.size(); i++)
         {
                 if (src[i] == '%' && isxdigit(src[i + 1]) && isxdigit(src[i + 2]))
                 {
                         char c1 = src[i + 1];
                         char c2 = src[i + 2];
                         c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
                         c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
                         dst += (char)(c1 * 16 + c2);
                 }
                 else
                 if (src[i] == '+')
                 {
                         dst += ' ';
                 }
                 else
                 {
                         dst += src[i];
                 }
         }
         return dst;
 } // rfc1738_decode

 std::string rfc1738_decode(const std::string & src)
 {
         std::string dst;
         //for (size_t i = 0; i < src.size(); i++)
		 size_t i = 0;
		 while(i < src.size())
         {
                 //if (src[i] == '%' && isxdigit(src[i + 1]) && isxdigit(src[i + 2]))
				 //if (false)
                 if (src[i] == '%')
                 {
                         if ((i+2) >= src.size()) break;//������ȫЩ
                         char c1 = src[i + 1];
                         char c2 = src[i + 2];
                         c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
                         c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
						 //����������Ҳ����,����ֻ���Ǵ�д
                         //c1 = c1 - '0';
                         //c2 = c2 - '0';
                         dst += (char)(c1 * 16 + c2);
						 i += 3;
                 }
                 else
                 if (src[i] == '+')
                 {
                         dst += ' ';
						 i += 1;
                 }
                 else
                 {
                         dst += src[i];
						 i += 1;
                 }
         }
         return dst;
 } // rfc1738_decode

 std::string rfc1738_encode(const std::string& src)
 {
 static  char hex[] = "0123456789ABCDEF";
         std::string dst;
         for (size_t i = 0; i < src.size(); i++)
         {
                 if (isalnum(src[i]))
                 {
                         dst += src[i];
                 }
                 else
                 if (src[i] == ' ')
                 {
                         dst += '+';
                 }
                 else
                 {
                         dst += '%';
                         dst += hex[src[i] / 16];
                         dst += hex[src[i] % 16];
                 }
         }
         return dst;
 } // rfc1738


 //�����ִ�Сд
  std::string get_value1_old(std::string src, std::string sp_begin1, std::string sp_end1)
  {
	  //return "";
	  ///*
	  std::string in1 = lcase(src);//��in1�������ַ�����λ��,����ȡʱ��Ȼ���ʵ���src
	  sp_begin1 = lcase(sp_begin1);
	  sp_end1 = lcase(sp_end1);

	  std::string fs1 = "";

      int pos_b1,pos_e1; //Ҫ�õ����ַ�������βλ��

      //�Ȱ���һ���ַ�����ȡ�ַ�//fs1Ϊpos_b1����ַ���
      pos_b1=in1.find(sp_begin1)+sp_begin1.length();
      if (sp_begin1.length()==0)
      {
        pos_b1=0;
      }
      pos_e1=in1.length();
      //fs1=in1.substr(pos_b1, pos_e1-pos_b1);//vc��stl��pos_b1����С��pos_e1//java�е�substring�ĵ�2��������ָ��ֹ�ַ���λ��;��stl������Ҫ���Ƶĳ���,����������̫��ͻ��쳣
	  fs1 = substring_JAVA(in1, pos_b1, pos_e1);
      //..end;

      //Ȼ���ٽ�ȡ������ַ���sp_end1ǰ�Ĳ���
      pos_e1=fs1.find(sp_end1);
      pos_b1=0; //��ʼλ�ñ��뻹ԭΪ��
      if (sp_end1.length()==0)
      {
        pos_e1=in1.length();
      }

      //��������ַ�����������,�򷵻ؿմ�
      if ((pos_b1==-1)&&(pos_e1==-1))
      {
        return "";
      }

      //fs1=fs1.substr(pos_b1, pos_e1-pos_b1);
	  fs1 = substring_JAVA(fs1, pos_b1, pos_e1);

      return fs1;
      //*/
  }

  std::string get_value1(std::string src, std::string sp_begin1, std::string sp_end1)
  {
	  std::string in1 = lcase(src);//��in1�������ַ�����λ��,����ȡʱ��Ȼ���ʵ���src//�������Ժ��Դ�Сд�ֲ���ԭ�е�����
	  sp_begin1 = lcase(sp_begin1);
	  sp_end1 = lcase(sp_end1);

	  std::string r = "";

      int pos_b1,pos_e1; //Ҫ�õ����ַ�������βλ��

      //����pos_b1
      if (sp_begin1.length()==0)//�����1���ָ���Ϊ�վͱ�ʾȡ��2���ָ���֮ǰ���ַ���
      {
        pos_b1=0;
      }
	  else//��һ���ָ�����Ϊ��ʱ
	  {
		pos_b1=in1.find(sp_begin1);
		if (pos_b1 == -1) return "";//��һ���ָ�����Ϊ�գ������Ҳ���ʱ����ֵ���ǿ�

		pos_b1 += sp_begin1.length();//��Ҫ���Ϸָ�������ĳ���
	  }

      //�Ӻ�pos_b1��ʼ��pos_e1
      pos_e1=in1.find(sp_end1, pos_b1);
      if (sp_end1.length()==0)//�����2���ָ���Ϊ�վͱ�ʾȡ��1���ָ���֮����ַ���
      {
        pos_e1=in1.length();
      }

      //��������ַ�����������,�򷵻ؿմ�
      if (
		    ((pos_b1==-1)&&(pos_e1==-1))
		  ||((pos_b1==-1)&&(sp_begin1.size() > 0))
		  ||((pos_e1==-1)&&(sp_end1.size() > 0))
		  )
      {
        return "";
      }

      //r = src.substr(pos_b1, pos_e1-pos_b1);
	  r = substring_JAVA(src, pos_b1, pos_e1);

      return r;

  }


void load_keys_buf1(keys & k, std::string & buf)
{
	std::string key = "";
	std::string value = "";
	std::string line = "";

	lines ls;
	load_from_string(ls, buf);
	for (int i=0; i<ls.size(); i++)
	{
		line = ls[i];

		if( (line.size() > 0) && (line[0]=='#') ) continue;//����ע��

		key = lcase(get_value1(line, "", "="));
		value = get_value1(line, "=", "");

		k[key] = value;

		continue;

	}
}

//���ļ��м��ؼ�ֵ��//��apacheһ��[#]�ſ�ʼ����Ϊע��
void load_keys1(keys & k, const std::string fn)
{
	std::string key = "";
	std::string value = "";
	std::string line = "";

	lines ls;
	load_from_file(ls, fn.c_str());
	for (int i=0; i<ls.size(); i++)
	{
		line = ls[i];

		if( (line.size() > 0) && (line[0]=='#') ) continue;//����ע��

		key = get_value1(line, "", "=");;
		value = get_value1(line, "=", "");

		k[key] = value;

		continue;

	}
	
}

//�����ֵ��
void save_keys1(keys & k, const std::string fn)
{
	std::string key = "";
	std::string value = "";
	std::string line = "";

	std::string buf = "";

	keys::iterator iter;

	for (iter = k.begin(); iter != k.end();)
	{

		key = iter->first;
		value = iter->second;

		buf += key + "=" + value + "\r\n";

		iter++;

	}

	save_to_file(fn, buf);
	
}


//�ж�ĳ�����Ƿ����б���
bool key_in_keys(std::string key, keys & list)
{
	std::map<std::string, std::string>::iterator iter;

	iter = list.find(key);
	if(iter != list.end())
	{
		return	true;
	}
	
	return false;
	
}

//���� NULL ��ֵ����,���ü�����
std::string safe_std_string(const char * str)
{
	std::string r = "";
	r = str;

	return r;
}

std::string safe_std_string(const std::string str)
{
	std::string r = "";

	r = str.c_str();

	return r;

}

std::string safe_std_string(keys & list, const std::string key)
{
	std::string value = "";

	std::map<std::string, std::string>::iterator iter;

	iter = list.find(key);
	if(iter != list.end())
	{	
		value = iter->second;
		//break;
	}
	
	return value;
}

//ֱ����[]ȡֵ�ǲ���ȫ��,����Ҫ��һ������
bool get_value(std::map<std::string, std::string> list, std::string key, std::string & value)
{
	std::map<std::string, std::string>::iterator iter;

	iter = list.find(key);
	if(iter != list.end())
	{	
		value = iter->second;
		return	true;
	}
	
	return false;
}

std::string get_value(std::map<std::string, std::string> list, std::string key)
{
	std::string s;
	if (get_value(list, key, s)==false)
		return "";

	return s;
}


std::string str_to_hex(std::string s)
{
	char buf[250];
	memset(buf, 0, sizeof(buf));

	std::string r = "";
	char c;
	for(int i=0; i<s.size(); i++)
	{
		c = s[i];
		//sprintf(buf, "%X", (byte)c);
		sprintf(buf, "%02X", (unsigned char)c);//Ҫ��������У�Ҫ�� 1 ��ת��Ϊ "1" ����������Ҫ��� "01"
		r += buf;
	}

	return r;

	//return ucase(r);
}

//
std::string buf_to_hex(const char * s, int len)
{
	char buf[250];
	memset(buf, 0, sizeof(buf));

	std::string r = "";
	char c;
	for(int i=0; i<len; i++)
	{
		c = s[i];
		//sprintf(buf, "%X", (byte)c);
		sprintf(buf, "%02X", (unsigned char)c);
		r += buf;
	}

	return r;

	//return ucase(r);
}

//['A']=>10
int hexchar_to_int(char c)
{
	if(c<='9'&&c>='0')
		return int(c)-48;

	if(c<='Z'&&c>='A')
		return int(c)-55;

	if(c<='z'&&c>='a')
		return int(c)-87;

	return -1;
}

//["63"]=>["c"]
std::string hex_to_str(std::string s)
{
	if ((s.size() % 2)!=0)//������2�ı���
	{
		printfd2("error at hex_to_str():%s\r\n", s.c_str());
		return "";
	}

	char buf[3];
	memset(buf, 0, sizeof(buf));

	std::string r = "";
	char c1,c2;
	int i1,i2;
	for(int i=0; i<s.size(); )
	{
		c1 = s[i];
		c2 = s[i+1];
		i1 = hexchar_to_int(c1);
		i2 = hexchar_to_int(c2);

		if ((i1==-1)||(i2==-1)) 
		{
			printfd2("error at hex_to_str()2:%s\r\n", s.c_str());
			return "";		
		}
		
		
		r += ( (char)(i1*16 + i2) );

		i += 2;
	}

	return r;
	
}

//����Ƿ��ǰ�ȫ���ַ���//�»���,����,��ĸ
bool check_safe_str(std::string s)
{
	char c1;
	for(int i=0; i<s.size(); i++)
	{
		c1 = s[i];

		if (c1=='.') return false;
		if (c1=='_') return true;
		if( (c1>='0' && c1<='9')||(c1>='a' && c1<='z')||(c1>='A' && c1<='Z') )
		{
			return true;
		}

	}

	return true;
	
}

//�ַ����Ƿ���ͬ[��Сд������]
bool uncase_cmp(const char * buf,const char * cmd)
{
	//������ַ��������ǲ�����
	if (strlen(buf) < strlen(cmd)) return false;

	//ÿ���ַ����Ƚ�һ��
	for (int i=0;i<strlen(cmd);i++)
	{
		//��һ���ַ���һ�����ǲ�ͬ��
		if (tolower(buf[i])!=tolower(cmd[i]))
		{
			return false;
		}
	}
	return true;
}

bool str_eq(std::string s1, std::string s2)
{
	if(s1.compare(s2) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//���Դ�Сд�ıȽ�
bool eqIgnoreCase(const std::string s1, const std::string s2)
{
	if(lcase(s1).compare(lcase(s2)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//�õ��ļ���׺��
std::string extract_file_ext(const std::string fn)
{
	size_t pos = fn.rfind(".");
	if (pos == std::string::npos) return "";

	return fn.substr(pos);
}

//�õ��ļ���
std::string extract_file_name(const std::string fn)
{
	std::string s = "";
	char c = '\0';

	for (int i = 0; i < fn.size(); i++)
	{
		c = fn[fn.size()-i-1];

		if (c == '\\' || c == '/')
			break;

		s = c + s;
	}

	return s;
}


//--------------------------------------------------
//���Ҫ�� md5.h
#ifdef __USE_MD5__
//md5����
std::string md5(const std::string strA1)
{
	unsigned char szMD5[16];
	char outbuf[3];

	MD5_CTX md5;

	MD5Init( &md5 );
	MD5Update( &md5, (unsigned char *)strA1.c_str( ), (unsigned int)strA1.size( ) );
	MD5Final( szMD5, &md5 );

	//return std::string( (char *)szMD5, 16 );//ע���������//�������Բ������Ƿ��н�β��"\0"�ַ���
	std::string s = "";

	for (int i = 0; i < 16; i++) 
	{
		sprintf(outbuf, "%02x", szMD5[i]);
		s += outbuf;
	}
    
	return s;
}
#endif
//--------------------------------------------------

void printf_error(const char * s)
{
	while(true)
	{
		printf("%s", s);
		::Sleep(100);
	}
}

//���Ҫ�� gd ��/////////////////////////////////
#ifdef __USE_GD__


//Ϊ��Ч��Ҫ�� gdFree �ͷŷ���ֵ
char * resize_imagett(const std::string fn, int new_W, int new_H, int & size)
{
		FILE * old_fp = NULL;
		FILE * new_fp = NULL;
		gdImagePtr old_im, new_im; //ͼƬ�ɡ���ͼƬ���
		//char * old_fn = "C:\\Documents and Settings\\Administrator\\����\\1.jpg";
		//char * new_fn = "C:\\Documents and Settings\\Administrator\\����\\2.jpg";
		char * old_fn = "C:\\Documents and Settings\\wilson\\����\\1.jpg";
		char * new_fn = "C:\\Documents and Settings\\wilson\\����\\2.jpg";
		int A4_W = 40;//?
		int A4_H = 40;//?
		new_W = 100;//�¿��
		new_H = 100;//�¸߶�

		/*
		if((old_fp=fopen(old_fn, "rb"))!=NULL)
		{ //��ԭͼƬ�ļ�
			old_im=gdImageCreateFromJpeg(old_fp);//ȡ��ԭͼƬ���
			new_im = gdImageCreate(A4_W,A4_H); //������ͼƬ��ȡ�þ��

			int white = gdImageColorAllocate(new_im, 255, 255, 255); //��ɫ,������ɫ
			int black = gdImageColorAllocate(new_im, 0, 0, 0); //��ɫ,��ǰ��ɫ

			gdImageCopyResized(new_im,old_im,0,0,0,0,new_W,new_H,old_im->sx,old_im->sy);//���Ʋ�����ͼƬ,new_W,new_H,��ͼƬ�Ŀ��
			if((new_fp=fopen(new_fn,"wb"))!=NULL)
			{ //�����ļ�
				gdImageJpeg(new_im,new_fp,-1); //���ͼƬ���ļ�
				fclose(new_fp);
			};
			fclose(old_fp);
		};
		*/

		if((old_fp=fopen(old_fn, "rb"))!=NULL)
		{ //��ԭͼƬ�ļ�
			old_im=gdImageCreateFromJpeg(old_fp);//ȡ��ԭͼƬ���

		}

	return "";
}


//Ϊ��Ч��Ҫ�� gdFree �ͷŷ���ֵ//type 0 - jpg, 1 - png
char * resize_image(const std::string fn, int new_W, int new_H, int & size, int type, int quality)
//char * resize_image(const std::string fn, int new_W, int new_H, long & size, int type, int quality)
{

	FILE * old_fp = NULL;
	FILE * new_fp = NULL;
	gdImagePtr old_im = NULL;
	gdImagePtr new_im = NULL; //ͼƬ�ɡ���ͼƬ���

	char * old_fn = (char *)fn.c_str();//"C:\\Documents and Settings\\Administrator\\����\\1.jpg";
	//char * old_fn = "C:\\Documents and Settings\\wilson\\����\\1.jpg";


	//char * new_fn = "C:\\Documents and Settings\\Administrator\\����\\2.jpg";
	//int A4_W = new_W;//40;//?
	//int A4_H = new_H;//40;//?
	//int new_W = 100;//�¿��
	//int new_H = 100;//�¸߶�
	char * data = NULL;

	log_cur(log_file);
	//����//�ܷ񱣴����ڴ���
	if((old_fp=fopen(old_fn, "rb"))!=NULL)
	{ //��ԭͼƬ�ļ�
	log_cur(log_file);
		old_im=gdImageCreateFromJpeg(old_fp);//ȡ��ԭͼƬ���

	log_cur(log_file);
		if (old_im == NULL)
			return NULL;
	log_cur(log_file);

		//����߶�//��ʱ

		//old_im->sx;//���
		//old_im->sy;//�߶�

		new_H = new_W * (((double)old_im->sy)/(double)(old_im->sx));
		
		//����߶�//��ʱ _end;

		//һ��Ҫ���پ�ͼƬ�������ڴ�й¶
		fclose(old_fp);
		//���ں���//gdImageDestroy(old_im);

		//new_im = gdImageCreate(A4_W,A4_H); //������ͼƬ��ȡ�þ��
//		new_im = gdImageCreate(new_W, new_H); //������ͼƬ��ȡ�þ��
		new_im = gdImageCreateTrueColor(new_W, new_H); //������ͼƬ��ȡ�þ��
		

		//int white = gdImageColorAllocate(new_im, 255, 255, 255); //��ɫ,������ɫ
		//int black = gdImageColorAllocate(new_im, 0, 0, 0); //��ɫ,��ǰ��ɫ

		//gdImageCopyResized ��Ч����Ҷ�˵����
//		gdImageCopyResized(new_im,old_im,0,0,0,0,new_W,new_H,old_im->sx,old_im->sy);//���Ʋ�����ͼƬ,new_W,new_H,��ͼƬ�Ŀ��
		gdImageCopyResampled(new_im,old_im,0,0,0,0,new_W,new_H,old_im->sx,old_im->sy);//���Ʋ�����ͼƬ,new_W,new_H,��ͼƬ�Ŀ��
		/*
		if((new_fp=fopen(new_fn,"wb"))!=NULL)
		{ //�����ļ�
			gdImageJpeg(new_im,new_fp,-1); //���ͼƬ���ļ�
			fclose(new_fp);
		};
		fclose(old_fp);
		*/

		if (type == 0)
		{
			data = (char *) gdImagePngPtr(new_im, &size);
		}
		else
		{
			if ((quality < -1)||(quality > 100))
			{
				quality = 85;//ѹ����
			}
			//data = (char *) gdImageJpegPtr(new_im, &size, -1);//���һ��������ʲô��˼?// -1 ��Ч���ǳ����ã�85 �ȽϺ�
			data = (char *) gdImageJpegPtr(new_im, &size, quality);//���һ��������ʲô��˼?
		}

		if (!data)
		{
			//Error
			return NULL;
		}


	};
	log_cur(log_file);


	//һ��Ҫ���پ�ͼƬ�������ڴ�й¶
	//fclose(old_fp);
	//���ں���//
	if (old_im != NULL)
	{
		gdImageDestroy(old_im);
	}

	if (new_im != NULL)
	{
		gdImageDestroy(new_im);//����᲻�ᵼ�� data ʧЧ��?
	}

	//--------------------------------------------------
	//2010.4.5
	if ((data != NULL)&&(size != 0))
	{
		char * out_data = (char *)malloc(size);
		memcpy(out_data, data, size);
		gdFree(data);
	
		return out_data;
	}


	//return data;
	return NULL;
	//ע����㺯��һ��Ҫ�������//gdFree(data); //���ã�ֱ�� free

}

#endif
//���Ҫ�� gd ��/////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
//ֻ�� windows �²����õĺ���
#ifdef WIN32

//windowsʱ��ת��Ϊtime_t//����https://ccvs.cvshome.org/source/browse/ccvs/windows-NT/JmgStat.c?rev=1.3&content-type=text/vnd.viewcvs-markup
bool FileTimeToUnixTime ( const FILETIME* ft, time_t* ut, bool local_time )
{
    bool success = FALSE;
    if ( local_time ) 
    {
        struct tm atm;
        SYSTEMTIME st;

        success = FileTimeToSystemTime ( ft, &st );

        /* Important: mktime looks at the tm_isdst field to determine
         * whether to apply the DST correction. If this field is zero,
         * then no DST is applied. If the field is one, then DST is
         * applied. If the field is minus one, then DST is applied
         * if the United States rule calls for it (DST starts at 
         * 02:00 on the first Sunday in April and ends at 02:00 on
         * the last Sunday in October.
         *
         * If you are concerned about time zones that follow different 
         * rules, then you must either use GetTimeZoneInformation() to 
         * get your system's TIME_ZONE_INFO and use the information
         * therein to figure out whether the time in question was in 
         * DST or not, or else use SystemTimeToTzSpecifiedLocalTime()
         * to do the same.
         *
         * I haven't tried playing with SystemTimeToTzSpecifiedLocalTime()
         * so I am nor sure how well it handles funky stuff.
         */
        atm.tm_sec = st.wSecond;
        atm.tm_min = st.wMinute;
        atm.tm_hour = st.wHour;
        atm.tm_mday = st.wDay;
        /* tm_mon is 0 based */
        atm.tm_mon = st.wMonth - 1;
        /* tm_year is 1900 based */
        atm.tm_year = st.wYear>1900?st.wYear - 1900:st.wYear;     
        atm.tm_isdst = -1;      /* see notes above */
        *ut = mktime ( &atm );
    }
    else 
    {

       /* FILETIME = number of 100-nanosecond ticks since midnight 
        * 1 Jan 1601 UTC. time_t = number of 1-second ticks since 
        * midnight 1 Jan 1970 UTC. To translate, we subtract a
        * FILETIME representation of midnight, 1 Jan 1970 from the
        * time in question and divide by the number of 100-ns ticks
        * in one second.
        */

        /* One second = 10,000,000 * 100 nsec */
        const ULONGLONG second = 10000000L;

        SYSTEMTIME base_st = 
        {
            1970,   /* wYear            */
            1,      /* wMonth           */
            0,      /* wDayOfWeek       */
            1,      /* wDay             */
            0,      /* wHour            */
            0,      /* wMinute          */
            0,      /* wSecond          */
            0       /* wMilliseconds    */
        };
        
        ULARGE_INTEGER itime;
        FILETIME base_ft;

        success = SystemTimeToFileTime ( &base_st, &base_ft );
        if (success) 
        {
            itime.QuadPart = ((ULARGE_INTEGER *)ft)->QuadPart;

            itime.QuadPart -= ((ULARGE_INTEGER *)&base_ft)->QuadPart;
            itime.QuadPart /= second;

            *ut = itime.LowPart;
        }
    }
    if (!success)
    {
        *ut = -1;   /* error value used by mktime() */
    }
    return success;
}

// GB2312 ת���� utf-8
std::string gbk_to_utf8(std::string src) 
{
	//int len = src.size() * 2 +1;//��С�������ٴ�����
	int len = (src.size()+1) * 4;//4��
	char * p_out = (char *)malloc(len);//LPWSTR
	char * p_out_utf8 = (char *)malloc(len);
	memset(p_out, 0, len); 
	memset(p_out_utf8, 0, len); 

	//gbk ת���� unicode
	//int word_count = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src.c_str(), src.size(), (WCHAR *)p_out, src.size());
	int word_count = ::MultiByteToWideChar(936, MB_PRECOMPOSED, 
		src.c_str(), 
		src.size(), // number of bytes in string//�ֽ���
		(WCHAR *)p_out, 
		len//src.size()//size of buffer//ע���ǻ������ֽ��������ǿ��ַ��ĸ���
		);
	//::MultiByteToWideChar(936, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);//936 �������ַ���

	//uncode ת���� CP_UTF8
	//::WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, (WCHAR *)p_out, word_count, p_out_utf8, len,
	//�� WC_COMPOSITECHECK ������ת�����˵�
	word_count = ::WideCharToMultiByte(CP_UTF8, 0, 
		(WCHAR *)p_out, 
		word_count, // number of chars in string//���ַ��ĸ���
		p_out_utf8, 
		len,// size of buffer//�������Ĵ�С
		NULL, NULL);//CP_UTF8 UTF-8�����ô�ֵʱlpDefaultChar��lpUsedDefaultChar������ΪNULL

	std::string s = p_out_utf8;

	free(p_out);
	free(p_out_utf8);
	return s; 

	/*
	Code Value (Codepage) Alphabet 
	DIN_66003 20106 IA5 (German) 
	NS_4551-1 20108 IA5 (Norwegian) 
	SEN_850200_B 20107 IA5 (Swedish) 
	_autodetect 50932 Japanese (Auto Select) 
	_autodetect_kr 50949 Korean (Auto Select) 
	big5 950 Chinese Traditional (Big5) 
	csISO2022JP 50221 Japanese (JIS-Allow 1 byte Kana) 
	euc-kr 51949 Korean (EUC) 
	gb2312 936 Chinese Simplified (GB2312) 
	hz-gb-2312 52936 Chinese Simplified (HZ) 
	ibm852 852 Central European (DOS) 
	ibm866 866 Cyrillic Alphabet (DOS) 
	irv 20105 IA5 (IRV) 
	iso-2022-jp 50220 Japanese (JIS) 
	iso-2022-jp 50222 Japanese (JIS-Allow 1 byte Kana) 
	iso-2022-kr 50225 Korean (ISO) 
	iso-8859-1 1252 Western Alphabet 
	iso-8859-1 28591 Western Alphabet (ISO) 
	iso-8859-2 28592 Central European Alphabet (ISO) 
	iso-8859-3 28593 Latin 3 Alphabet (ISO) 
	iso-8859-4 28594 Baltic Alphabet (ISO) 
	iso-8859-5 28595 Cyrillic Alphabet (ISO) 
	iso-8859-6 28596 Arabic Alphabet (ISO) 
	iso-8859-7 28597 Greek Alphabet (ISO) 
	iso-8859-8 28598 Hebrew Alphabet (ISO) 
	koi8-r 20866 Cyrillic Alphabet (KOI8-R) 
	ks_c_5601 949 Korean 
	shift-jis 932 Japanese (Shift-JIS) 
	unicode 1200 Universal Alphabet 
	unicodeFEFF 1201 Universal Alphabet (Big-Endian) 
	utf-7 65000 Universal Alphabet (UTF-7) 
	utf-8 65001 Universal Alphabet (UTF-8) 
	windows-1250 1250 Central European Alphabet (Windows) 
	windows-1251 1251 Cyrillic Alphabet (Windows) 
	windows-1252 1252 Western Alphabet (Windows) 
	windows-1253 1253 Greek Alphabet (Windows) 
	windows-1254 1254 Turkish Alphabet 
	windows-1255 1255 Hebrew Alphabet (Windows) 
	windows-1256 1256 Arabic Alphabet (Windows) 
	windows-1257 1257 Baltic Alphabet (Windows) 
	windows-1258 1258 Vietnamese Alphabet (Windows) 
	windows-874 874 Thai (Windows) 
	x-euc 51932 Japanese (EUC) 
	x-user-defined 50000 User Defined
	*/
}

// GB2312 ת���� unicode
std::wstring gbk_to_unicode(std::string src) 
{
	//int len = src.size() * 2 +1;//��С�������ٴ�����
	int len = (src.size()+1) * 4;//4��
	WCHAR * p_out = (WCHAR *)malloc(len);//LPWSTR
	memset(p_out, 0, len); 

	//gbk ת���� unicode
	//int word_count = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src.c_str(), src.size(), (WCHAR *)p_out, src.size());
	int word_count = ::MultiByteToWideChar(936, MB_PRECOMPOSED, 
		src.c_str(),
		src.size(), // number of bytes in string//�ֽ���
		p_out, 
		len//src.size()//size of buffer//ע���ǻ������ֽ��������ǿ��ַ��ĸ���
		);

	std::wstring s = p_out;

	s.resize(word_count);//test//��ʵ����һ�������⣬��ʱ��������

	free(p_out);
	return s; 

}


// unicode ת���� GB2312
std::string unicode_to_gbk(std::wstring src) 
{
	//int len = src.size() * 2 +1;//��С�������ٴ�����
	int len = (src.size()+1) * 4;//4��
	char * p_out_utf8 = (char *)malloc(len);
	memset(p_out_utf8, 0, len); 

	WCHAR * p_out = (WCHAR *)src.c_str();

	//uncode ת���� gbk
	//int word_count = ::WideCharToMultiByte(936, 0, (WCHAR *)p_out, word_count, p_out_utf8, len,
	int word_count = ::WideCharToMultiByte(936, 0, (WCHAR *)p_out, src.size(), p_out_utf8, len,
		NULL, NULL);

	std::string s = p_out_utf8;

	//free(p_out);
	free(p_out_utf8);
	return s; 

}

//ȡǰ�����ַ�,ע���Ǻ��ֵģ����ֿܷ�һ������
std::string left_string_cn(std::string src, int count)
{
	std::wstring ws = gbk_to_unicode(src);

	int src_count = ws.size();

	if (count > src_count) count = src_count;//����Խ��

	ws = substring_STL(ws, 0, count);

	//ws = ws.substr(0, count);//test

	std::string s = unicode_to_gbk(ws);

	return s;

}

//ȡǰ�����ַ�,ע���Ǻ��ֵģ����ֿܷ�һ������
std::string sub_string_cn(std::string src, int begin, int count)
{
	std::wstring ws = gbk_to_unicode(src);

	int src_count = ws.size();

	if (count > src_count) count = src_count;//����Խ��

	//ws = substring_STL(ws, 0, count);
	ws = substring_STL(ws, begin, count);

	//ws = ws.substr(0, count);//test

	std::string s = unicode_to_gbk(ws);

	return s;

}

//windowsʱ��ת��Ϊtime_t
time_t get_time_t( const FILETIME ft )
{
  time_t t;
  FileTimeToUnixTime (&ft, &t, true);
  return t;
}

std::string get_app_filename()
{
	std::string r = "";

	tmem t(MAX_PATH);//���ͷ���ʱ�ڴ�
	char * buf = t.buf;

	::GetModuleFileName(NULL, buf, MAX_PATH);
	r = buf;

	//printf("%s\r\n", r.c_str());

	return r;
}

//ֻ�����Ϸ��ַ�//����Ҳ�ᱻ���
std::string clear_str(std::string s)
{
	std::string r = "";
	char c = 0;

	for (int i = 0; i < s.size(); i++)
	{
		c = s[i];
		if (
			((c >= '0')&&( c <= '9'))
			|| ((c >= 'a')&&( c <= 'z'))
			|| ((c >= 'A')&&( c <= 'Z'))
			|| (c == '_')
			)
		{
			r += c;
		}

		/*
		char * p = NULL; 
		p = strchr("_-",'5');
		if (p != NULL)
		{
			r += c;
		}
		*/
	}

	return r;

}


#endif
////////////////////////////////////////////////////////////////



