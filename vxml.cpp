
#include "vxml.h"
#include "os.h"
#include "public_function.h"
#include "tmem.h"


//���� xml �������࣬�����ò�ͬ�� xml �ײ�⣬��ʵ�ֵĽӿ�����ͬ�ġ�


//���ڵݹ���ҵ��Ӻ���
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

		//������һ��
		r = SubFindNode(cur->children, name);
		if (r != NULL)
		{
			return r;
		}

		cur = cur->next;
	}

	return NULL;
}


//������һ��ָ�����ƵĽڵ�//������ xml �ļ��������ң�������Ŀ¼·��
CVXmlNode CVXml::FindNode(std::string name)
{
	xmlNodePtr cur = m_Root;
	xmlNodePtr r = NULL;

	r = SubFindNode(cur, name);

	return r;
}

//װ���ļ�
void CVXml::LoadFromFile(std::string fn)
{
	//װ�������ļ�
	xmlNodePtr cur;

	m_Doc = xmlParseFile(fn.c_str());

	if (m_Doc == NULL)
	{
		printf_error("�����ļ������ڡ�\r\n");
	}

	m_Root = xmlDocGetRootElement(m_Doc);//ȡ�õ�һ���ڵ�
	cur = m_Root;

	while (cur != NULL) 
	{
		printf("name:%s\r\n", cur->name);
		cur = cur->next;

		//cur->
	}
}


//�õ��ڵ����һ�����нڵ�//�������Ϊ NULL ��Ϊ����Ҫ�ڵ�
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

//�õ��ڵ����һ�����нڵ�//�������Ϊ NULL ��Ϊ����Ҫ�ڵ�//ָ�����Ƶģ���Ϊ libxml2 ��õ�һЩĪ�������
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



//�õ�һ���ڵ���ĳ���ӽڵ������//������
std::string CVXml::GetSubNodeString(CVXmlNode node, int index)
{//���ԣ���ʱ�����ô˺���
	CVXmlNode cur = node;
	std::string r = "";
	r = (const char *)xmlNodeListGetString(m_Doc, cur->children, index);

	return r;
}



//����ת��:��һ�ֱ���תΪ��һ�ֱ���
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




//UNICODE��תΪGB2312��
int u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
	return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}

//GB2312��תΪUNICODE��
int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
	return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
}


//Ҫ�� utf8 ת���� gbk
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

	char * in_utf8 = (char *)utf8;//"正�?��??�?";
	char * in_gb2312 = "���ڰ�װ";
	tmem tmp(strlen((const char *)utf8)*4 + 1);

	//unicode��תΪgb2312��
	int rc = u2g(in_utf8,strlen(in_utf8), tmp.buf, tmp.buf_len);
	//printf("unicode-->gb2312 out=%sn",out);
	//gb2312��תΪunicode��
	//rc = g2u(in_gb2312,strlen(in_gb2312),tmp.buf, tmp.buf_len);

	r = tmp.buf;

	return r;
}



//�õ�һ���ڵ���ĳ���ӽڵ������//������
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
			//�Źֵ��﷨��Ҫ����ȡ�ñ�־�������
			xmlChar * tmp = xmlNodeListGetString(m_Doc, cur->children, 1);
			r = (const char *)tmp;

			//�� utf8 ��Ҫת���� gbk
			//UTF8Toisolat1
			r = ToGbk(tmp);

			xmlFree(tmp);//ע�⣡�����ͷ�

			return r;
		}


		cur = cur->next;
	}

	return r;
}






