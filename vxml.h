#ifndef _V_XML_H__
#define _V_XML_H__

//取消VC中使用STL的警告
#pragma warning(disable:4503)
#pragma warning(disable:4786)

#include <string>
#include <vector>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>



//操作 xml 的虚拟类，允许用不同的 xml 底层库，但实现的接口是相同的。

//节点
typedef xmlNodePtr CVXmlNode;
//同一级的节点列表
typedef std::vector<xmlNodePtr> CVXmlNodeList;

//	xmlDocPtr doc;
//	xmlNodePtr cur;
//	xmlNodePtr root;



class CVXml
{
public://变量
	xmlDocPtr m_Doc;//文档
	xmlNodePtr m_Root;//根节点

public://函数
	//装载文件
	void CVXml::LoadFromFile(std::string fn);

	//查找下一个指定名称的节点
	CVXmlNode CVXml::FindNode(std::string name);

	//得到节点的下一级所有节点//如果参数为 NULL 则为查找要节点
	void CVXml::GetSubList(CVXmlNode cur, CVXmlNodeList & list);

	//得到节点的下一级所有节点//如果参数为 NULL 则为查找要节点//指定名称的，因为 libxml2 会得到一些莫名其妙的
	void CVXml::GetSubList(CVXmlNode cur, CVXmlNodeList & list, const std::string key);

	//得到一个节点下某个子节点的内容//按索引
	std::string CVXml::GetSubNodeString(CVXmlNode node, int index);

	//得到一个节点下某个子节点的内容//按名称
	std::string CVXml::GetSubNodeString(CVXmlNode node, std::string name);

private:
	//要从 utf8 转换成 gbk
	std::string CVXml::ToGbk(xmlChar * utf8);



};







#endif



