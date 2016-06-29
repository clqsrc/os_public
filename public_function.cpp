
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
//如果要用 md5.h
#ifdef __USE_MD5__
#include "md5.h"
#endif
//--------------------------------------------------

//#include "md5.h"
#include <math.h>

//#include <gd.h>
//#include <gdfontg.h>

extern FILE * log_file;//日志文件


/* 公用函数/常用函数 */

//得到文件路径//不含最后的[/]
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


//得到 mysql 格式的当前时间
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

//打开一个立即写入的文件// log 类型的特点的文件
FILE * open_log_file(const std::string fn)
{
	FILE * f = fopen(fn.c_str(), "a+b");
	if(f == NULL)
	{
		printf("%s", "error! log file can't open!\r\n");
		return NULL;
	}
	setbuf(f, NULL);//立即写入文件

	return f;

}

//得到当地的 tm 结构
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


//得到 gm 的 tm 结构
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


//取得当前时区
int GetTimeZone()
{
	time_t t = 0;//time(NULL);
	tm * m = NULL;

	tm gtm;//格林威治
	tm ltm;//本地
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

//mktime 是把 tm 中的时间当做本地时间来得到格林威治时间的,现在要把 tm 中的时间直接当做格林威治时间来算//算法上就是要加上时区的秒差
time_t gm_mktime(struct tm * m)
{
	time_t t = mktime(m);//这时的 t 是要少 8 个小时的,要加上8小时的时间//但对于其他国家来说怎么算出这个时差呢.

	//t = 8 * 60 * 60;

	int iTimeZone = GetTimeZone();

	t = t + iTimeZone * 60 * 60;

	return t;
}


//比较浮点数是否相等//浮点数是不能直接写等号比较的，要一个精度范围内才行
bool float_cmp(const float f1, const float f2)
{
	bool r = false;

	if (fabs(f1 - f2) < 0.000001)
	{
		r = true;
	}

	return r;
}

//从文件中装载内容//这个还要释放返回的字符串内存尽量用后面的那个
char * load_from_file(const char * fn, long & len)
{
	if ((fn == NULL)||(strlen(fn) == 0))
	{
		return NULL;
	}

	len = file_system::get_file_length(fn);

	if (0 >= len) return NULL;

	//char * buf = new char[len];
	char * buf = (char *)malloc(len);//统一换用 malloc/free
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

	long r = fread(buf, 1, len, f);//如果是fread(buf, len, 1, f)会出错！！但是buf足够大的时候又是对的！

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



std::string load_from_file(const char * fn)//注意,这个的返回值是不用 delete的
{
	long len;
	char * buf = load_from_file(fn, len);

	if (buf == NULL)
		return "";

	std::string s(buf, len);

	//delete [] buf;
	free(buf);//统一换用 malloc/free
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

	//还有\n之外的最后的行
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

	//还有\n之外的最后的行
	if (line.size() > 0)
	{
		l.push_back(line);
		line = "";
	}

}

//保存一个缓冲区内容到文件中
bool save_to_file(const char * fn, const char * buf, long len)
{
	//写入文件
	FILE * f = fopen(fn, "wb");
	if (f == NULL)
	{
		//fclose(f);//不用 文件还没打开
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

//保存一个缓冲区内容到文件中
bool save_to_file(const std::string fn, const std::string buf)
{
	//写入文件
	return save_to_file(fn.c_str(), buf.data(), buf.size());
}

//保存一个缓冲区内容到文件中
bool save_to_file(const std::string fn, lines & l)
{
	std::string buf = "";

	for (int i = 0; i < l.size(); i++)
	{
		buf += l[i] + "\r\n";
	}

	//写入文件
	return save_to_file(fn.c_str(), buf.data(), buf.size());
}


//整数转换为字符串
std::string int_to_str(long value)
{
	char s[256];
	memset(s, 0, sizeof(s));
	//ltoa(value,s,10);//似乎不能在linux下使用
	//sprintf(s, "%d", value);
	sprintf(s, "%ld", value);//可用吗//sprintf(s, "%08X", 4567); //产生："000011D7"

	std::string r = s;
	return r;
}

//整数转换为字符串
std::string int_to_str(long value, int width)
{
	char f[256];//格式
	char s[256];
	memset(f, 0, sizeof(s));
	memset(s, 0, sizeof(s));

	//ltoa(value,s,10);//似乎不能在linux下使用
	//sprintf(s, "%d", value);
	sprintf(f, "%%0%dld", width);//如"%08ld"
	//sprintf(s, "%ld", value);//可用吗//sprintf(s, "%08X", 4567); //产生："000011D7"
	sprintf(s, f, value);//可用吗//sprintf(s, "%08X", 4567); //产生："000011D7"

	std::string r = s;
	return r;
}

//浮点数转换为字符串
std::string float_to_str(double value, int width)
{
	char f[256];//格式
	char s[256];
	memset(f, 0, sizeof(s));
	memset(s, 0, sizeof(s));

	//参考 http://www.cnblogs.com/wqlblogger/archive/2007/01/09/615525.aspx
	//sprintf(f, "%%*.*f");
	sprintf(f, "%%.*f");
	sprintf(s, f, width, value);

	std::string r = s;
	return r;
}


//浮点数转换为字符串
std::string float_to_str(double value)
{
	char f[256];//格式
	char s[256];
	memset(f, 0, sizeof(s));
	memset(s, 0, sizeof(s));

	//参考 http://www.cnblogs.com/wqlblogger/archive/2007/01/09/615525.aspx
	sprintf(f, "%%f");//默认的小数点位数为 6
	//sprintf(f, "%%*.*f");//这个不行
	sprintf(s, f, value);

	std::string r = s;
	return r;
}


int str_to_int(std::string v)
{
	return atoi( v.c_str() );
}

//模板函数
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

//模板函数
std::string make_string_3(const std::string src, std::string sp_begin1, std::string sp_end1, std::string & l, std::string & r)
{
	std::string m = "";
	
	std::string in1 = lcase(src);//用in1来查找字符串的位置,最后截取时仍然访问的是src//这样可以忽略大小写又不损坏原有的数据
	sp_begin1 = lcase(sp_begin1);
	sp_end1 = lcase(sp_end1);
	
	
	int pos_b1,pos_e1; //要得到的字符串的首尾位置
	
	//先找pos_b1
	if (sp_begin1.length()==0)//如果第1个分隔符为空就表示取第2个分隔符之前的字符串
	{
        pos_b1=0;
	}
	else//第一个分隔符不为空时
	{
		pos_b1=in1.find(sp_begin1);
		if (pos_b1 == -1) return "";//第一个分隔符不为空，而且找不到时返回值就是空
		
		pos_b1 += sp_begin1.length();//还要加上分隔符本身的长度
	}
	
	//从后pos_b1开始找pos_e1
	pos_e1=in1.find(sp_end1, pos_b1);
	if (sp_end1.length()==0)//如果第2个分隔符为空就表示取第1个分隔符之后的字符串
	{
        pos_e1=in1.length();
	}
	
	//如果两个字符串都不存在,则返回空串
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

//替换字符串
std::string str_replace(const std::string s, const std::string old_s, const std::string new_s)
{
	size_t pos = 0;
	std::string r = s;
	while(true)
	{
		pos = r.find(old_s, pos);
		if (pos == std::string::npos) break;

		//c++的replace是与其他语言有所不同的
		//r.replace(pos, pos + old_s.length(), new_s);
		//注意第二个参数是字数
		r.replace(pos, old_s.length(), new_s);
		pos += new_s.length();//查找位置要向下移
	}
  
	return r;
}



std::string trim(std::string s)
{
	if (s.size()==0) return "";
	//没想到还挺难写
	//以为可用int ibpos = strbuf[i].find_first_not_of(" "); int iepos = strbuf[i].find_last_not_of(" ");实现
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

	for (i = s.size()-1; ; i--)//这里的s.size()不能为0,否则会死循环
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

//在需要效率的地方代替 trim//去掉右边的空格
void trimr(char * buf, int buf_len)
{
	char c;
	int i = 0;

	for (i = buf_len-1; ; i--)//这里的s.size()不能为0,否则会死循环
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

//在需要效率的地方代替 std::string 的实现
void lcase(char * buf, int buf_len)
{
	for (int i=0; i < buf_len; i++)
	{
		buf[i] = tolower(buf[i]);
	}

}

//小写//注意对中文字符串并不安全
std::string lcase(std::string s)
{
	std::string r = "";
	for (int i=0;i<s.size();i++)
	{
		r += tolower(s[i]);
	}
	return r;
}

//在需要效率的地方代替 std::string 的实现
void ucase(char * buf, int buf_len)
{
	for (int i=0; i < buf_len; i++)
	{
		buf[i] = toupper(buf[i]);
	}

}


//大写//注意对中文字符串并不安全
std::string ucase(std::string s)
{
	std::string r = "";
	for (int i=0;i<s.size();i++)
	{
		r += toupper(s[i]);
	}
	return r;

}

//查找//[大小写不敏感]
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

//按java的substring来实现的,主要是为java的代码移植到C++中,而且VC的std::string的substr中访问的参数越界的话会异常[这里改为返回空字符串]
std::string substring_JAVA(std::string s, int beginIndex, int endIndex)
{
	std::string r = "";
    r = substring_STL(s, beginIndex, endIndex-beginIndex);//java中的substring的第2个参数是指终止字符的位置;而stl的则是要复制的长度,如果这个长度太大就会异常

	return r;
}

//避免VC中STL异常,参见substring_JAVA的说明
std::string substring_STL(std::string s, int pos, int n)
{
	std::string r = "";

	//如果起始位置太大,就直接返回空
	if (s.size()<=pos)
	{
		return "";
	}

	//如果起始位置太大,就直接返回空
	if (pos<0)
	{
		pos = 0;
	}

	//如果终止起始位置太大,就直接取最后的字符位置
	if (s.size() <= pos+n )
	{
		n = s.size()-pos;
	}
    r = s.substr(pos, n);//vc的stl中pos_b1不能小于pos_e1//java中的substring的第2个参数是指终止字符的位置;而stl的则是要复制的长度,如果这个长度太大就会异常

	return r;
}

//避免VC中STL异常,参见substring_JAVA的说明
std::wstring substring_STL(std::wstring s, int pos, int n)
{

	//std::wstring r = NULL;
	std::wstring r;
	r.resize(0);//奇怪不能直接写 r = "";

	//如果起始位置太大,就直接返回空
	if (s.size()<=pos)
	{
		return r;
	}

	//如果起始位置太大,就直接返回空
	if (pos<0)
	{
		pos = 0;
	}

	//如果终止起始位置太大,就直接取最后的字符位置
	if (s.size() <= pos+n )
	{
		n = s.size()-pos;
	}
    r = s.substr(pos, n);//vc的stl中pos_b1不能小于pos_e1//java中的substring的第2个参数是指终止字符的位置;而stl的则是要复制的长度,如果这个长度太大就会异常

	return r;
}


//来自http://www.alhem.net/Sockets/html/Utility_8cpp-source.html//去掉IE的UTF8才能正常使用
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
                         if ((i+2) >= src.size()) break;//这样安全些
                         char c1 = src[i + 1];
                         char c2 = src[i + 2];
                         c1 = c1 - 48 - ((c1 >= 'A') ? 7 : 0) - ((c1 >= 'a') ? 32 : 0);
                         c2 = c2 - 48 - ((c2 >= 'A') ? 7 : 0) - ((c2 >= 'a') ? 32 : 0);
						 //下面这两个也可以,不过只能是大写
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


 //不区分大小写
  std::string get_value1_old(std::string src, std::string sp_begin1, std::string sp_end1)
  {
	  //return "";
	  ///*
	  std::string in1 = lcase(src);//用in1来查找字符串的位置,最后截取时仍然访问的是src
	  sp_begin1 = lcase(sp_begin1);
	  sp_end1 = lcase(sp_end1);

	  std::string fs1 = "";

      int pos_b1,pos_e1; //要得到的字符串的首尾位置

      //先按第一个字符串截取字符//fs1为pos_b1后的字符串
      pos_b1=in1.find(sp_begin1)+sp_begin1.length();
      if (sp_begin1.length()==0)
      {
        pos_b1=0;
      }
      pos_e1=in1.length();
      //fs1=in1.substr(pos_b1, pos_e1-pos_b1);//vc的stl中pos_b1不能小于pos_e1//java中的substring的第2个参数是指终止字符的位置;而stl的则是要复制的长度,如果这个长度太大就会异常
	  fs1 = substring_JAVA(in1, pos_b1, pos_e1);
      //..end;

      //然后再截取这个子字符串sp_end1前的部分
      pos_e1=fs1.find(sp_end1);
      pos_b1=0; //起始位置必须还原为零
      if (sp_end1.length()==0)
      {
        pos_e1=in1.length();
      }

      //如果两个字符串都不存在,则返回空串
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
	  std::string in1 = lcase(src);//用in1来查找字符串的位置,最后截取时仍然访问的是src//这样可以忽略大小写又不损坏原有的数据
	  sp_begin1 = lcase(sp_begin1);
	  sp_end1 = lcase(sp_end1);

	  std::string r = "";

      int pos_b1,pos_e1; //要得到的字符串的首尾位置

      //先找pos_b1
      if (sp_begin1.length()==0)//如果第1个分隔符为空就表示取第2个分隔符之前的字符串
      {
        pos_b1=0;
      }
	  else//第一个分隔符不为空时
	  {
		pos_b1=in1.find(sp_begin1);
		if (pos_b1 == -1) return "";//第一个分隔符不为空，而且找不到时返回值就是空

		pos_b1 += sp_begin1.length();//还要加上分隔符本身的长度
	  }

      //从后pos_b1开始找pos_e1
      pos_e1=in1.find(sp_end1, pos_b1);
      if (sp_end1.length()==0)//如果第2个分隔符为空就表示取第1个分隔符之后的字符串
      {
        pos_e1=in1.length();
      }

      //如果两个字符串都不存在,则返回空串
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

		if( (line.size() > 0) && (line[0]=='#') ) continue;//跳过注释

		key = lcase(get_value1(line, "", "="));
		value = get_value1(line, "=", "");

		k[key] = value;

		continue;

	}
}

//从文件中加载键值对//象apache一样[#]号开始的作为注释
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

		if( (line.size() > 0) && (line[0]=='#') ) continue;//跳过注释

		key = get_value1(line, "", "=");;
		value = get_value1(line, "=", "");

		k[key] = value;

		continue;

	}
	
}

//保存键值对
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


//判断某个键是否在列表中
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

//避免 NULL 赋值崩溃,引用计数等
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

//直接用[]取值是不安全的,所以要有一个函数
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
		sprintf(buf, "%02X", (unsigned char)c);//要用这个才行，要不 1 会转换为 "1" 而不是我们要求的 "01"
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
	if ((s.size() % 2)!=0)//必须是2的倍数
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

//检查是否是安全的字符串//下划线,数字,字母
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

//字符串是否相同[大小写不敏感]
bool uncase_cmp(const char * buf,const char * cmd)
{
	//命令比字符串长就是不对了
	if (strlen(buf) < strlen(cmd)) return false;

	//每个字符都比较一下
	for (int i=0;i<strlen(cmd);i++)
	{
		//有一个字符不一样就是不同的
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

//忽略大小写的比较
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

//得到文件后缀名
std::string extract_file_ext(const std::string fn)
{
	size_t pos = fn.rfind(".");
	if (pos == std::string::npos) return "";

	return fn.substr(pos);
}

//得到文件名
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
//如果要用 md5.h
#ifdef __USE_MD5__
//md5编码
std::string md5(const std::string strA1)
{
	unsigned char szMD5[16];
	char outbuf[3];

	MD5_CTX md5;

	MD5Init( &md5 );
	MD5Update( &md5, (unsigned char *)strA1.c_str( ), (unsigned int)strA1.size( ) );
	MD5Final( szMD5, &md5 );

	//return std::string( (char *)szMD5, 16 );//注意这个方法//这样可以不考虑是否有结尾的"\0"字符了
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

//如果要用 gd 库/////////////////////////////////
#ifdef __USE_GD__


//为了效率要用 gdFree 释放返回值
char * resize_imagett(const std::string fn, int new_W, int new_H, int & size)
{
		FILE * old_fp = NULL;
		FILE * new_fp = NULL;
		gdImagePtr old_im, new_im; //图片旧、新图片句柄
		//char * old_fn = "C:\\Documents and Settings\\Administrator\\桌面\\1.jpg";
		//char * new_fn = "C:\\Documents and Settings\\Administrator\\桌面\\2.jpg";
		char * old_fn = "C:\\Documents and Settings\\wilson\\桌面\\1.jpg";
		char * new_fn = "C:\\Documents and Settings\\wilson\\桌面\\2.jpg";
		int A4_W = 40;//?
		int A4_H = 40;//?
		new_W = 100;//新宽度
		new_H = 100;//新高度

		/*
		if((old_fp=fopen(old_fn, "rb"))!=NULL)
		{ //打开原图片文件
			old_im=gdImageCreateFromJpeg(old_fp);//取得原图片句柄
			new_im = gdImageCreate(A4_W,A4_H); //创建新图片，取得句柄

			int white = gdImageColorAllocate(new_im, 255, 255, 255); //白色,做背景色
			int black = gdImageColorAllocate(new_im, 0, 0, 0); //黑色,做前景色

			gdImageCopyResized(new_im,old_im,0,0,0,0,new_W,new_H,old_im->sx,old_im->sy);//复制并缩放图片,new_W,new_H,新图片的宽高
			if((new_fp=fopen(new_fn,"wb"))!=NULL)
			{ //打开新文件
				gdImageJpeg(new_im,new_fp,-1); //输出图片到文件
				fclose(new_fp);
			};
			fclose(old_fp);
		};
		*/

		if((old_fp=fopen(old_fn, "rb"))!=NULL)
		{ //打开原图片文件
			old_im=gdImageCreateFromJpeg(old_fp);//取得原图片句柄

		}

	return "";
}


//为了效率要用 gdFree 释放返回值//type 0 - jpg, 1 - png
char * resize_image(const std::string fn, int new_W, int new_H, int & size, int type, int quality)
//char * resize_image(const std::string fn, int new_W, int new_H, long & size, int type, int quality)
{

	FILE * old_fp = NULL;
	FILE * new_fp = NULL;
	gdImagePtr old_im = NULL;
	gdImagePtr new_im = NULL; //图片旧、新图片句柄

	char * old_fn = (char *)fn.c_str();//"C:\\Documents and Settings\\Administrator\\桌面\\1.jpg";
	//char * old_fn = "C:\\Documents and Settings\\wilson\\桌面\\1.jpg";


	//char * new_fn = "C:\\Documents and Settings\\Administrator\\桌面\\2.jpg";
	//int A4_W = new_W;//40;//?
	//int A4_H = new_H;//40;//?
	//int new_W = 100;//新宽度
	//int new_H = 100;//新高度
	char * data = NULL;

	log_cur(log_file);
	//缩放//能否保存在内存中
	if((old_fp=fopen(old_fn, "rb"))!=NULL)
	{ //打开原图片文件
	log_cur(log_file);
		old_im=gdImageCreateFromJpeg(old_fp);//取得原图片句柄

	log_cur(log_file);
		if (old_im == NULL)
			return NULL;
	log_cur(log_file);

		//计算高度//临时

		//old_im->sx;//宽度
		//old_im->sy;//高度

		new_H = new_W * (((double)old_im->sy)/(double)(old_im->sx));
		
		//计算高度//临时 _end;

		//一定要销毁旧图片，以免内存泄露
		fclose(old_fp);
		//放在后面//gdImageDestroy(old_im);

		//new_im = gdImageCreate(A4_W,A4_H); //创建新图片，取得句柄
//		new_im = gdImageCreate(new_W, new_H); //创建新图片，取得句柄
		new_im = gdImageCreateTrueColor(new_W, new_H); //创建新图片，取得句柄
		

		//int white = gdImageColorAllocate(new_im, 255, 255, 255); //白色,做背景色
		//int black = gdImageColorAllocate(new_im, 0, 0, 0); //黑色,做前景色

		//gdImageCopyResized 的效果大家都说不好
//		gdImageCopyResized(new_im,old_im,0,0,0,0,new_W,new_H,old_im->sx,old_im->sy);//复制并缩放图片,new_W,new_H,新图片的宽高
		gdImageCopyResampled(new_im,old_im,0,0,0,0,new_W,new_H,old_im->sx,old_im->sy);//复制并缩放图片,new_W,new_H,新图片的宽高
		/*
		if((new_fp=fopen(new_fn,"wb"))!=NULL)
		{ //打开新文件
			gdImageJpeg(new_im,new_fp,-1); //输出图片到文件
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
				quality = 85;//压缩率
			}
			//data = (char *) gdImageJpegPtr(new_im, &size, -1);//最后一个参数是什么意思?// -1 的效果非常不好，85 比较好
			data = (char *) gdImageJpegPtr(new_im, &size, quality);//最后一个参数是什么意思?
		}

		if (!data)
		{
			//Error
			return NULL;
		}


	};
	log_cur(log_file);


	//一定要销毁旧图片，以免内存泄露
	//fclose(old_fp);
	//放在后面//
	if (old_im != NULL)
	{
		gdImageDestroy(old_im);
	}

	if (new_im != NULL)
	{
		gdImageDestroy(new_im);//这个会不会导致 data 失效呢?
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
	//注意外层函数一定要调用这个//gdFree(data); //不用，直接 free

}

#endif
//如果要用 gd 库/////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////
//只有 windows 下才能用的函数
#ifdef WIN32

//windows时间转换为time_t//来自https://ccvs.cvshome.org/source/browse/ccvs/windows-NT/JmgStat.c?rev=1.3&content-type=text/vnd.viewcvs-markup
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

// GB2312 转换成 utf-8
std::string gbk_to_utf8(std::string src) 
{
	//int len = src.size() * 2 +1;//大小必须至少大两倍
	int len = (src.size()+1) * 4;//4倍
	char * p_out = (char *)malloc(len);//LPWSTR
	char * p_out_utf8 = (char *)malloc(len);
	memset(p_out, 0, len); 
	memset(p_out_utf8, 0, len); 

	//gbk 转换成 unicode
	//int word_count = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src.c_str(), src.size(), (WCHAR *)p_out, src.size());
	int word_count = ::MultiByteToWideChar(936, MB_PRECOMPOSED, 
		src.c_str(), 
		src.size(), // number of bytes in string//字节数
		(WCHAR *)p_out, 
		len//src.size()//size of buffer//注意是缓冲区字节数，不是宽字符的个数
		);
	//::MultiByteToWideChar(936, MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);//936 是中文字符集

	//uncode 转换成 CP_UTF8
	//::WideCharToMultiByte(CP_UTF8, WC_COMPOSITECHECK, (WCHAR *)p_out, word_count, p_out_utf8, len,
	//用 WC_COMPOSITECHECK 参数是转换不了的
	word_count = ::WideCharToMultiByte(CP_UTF8, 0, 
		(WCHAR *)p_out, 
		word_count, // number of chars in string//宽字符的个数
		p_out_utf8, 
		len,// size of buffer//缓冲区的大小
		NULL, NULL);//CP_UTF8 UTF-8，设置此值时lpDefaultChar和lpUsedDefaultChar都必须为NULL

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

// GB2312 转换成 unicode
std::wstring gbk_to_unicode(std::string src) 
{
	//int len = src.size() * 2 +1;//大小必须至少大两倍
	int len = (src.size()+1) * 4;//4倍
	WCHAR * p_out = (WCHAR *)malloc(len);//LPWSTR
	memset(p_out, 0, len); 

	//gbk 转换成 unicode
	//int word_count = ::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, src.c_str(), src.size(), (WCHAR *)p_out, src.size());
	int word_count = ::MultiByteToWideChar(936, MB_PRECOMPOSED, 
		src.c_str(),
		src.size(), // number of bytes in string//字节数
		p_out, 
		len//src.size()//size of buffer//注意是缓冲区字节数，不是宽字符的个数
		);

	std::wstring s = p_out;

	s.resize(word_count);//test//其实是上一句有问题，暂时用这个解决

	free(p_out);
	return s; 

}


// unicode 转换成 GB2312
std::string unicode_to_gbk(std::wstring src) 
{
	//int len = src.size() * 2 +1;//大小必须至少大两倍
	int len = (src.size()+1) * 4;//4倍
	char * p_out_utf8 = (char *)malloc(len);
	memset(p_out_utf8, 0, len); 

	WCHAR * p_out = (WCHAR *)src.c_str();

	//uncode 转换成 gbk
	//int word_count = ::WideCharToMultiByte(936, 0, (WCHAR *)p_out, word_count, p_out_utf8, len,
	int word_count = ::WideCharToMultiByte(936, 0, (WCHAR *)p_out, src.size(), p_out_utf8, len,
		NULL, NULL);

	std::string s = p_out_utf8;

	//free(p_out);
	free(p_out_utf8);
	return s; 

}

//取前几个字符,注意是汉字的，不能分开一个汉字
std::string left_string_cn(std::string src, int count)
{
	std::wstring ws = gbk_to_unicode(src);

	int src_count = ws.size();

	if (count > src_count) count = src_count;//不能越界

	ws = substring_STL(ws, 0, count);

	//ws = ws.substr(0, count);//test

	std::string s = unicode_to_gbk(ws);

	return s;

}

//取前几个字符,注意是汉字的，不能分开一个汉字
std::string sub_string_cn(std::string src, int begin, int count)
{
	std::wstring ws = gbk_to_unicode(src);

	int src_count = ws.size();

	if (count > src_count) count = src_count;//不能越界

	//ws = substring_STL(ws, 0, count);
	ws = substring_STL(ws, begin, count);

	//ws = ws.substr(0, count);//test

	std::string s = unicode_to_gbk(ws);

	return s;

}

//windows时间转换为time_t
time_t get_time_t( const FILETIME ft )
{
  time_t t;
  FileTimeToUnixTime (&ft, &t, true);
  return t;
}

std::string get_app_filename()
{
	std::string r = "";

	tmem t(MAX_PATH);//自释放临时内存
	char * buf = t.buf;

	::GetModuleFileName(NULL, buf, MAX_PATH);
	r = buf;

	//printf("%s\r\n", r.c_str());

	return r;
}

//只保留合法字符//中文也会被清除
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



