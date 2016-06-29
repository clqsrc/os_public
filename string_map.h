#pragma once

#include <string>
#include <map>


//代替 std::map<std::string, std::string>  中危险的 [] 操作,以及啰嗦的遍历等
class string_map
{
public:
	string_map(void)
	{}

	~string_map(void)
	{}

private:
	std::map<std::string, std::string> data;//实际数据
	//std::map<std::string, std::string>::iterator iter;//迭代器
	
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
	}//


public:
	std::map<std::string, std::string>::iterator iter;//迭代器//注意其是非线程安全的

	//代替 System::String 赋值的地方//例如 this->Caption = dstring;
	//可以把 System::String 当作一个运算符号来重载//不用写上返回值
	operator std::map<std::string, std::string> &()//和 operator std::string () 是否是有区别的,可能是没有再复制一个实例?
	{
		return data;
	}//

	//--------------------------------------------------
	//迭代器方便函数//注意其是非线程安全的
	std::map<std::string, std::string>::iterator first()
	{
		iter = data.begin();

		//map <string,int>   words;
		//map <string,int> ::iterator   it=words.begin();
		//for(;it!=words.end();++it)
		//		  cout < < "key: " < <it-> first
		//				  < < "value: " < <it-> second < <end1;
		//return   0;

		return iter;
	}//

	bool next(std::string & key, std::string & value)
	{
		if (iter != data.end())
		{
			//iter++;//应当是在 while 循环里用,所以放在后面

			key = iter->first;
			value = iter->second;

			iter++;
		}
		else
		{
			return false;
		}

		return true;
	}//

	bool next()
	{
		if (iter != data.end())
		{
			//iter++;//应当是在 while 循环里用,所以放在后面

			//key = iter->first;
			//value = iter->second;

			iter++;
		}
		else
		{
			return false;
		}

		return true;
	}//

	void delete_current()
	{
		//据说在 vs2008 和 vs2005 下是不对的,要用 erase 的返回值才是标准//data.erase(iter++);//不能是 erase(++iter); 因为这时候 iter 已经失效了
		iter = data.erase(iter);
	}//

	//--------------------------------------------------

	//直接用[]取值是不安全的,所以要有一个函数
	std::string operator[](std::string key)
	{
		std::string value = "";
		std::map<std::string, std::string>::iterator iter;

		iter = data.find(key);
		if(iter != data.end())
		{	
			value = iter->second;
			return value;
		}
		
		return std::string("");//失败的话返回空字符串

	}//


};


