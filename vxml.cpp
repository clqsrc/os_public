
#include "vxml.h"
#include "os.h"
#include "public_function.h"
#include "tmem.h"


//操作 xml 的虚拟类，允许用不同的 xml 底层库，但实现的接口是相同的。


//用于递归查找的子函数
const xmlNodePtr SubFindNode(const xmlNodePtr node, const std::string name)
{
	xmlNodePtr r = NULL;
	xmlNodePtr cur = node;//node->children;

	while (cur != NULL) 
	{
		printf("name:%s\r\n", cur->name);

		//if ((!xmlStrcmp(cur->name, (const xmlChar *) (name.c_str()))))
		if (str_eq((const char *)cur->name, name) == true)
		{
			return cur;
		}

		//查找下一级
		r = SubFindNode(cur->children, name);
		if (r != NULL)
		{
			return r;
		}

		cur = cur->next;
	}

	return NULL;
}


//查找下一个指定名称的节点//在整个 xml 文件中向下找，含有子目录路径
CVXmlNode CVXml::FindNode(std::string name)
{
	xmlNodePtr cur = m_Root;
	xmlNodePtr r = NULL;

	r = SubFindNode(cur, name);

	return r;
}

//装载文件
void CVXml::LoadFromFile(std::string fn)
{
	//装载配置文件
	xmlNodePtr cur;

	m_Doc = xmlParseFile(fn.c_str());

	if (m_Doc == NULL)
	{
		printf_error("配置文件不存在。\r\n");
	}

	m_Root = xmlDocGetRootElement(m_Doc);//取得第一个节点
	cur = m_Root;

	while (cur != NULL) 
	{
		printf("name:%s\r\n", cur->name);
		cur = cur->next;

		//cur->
	}
}


//得到节点的下一级所有节点//如果参数为 NULL 则为查找要节点
void CVXml::GetSubList(CVXmlNode cur, CVXmlNodeList & list)
{
	if (cur == NULL) 
		cur = m_Root;

	cur = cur->children;

	list.clear();
	while (cur != NULL) 
	{
		//printf("name:%s\r\n", cur->name);
		
		list.push_back(cur);
		
		cur = cur->next;
	}
}

//得到节点的下一级所有节点//如果参数为 NULL 则为查找要节点//指定名称的，因为 libxml2 会得到一些莫名其妙的
void CVXml::GetSubList(CVXmlNode cur, CVXmlNodeList & list, const std::string key)
{
	if (cur == NULL) 
		cur = m_Root;

	cur = cur->children;

	list.clear();
	while (cur != NULL) 
	{
		//printf("name:%s\r\n", cur->name);
		if (str_eq((const char *)cur->name, key) == true)
		{	
			list.push_back(cur);
		}
		
		cur = cur->next;
	}
}



//得到一个节点下某个子节点的内容//按索引
std::string CVXml::GetSubNodeString(CVXmlNode node, int index)
{//不对，暂时不能用此函数
	CVXmlNode cur = node;
	std::string r = "";
	r = (const char *)xmlNodeListGetString(m_Doc, cur->children, index);

	return r;
}



//代码转换:从一种编码转为另一种编码
int code_convert(char *from_charset,char *to_charset,char *inbuf,int inlen,char *outbuf,int outlen)
{
	iconv_t cd;
	//int rc;
	char **pin = &inbuf;
	char **pout = &outbuf;

	cd = iconv_open(to_charset,from_charset);
	if (cd==0) return -1;
	memset(outbuf,0,outlen);
	if (iconv(cd, (const char **)pin, (unsigned int *)&inlen, pout, (unsigned int *)&outlen) == -1) return -1;
	iconv_close(cd);
	return 0;
}




//UNICODE码转为GB2312码
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}

//GB2312码转为UNICODE码
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}


//要从 utf8 转换成 gbk
std::string CVXml::ToGbk(xmlChar * utf8)
{
	std::string r = "";

	/*
	char * encoding = "ISO-8859-1";//"gb2312";//"GBK";

	xmlCharEncodingHandlerPtr handler = NULL; 
	int size = strlen((const char *)utf8)+1; 
	int out_size = (size * 2) -1; 
	char * out = (char *)malloc((size_t)out_size); 
	handler = xmlFindCharEncodingHandler(encoding); 
	int temp = 0;
	handler->input((unsigned char *)out, &out_size, utf8, &temp); 
	//xmlSaveFormatFileEnc("-", doc, encoding, 1);

	r = out;
	*/

	char * in_utf8 = (char *)utf8;//"姝ｅ?ㄥ??瑁?";
	char * in_gb2312 = "正在安装";
	tmem tmp(strlen((const char *)utf8)*4 + 1);

	//unicode码转为gb2312码
	int rc = u2g(in_utf8,strlen(in_utf8), tmp.buf, tmp.buf_len);
	//printf("unicode-->gb2312 out=%sn",out);
	//gb2312码转为unicode码
	//rc = g2u(in_gb2312,strlen(in_gb2312),tmp.buf, tmp.buf_len);

	r = tmp.buf;

	return r;
}



//得到一个节点下某个子节点的内容//按名称
std::string CVXml::GetSubNodeString(CVXmlNode node, std::string name)
{
	if (node == NULL)
		return "";
	
	std::string r = "";
	CVXmlNode cur = node;
	cur = cur->children;

	while (cur != NULL) 
	{
		//printf("name:%s\r\n", cur->name);

		//if ((!xmlStrcmp(cur->name, (const xmlChar *) (name.c_str()))))
		if (str_eq((const char *)cur->name, name) == true)
		{
			//r = (const char *)cur->name;
			//古怪的语法，要这样取得标志间的内容
			xmlChar * tmp = xmlNodeListGetString(m_Doc, cur->children, 1);
			r = (const char *)tmp;

			//是 utf8 还要转换成 gbk
			//UTF8Toisolat1
			r = ToGbk(tmp);

			xmlFree(tmp);//注意！必须释放

			return r;
		}


		cur = cur->next;
	}

	return r;
}






