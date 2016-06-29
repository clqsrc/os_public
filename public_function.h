#ifndef _PUBLIC_FUNCTION_H_
#define _PUBLIC_FUNCTION_H_

#include "os.h"
#include "thread_api.h"
#include "thread_lock.h"
#include <iostream>
#include <vector>
#include <map>
#include <string>
#include "tmem.h"
#include "debug_new.h"

//--------------------------------------------------
//���Ҫ�� gd ��
#ifdef __USE_GD__

#include <gd.h>
#include <gdfontg.h>

#endif


//--------------------------------------------------
//���Ҫ�� md5.h
#ifdef __USE_MD5__

#endif
//--------------------------------------------------




/* ���ú���/���ú��� *///���÷ŵ�CPP��ʵ��,Ҫ�����п�������ʱ����


//__DEBUG__��gcc[dev c++]�ĵ��Ա�־,_DEBUG��VC��
#ifdef __DEBUG__
#define _DEBUG  
#endif

//������VC��_RPT0��//ֻ�ڵ���״̬�´�ӡ//vc�µ�
#ifdef _DEBUG
  #define  printfd(a)   printf(a)
  #define  printfd2(a,b)   printf(a,b)
  #define  printfd3(a,b,c)   printf(a,b,c)
  #define  printfd4(a,b,c,d)   printf(a,b,c,d)
#else
  #define  printfd(a)   
  #define  printfd2(a,b)   
  #define  printfd3(a,b,c)   
  #define  printfd4(a,b,c,d)   
#endif

typedef std::map<std::string, std::string> keys;//��ֵ��
typedef std::vector< std::string > lines;//�б�

//ȡ�õ�ǰʱ��
int GetTimeZone();

//�õ����ص� tm �ṹ
tm GetLocalTM();

//�õ� gm �� tm �ṹ
tm GetTM();

//mktime �ǰ� tm �е�ʱ�䵱������ʱ�����õ���������ʱ���,����Ҫ�� tm �е�ʱ��ֱ�ӵ�����������ʱ������//�㷨�Ͼ���Ҫ����ʱ�������
time_t gm_mktime(struct tm * m);

//�õ� mysql ��ʽ�ĵ�ǰʱ��
std::string GetMysqlDateTime();

//�õ��ļ�·��
std::string extract_file_path(const std::string fn);

//��һ������д����ļ�// log ���͵��ص���ļ�
FILE * open_log_file(const std::string fn);

//����ת��Ϊ�ַ���
std::string int_to_str(long value);
std::string int_to_str(long value, int width);
//������ת��Ϊ�ַ���
std::string float_to_str(double value, int width);
std::string float_to_str(double value);
int str_to_int(std::string v);
//�滻�ַ���
std::string str_replace(const std::string s, const std::string old_s, const std::string new_s);

//trim
std::string trim(std::string s);
//����ҪЧ�ʵĵط����� trim//ȥ���ұߵĿո�
void trimr(char * buf, int buf_len);
//Сд
std::string lcase(std::string s);
//����ҪЧ�ʵĵط����� std::string ��ʵ��
void lcase(char * buf, int buf_len);
//��д
std::string ucase(std::string s);
//����ҪЧ�ʵĵط����� std::string ��ʵ��
void ucase(char * buf, int buf_len);

//����//[��Сд������]
//std::string uncase_find(std::string s1, std::string s2)
bool ufind(std::string s, std::string sub_s);
//��java��substring��ʵ�ֵ�,��Ҫ��Ϊjava�Ĵ�����ֲ��C++��,����VC��std::string��substr�з��ʵĲ���Խ��Ļ����쳣[�����Ϊ���ؿ��ַ���]
std::string substring_JAVA(std::string s, int beginIndex, int endIndex);
//����VC��STL�쳣,�μ�substring_JAVA��˵��
std::string substring_STL(std::string s, int pos, int n);
//�ַ����Ƿ���ͬ[��Сд������]
bool uncase_cmp(const char * buf,const char * cmd);
//�򵥱Ƚ��Ƿ�һ��
bool str_eq(std::string s1, std::string s2);
//���Դ�Сд�ıȽ�
bool eqIgnoreCase(const std::string s1, const std::string s2);


//����http://www.alhem.net/Sockets/html/Utility_8cpp-source.html
std::string rfc1738_decode(const std::string & src);
std::string rfc1738_encode(const std::string & src);

//��java��ֲ����,����ȡ��ֵ�ַ���
std::string get_value1(std::string src, std::string sp_begin1, std::string sp_end1);
//�õ��ļ���׺��
std::string extract_file_ext(const std::string fn);
//�õ��ļ���
std::string extract_file_name(const std::string fn);

//�Ƚϸ������Ƿ����
bool float_cmp(const float f1, const float f2);

//���ļ���װ������
char * load_from_file(const char * fn, long & len);//ע��,����ķ���ֵ��Ҫ delete [] ��//������������Ǹ�
std::string load_from_file(const char * fn);//ע��,����ķ���ֵ�ǲ��� delete��
void load_from_file(lines & l, const char * fn);
void load_from_file(lines & l, const char * fn, char sp);
void load_from_string(lines & l, std::string & buf);//[\r][\n]�Ƿָ���
void load_from_string(lines & l, std::string & buf, char sp);//sp�Ƿָ���

//����һ�����������ݵ��ļ���
bool save_to_file(const char * fn, const char * buf, long len);
bool save_to_file(const std::string fn, const std::string buf);
bool save_to_file(const std::string fn, lines & l);


//���ļ��м��ؼ�ֵ��
void load_keys1(keys & k, const std::string fn);
void load_keys_buf1(keys & k, std::string & buf);
//�����ֵ��
void save_keys1(keys & k, const std::string fn);

//�ж�ĳ�����Ƿ����б���
bool key_in_keys(std::string key, keys & list);

//���� NULL ��ֵ����,���ü�����
std::string safe_std_string(const char * str);
std::string safe_std_string(const std::string str);
std::string safe_std_string(keys & list, const std::string key);

bool get_value(std::map<std::string, std::string> list, std::string key, std::string & value);
std::string get_value(std::map<std::string, std::string> list, std::string key);
std::string str_to_hex(std::string s);
std::string buf_to_hex(const char * s, int len);
std::string hex_to_str(std::string s);
//md5����
std::string md5(const std::string strA1);

//��ͣ�ش�ӡ������Ϣ,������������ʱ����
void printf_error(const char * s);

//����Ƿ��ǰ�ȫ���ַ���//�»���,����,��ĸ
bool check_safe_str(std::string s);
//ȡǰ�����ַ�,ע���Ǻ��ֵģ����ֿܷ�һ������
std::string left_string_cn(std::string src, int count);
// GB2312 ת���� unicode
std::wstring gbk_to_unicode(std::string src);
// GB2312 ת���� utf-8
std::string gbk_to_utf8(std::string src);
// unicode ת���� GB2312
std::string unicode_to_gbk(std::wstring src);
//ȡǰ�����ַ�,ע���Ǻ��ֵģ����ֿܷ�һ������
std::string sub_string_cn(std::string src, int begin, int count);
//ֻ�����Ϸ��ַ�//����Ҳ�ᱻ���
std::string clear_str(std::string s);
//ģ�庯��
std::string get_template_string(std::string s, keys & k);
std::string get_template_string_from_file(std::string fn, keys & k);
//ģ�庯��
//std::string make_string_3(const std::string s, const std::string sb, const std::string se, std::string & l, std::string & r);
std::string make_string_3(const std::string src, std::string sp_begin1, std::string sp_end1, std::string & l, std::string & r);

//gd ������
//Ϊ��Ч��Ҫ�� gdFree �ͷŷ���ֵ//type 0 - jpg, 1 - png
char * resize_image(const std::string fn, int new_W, int new_H, int & size, int type, int quality);
//char * resize_image(const std::string fn, int new_W, int new_H, long & size, int type, int quality);


//char *data;
//gdFree(data);


////////////////////////////////////////////////////////////////////////////
//ֻ�� windows �����õĺ���
#ifdef WIN32

//windowsʱ��ת��Ϊtime_t//����https://ccvs.cvshome.org/source/browse/ccvs/windows-NT/JmgStat.c?rev=1.3&content-type=text/vnd.viewcvs-markup
bool FileTimeToUnixTime ( const FILETIME* ft, time_t* ut, bool local_time );

//windowsʱ��ת��Ϊtime_t
time_t get_time_t( const FILETIME ft );

std::string get_app_filename();

#endif
////////////////////////////////////////////////////////////////////////////


#endif
