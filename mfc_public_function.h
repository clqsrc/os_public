#ifndef _MFC_PUBLIC_FUNCTION_H_
#define _MFC_PUBLIC_FUNCTION_H_

//mfc 的公用函数//mfc 比较啰嗦,有个公用函数比较好

//取消VC中使用STL的警告
#pragma warning(disable:4503)
#pragma warning(disable:4786)


#include <iostream>
#include <vector>
#include <map>
#include <string>

//#include "stdafx.h"
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

//取消VC中使用STL的警告
//#pragma warning(disable:4503)
//#pragma warning(disable:4786)

//对应 CTabCtrl 的虚拟操作类
typedef std::map<int, CDialog *> VTabCtrlPages;//键值对

class VTabCtrl
{
public:
	//增加一个页面
	static void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption);

	//增加一个页面//在主对话框的OnInitDialog()函数中
	static void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption, CDialog * dlg, UINT dlgID);

	//激活一个页面
	//在主对话中为标签控件添加一个标签选择改变（TCN_SELCHANGE）的控件通知消息，以便在用户选择标签时通知主对话框。在主对话框的编辑界面右击标签控件，选择添加一个事件可以完成这个操作。     
	//  在事件处理中添加如下代码，如下例：     
	//  void   CtabdialogDlg::OnTcnSelchangeTab1(NMHDR   *pNMHDR,   LRESULT   *pResult)  

	static void VTabCtrl::OnSelChange(NMHDR * pNMHDR, LRESULT * pResult, CTabCtrl * ctrl, VTabCtrlPages & pages);

	//显示某一页
	static void VTabCtrl::ShowPage(CTabCtrl * ctrl, VTabCtrlPages & pages, int sel);

	//清空
	static void VTabCtrl::Clear(CTabCtrl * ctrl);
	
};


//文件对话框//http://dev.csdn.net/article/13/13461.shtm
class VFileDialog
{
public:		
	static std::string VFileDialog::GetOpenFileName(std::string filter);
	//得到一个路径
	static std::string VFileDialog::GetPath(bool newDir);

};

//CImageList
class VImageList
{
public:
	//增加一个
	static void VImageList::AddImage(CImageList * ctrl, UINT idImage);
	static void VImageList::Create(CImageList * ctrl, int countImage);
};

//CTreeCtrl
//TV_ITEM.pszText 是要预先分配内存的,所以弄个简单的保存类
class VTreeNode
{
public:
	TV_ITEM tvi;
	char szText[250];

	VTreeNode()
	{
		memset(&tvi, 0, sizeof(TVITEM));
		memset(&szText, 0, sizeof(TVITEM));
	}
};

class VTreeCtrl
{
public:
	//失去焦点时是否显示当前选择项
	static void VTreeCtrl::SetStyleHideSel(CTreeCtrl * ctrl, bool hide);
	//删除所有子节点
	static void VTreeCtrl::DeleteChildren(CTreeCtrl * ctrl, HTREEITEM item);
	//用线显示层次关系
	static void VTreeCtrl::SetStyleShowLines(CTreeCtrl * ctrl);
	//取父节点//如果 return false 则是没有父节点
	static bool VTreeCtrl::GetParentItem(CTreeCtrl * ctrl, HTREEITEM item, VTreeNode * node);

};

#endif




