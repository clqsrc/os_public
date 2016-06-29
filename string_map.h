#pragma once

#include <string>
#include <map>


//���� std::map<std::string, std::string>  ��Σ�յ� [] ����,�Լ����µı�����
class string_map
{
public:
	string_map(void)
	{}

	~string_map(void)
	{}

private:
	std::map<std::string, std::string> data;//ʵ������
	//std::map<std::string, std::string>::iterator iter;//������
	
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
	}//


public:
	std::map<std::string, std::string>::iterator iter;//������//ע�����Ƿ��̰߳�ȫ��

	//���� System::String ��ֵ�ĵط�//���� this->Caption = dstring;
	//���԰� System::String ����һ���������������//����д�Ϸ���ֵ
	operator std::map<std::string, std::string> &()//�� operator std::string () �Ƿ����������,������û���ٸ���һ��ʵ��?
	{
		return data;
	}//

	//--------------------------------------------------
	//���������㺯��//ע�����Ƿ��̰߳�ȫ��
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
			//iter++;//Ӧ������ while ѭ������,���Է��ں���

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
			//iter++;//Ӧ������ while ѭ������,���Է��ں���

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
		//��˵�� vs2008 �� vs2005 ���ǲ��Ե�,Ҫ�� erase �ķ���ֵ���Ǳ�׼//data.erase(iter++);//������ erase(++iter); ��Ϊ��ʱ�� iter �Ѿ�ʧЧ��
		iter = data.erase(iter);
	}//

	//--------------------------------------------------

	//ֱ����[]ȡֵ�ǲ���ȫ��,����Ҫ��һ������
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
		
		return std::string("");//ʧ�ܵĻ����ؿ��ַ���

	}//


};


