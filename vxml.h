#ifndef _V_XML_H__
#define _V_XML_H__

//ȡ��VC��ʹ��STL�ľ���
#pragma warning(disable:4503)
#pragma warning(disable:4786)

#include <string>
#include <vector>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>



//���� xml �������࣬�����ò�ͬ�� xml �ײ�⣬��ʵ�ֵĽӿ�����ͬ�ġ�

//�ڵ�
typedef xmlNodePtr CVXmlNode;
//ͬһ���Ľڵ��б�
typedef std::vector<xmlNodePtr> CVXmlNodeList;

//	xmlDocPtr doc;
//	xmlNodePtr cur;
//	xmlNodePtr root;



class CVXml
{
public://����
	xmlDocPtr m_Doc;//�ĵ�
	xmlNodePtr m_Root;//���ڵ�

public://����
	//װ���ļ�
	void CVXml::LoadFromFile(std::string fn);

	//������һ��ָ�����ƵĽڵ�
	CVXmlNode CVXml::FindNode(std::string name);

	//�õ��ڵ����һ�����нڵ�//�������Ϊ NULL ��Ϊ����Ҫ�ڵ�
	void CVXml::GetSubList(CVXmlNode cur, CVXmlNodeList & list);

	//�õ��ڵ����һ�����нڵ�//�������Ϊ NULL ��Ϊ����Ҫ�ڵ�//ָ�����Ƶģ���Ϊ libxml2 ��õ�һЩĪ�������
	void CVXml::GetSubList(CVXmlNode cur, CVXmlNodeList & list, const std::string key);

	//�õ�һ���ڵ���ĳ���ӽڵ������//������
	std::string CVXml::GetSubNodeString(CVXmlNode node, int index);

	//�õ�һ���ڵ���ĳ���ӽڵ������//������
	std::string CVXml::GetSubNodeString(CVXmlNode node, std::string name);

private:
	//Ҫ�� utf8 ת���� gbk
	std::string CVXml::ToGbk(xmlChar * utf8);



};







#endif



