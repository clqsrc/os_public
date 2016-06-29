#pragma once

#include <string>

//把字符串当做数字运算的类

class nstring//数字字符串类//numeric string
{
public:
	nstring(void)
	{
		data = "";
	}

	~nstring(void);

private:

	std::string data;

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
	}//

public:



	//返回类型 operator 运算符符号 (参数说明)
	//{
	////函数体的内部实现
	//} 

	std::string operator + (int v)
	{
		//函数体的内部实现
		int r = str_to_int(this->data) + v;

		return int_to_str(r);
	}//
	std::string operator - (int v)
	{
		//函数体的内部实现
		int r = str_to_int(this->data) - v;

		return int_to_str(r);
	}//

	nstring(std::string v)
	{
		data = v;
	}//

	////代替 System::String 赋值的地方//例如 this->Caption = dstring;
	////可以把 System::String 当作一个运算符号来重载//不用写上返回值
	//operator System::String();

	//代替 System::String 赋值的地方//例如 this->Caption = dstring;
	//可以把 System::String 当作一个运算符号来重载//不用写上返回值
	operator std::string &()//和 operator std::string () 是否是有区别的,可能是没有再复制一个实例?
	{
		return data;
	}

	operator const char *()
	{
		return data.c_str();
	}

};
