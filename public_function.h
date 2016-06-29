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
//如果要用 gd 库
#ifdef __USE_GD__

#include <gd.h>
#include <gdfontg.h>

#endif


//--------------------------------------------------
//如果要用 md5.h
#ifdef __USE_MD5__

#endif
//--------------------------------------------------




/* 公用函数/常用函数 *///都得放到CPP中实现,要不很有可能连接时出错


//__DEBUG__是gcc[dev c++]的调试标志,_DEBUG是VC的
#ifdef __DEBUG__
#define _DEBUG  
#endif

//类似于VC的_RPT0宏//只在调试状态下打印//vc下的
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

typedef std::map<std::string, std::string> keys;//键值对
typedef std::vector< std::string > lines;//列表

//取得当前时区
int GetTimeZone();

//得到当地的 tm 结构
tm GetLocalTM();

//得到 gm 的 tm 结构
tm GetTM();

//mktime 是把 tm 中的时间当做本地时间来得到格林威治时间的,现在要把 tm 中的时间直接当做格林威治时间来算//算法上就是要加上时区的秒差
time_t gm_mktime(struct tm * m);

//得到 mysql 格式的当前时间
std::string GetMysqlDateTime();

//得到文件路径
std::string extract_file_path(const std::string fn);

//打开一个立即写入的文件// log 类型的特点的文件
FILE * open_log_file(const std::string fn);

//整数转换为字符串
std::string int_to_str(long value);
std::string int_to_str(long value, int width);
//浮点数转换为字符串
std::string float_to_str(double value, int width);
std::string float_to_str(double value);
int str_to_int(std::string v);
//替换字符串
std::string str_replace(const std::string s, const std::string old_s, const std::string new_s);

//trim
std::string trim(std::string s);
//在需要效率的地方代替 trim//去掉右边的空格
void trimr(char * buf, int buf_len);
//小写
std::string lcase(std::string s);
//在需要效率的地方代替 std::string 的实现
void lcase(char * buf, int buf_len);
//大写
std::string ucase(std::string s);
//在需要效率的地方代替 std::string 的实现
void ucase(char * buf, int buf_len);

//查找//[大小写不敏感]
//std::string uncase_find(std::string s1, std::string s2)
bool ufind(std::string s, std::string sub_s);
//按java的substring来实现的,主要是为java的代码移植到C++中,而且VC的std::string的substr中访问的参数越界的话会异常[这里改为返回空字符串]
std::string substring_JAVA(std::string s, int beginIndex, int endIndex);
//避免VC中STL异常,参见substring_JAVA的说明
std::string substring_STL(std::string s, int pos, int n);
//字符串是否相同[大小写不敏感]
bool uncase_cmp(const char * buf,const char * cmd);
//简单比较是否一样
bool str_eq(std::string s1, std::string s2);
//忽略大小写的比较
bool eqIgnoreCase(const std::string s1, const std::string s2);


//来自http://www.alhem.net/Sockets/html/Utility_8cpp-source.html
std::string rfc1738_decode(const std::string & src);
std::string rfc1738_encode(const std::string & src);

//从java移植而来,用于取键值字符串
std::string get_value1(std::string src, std::string sp_begin1, std::string sp_end1);
//得到文件后缀名
std::string extract_file_ext(const std::string fn);
//得到文件名
std::string extract_file_name(const std::string fn);

//比较浮点数是否相等
bool float_cmp(const float f1, const float f2);

//从文件中装载内容
char * load_from_file(const char * fn, long & len);//注意,这个的返回值是要 delete [] 的//尽量作后面的那个
std::string load_from_file(const char * fn);//注意,这个的返回值是不用 delete的
void load_from_file(lines & l, const char * fn);
void load_from_file(lines & l, const char * fn, char sp);
void load_from_string(lines & l, std::string & buf);//[\r][\n]是分隔符
void load_from_string(lines & l, std::string & buf, char sp);//sp是分隔符

//保存一个缓冲区内容到文件中
bool save_to_file(const char * fn, const char * buf, long len);
bool save_to_file(const std::string fn, const std::string buf);
bool save_to_file(const std::string fn, lines & l);


//从文件中加载键值对
void load_keys1(keys & k, const std::string fn);
void load_keys_buf1(keys & k, std::string & buf);
//保存键值对
void save_keys1(keys & k, const std::string fn);

//判断某个键是否在列表中
bool key_in_keys(std::string key, keys & list);

//避免 NULL 赋值崩溃,引用计数等
std::string safe_std_string(const char * str);
std::string safe_std_string(const std::string str);
std::string safe_std_string(keys & list, const std::string key);

bool get_value(std::map<std::string, std::string> list, std::string key, std::string & value);
std::string get_value(std::map<std::string, std::string> list, std::string key);
std::string str_to_hex(std::string s);
std::string buf_to_hex(const char * s, int len);
std::string hex_to_str(std::string s);
//md5编码
std::string md5(const std::string strA1);

//不停地打印错误信息,在有致命错误时才用
void printf_error(const char * s);

//检查是否是安全的字符串//下划线,数字,字母
bool check_safe_str(std::string s);
//取前几个字符,注意是汉字的，不能分开一个汉字
std::string left_string_cn(std::string src, int count);
// GB2312 转换成 unicode
std::wstring gbk_to_unicode(std::string src);
// GB2312 转换成 utf-8
std::string gbk_to_utf8(std::string src);
// unicode 转换成 GB2312
std::string unicode_to_gbk(std::wstring src);
//取前几个字符,注意是汉字的，不能分开一个汉字
std::string sub_string_cn(std::string src, int begin, int count);
//只保留合法字符//中文也会被清除
std::string clear_str(std::string s);
//模板函数
std::string get_template_string(std::string s, keys & k);
std::string get_template_string_from_file(std::string fn, keys & k);
//模板函数
//std::string make_string_3(const std::string s, const std::string sb, const std::string se, std::string & l, std::string & r);
std::string make_string_3(const std::string src, std::string sp_begin1, std::string sp_end1, std::string & l, std::string & r);

//gd 处理类
//为了效率要用 gdFree 释放返回值//type 0 - jpg, 1 - png
char * resize_image(const std::string fn, int new_W, int new_H, int & size, int type, int quality);
//char * resize_image(const std::string fn, int new_W, int new_H, long & size, int type, int quality);


//char *data;
//gdFree(data);


////////////////////////////////////////////////////////////////////////////
//只有 windows 才能用的函数
#ifdef WIN32

//windows时间转换为time_t//来自https://ccvs.cvshome.org/source/browse/ccvs/windows-NT/JmgStat.c?rev=1.3&content-type=text/vnd.viewcvs-markup
bool FileTimeToUnixTime ( const FILETIME* ft, time_t* ut, bool local_time );

//windows时间转换为time_t
time_t get_time_t( const FILETIME ft );

std::string get_app_filename();

#endif
////////////////////////////////////////////////////////////////////////////


#endif
