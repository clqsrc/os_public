
//取消VC中使用STL的警告
#pragma warning(disable:4503)
#pragma warning(disable:4786)

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include "mfc_public_function.h"

//取消VC中使用STL的警告
//#pragma warning(disable:4503)
//#pragma warning(disable:4786)

//CTabCtrl
//增加一个页面//在主对话框的OnInitDialog()函数中
void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption)
{
	TCITEM   item;
	item.mask   =   TCIF_TEXT;
	item.pszText   =  (char *) caption.c_str();//"第一页";

	//ctrl->InsertItem(1, &item);
	ctrl->InsertItem(20, &item);//感觉第一个参数表示插入到哪个地方,开大点就能插入到后面去
}

//增加一个页面//在主对话框的OnInitDialog()函数中
void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption, CDialog * dlg, UINT dlgID)
{
	VTabCtrl::AddPage(ctrl, caption);

	//实际上是在一个将一个窗体建立为另一个窗体的子窗体
	//对话框资源的属性必须是  
	//Border: none   //边界为空,这样它就没了标题栏   
	//Style: Child   //这样这个模板就可以当作另一个窗口的子窗口了
	//dlg->Create(dlgID, ctrl->GetParent());
	dlg->Create(dlgID, ctrl);//这样也是可以的,就是不知道是否影响消息循环
}

//清空
void VTabCtrl::Clear(CTabCtrl * ctrl)
{
	ctrl->DeleteAllItems();
}



//激活一个页面
//在主对话中为标签控件添加一个标签选择改变（TCN_SELCHANGE）的控件通知消息，以便在用户选择标签时通知主对话框。在主对话框的编辑界面右击标签控件，选择添加一个事件可以完成这个操作。     
//  在事件处理中添加如下代码，如下例：     
//  void   CtabdialogDlg::OnTcnSelchangeTab1(NMHDR   *pNMHDR,   LRESULT   *pResult)  

void VTabCtrl::OnSelChange(NMHDR * pNMHDR, LRESULT * pResult, CTabCtrl * ctrl, VTabCtrlPages & pages)
{   

	int sel = ctrl->GetCurSel();

	ShowPage(ctrl, pages, sel);

	*pResult = 0;   
}

//显示某一页
void VTabCtrl::ShowPage(CTabCtrl * ctrl, VTabCtrlPages & pages, int sel)
{   
	CRect r;   
	ctrl->GetClientRect(&r);   
	CDialog * dlg = NULL;

	//显示
	//dlg->SetWindowPos(NULL, 10, 30, r.right-20, r.bottom-40, SWP_SHOWWINDOW);   

	//其他的隐藏
	//std::map<std::string, std::string>::iterator iter;
	VTabCtrlPages::iterator iter;
	iter = pages.begin();
	for (iter = pages.begin(); iter != pages.end();)
	{
		dlg = iter->second;

		if (dlg != NULL)
		{
			dlg->SetWindowPos(NULL, 10, 30, r.right-20, r.bottom-40, SWP_HIDEWINDOW );	
		}
	
		iter++;
	}

	//m_mm2.SetWindowPos   (NULL,10,30,r.right   -20,r.bottom   -40,SWP_HIDEWINDOW   );       

	//显示
	dlg = pages[sel];
	if (dlg != NULL)
	{
		dlg->SetWindowPos(NULL, 10, 30, r.right-20, r.bottom-40, SWP_SHOWWINDOW);
		//dlg->ShowWindow(SW_SHOW);//产生 wm_show 事件//不行
		::PostMessage(dlg->m_hWnd, WM_SHOWWINDOW, 0, 0);//ok
	}

}



//文件对话框//http://dev.csdn.net/article/13/13461.shtm
std::string VFileDialog::GetOpenFileName(std::string filter)
{
	/*
	CFileDialog dlg(FALSE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,
		"All Files(*.*)|*.*| |",AfxGetMainWnd());
		
	if(dlg.DoModal()==IDOK)
	{
		//strPath=dlg.GetPathName();
		//if(strPath.Right(4)!=".txt")
		//    strPath+=".txt";
	}

	lpstrFilter 
	指向一对以空字符结束的过滤字符串的一个缓冲。缓冲中的最后一个字符串必须以两个NULL字符结束。 

	第一个字符串是过滤器描述的显示字符串（例如，“文本文件”），第二个字符指定过滤样式（例如，“*.TXT”）。要为一个显示字符串指定多个过滤样式，使用分号（“;”）分隔样式（例如，“*.TXT;*.DOC;*.BAK”）。一个样式字符串中可以包含有效的文件名字字符及星号（*）通配符。不能在样式字符串中包含空格。 

	系统不能改变过滤器的次序。它按lpstrFilter指定的次序显示在文件类型组合框中。 

	如果lpstrFilter是NULL，对话框不能显示任何过滤器。
	*/

	char * s = (char *) malloc(filter.size() + 10);
	memset(s, 0, filter.size() + 10);

	strcpy(s, filter.c_str());

	for (int i = 0; i < filter.size(); i++)
	{
		if (s[i] == '|')
		{
			s[i] = '\0';
		}
	}

	//Windows 2000新型的打开对话框
	OPENFILENAME fopt; 
	memset(&fopt, 0, sizeof(fopt));
	fopt.lStructSize = sizeof(fopt);

	//fopt.lpstrFilter = "*.exe|*.exe;All Files|*.*";

	fopt.lpstrFilter = s;

	//必须先分配收到的字符串的缓冲区
	char * fn = (char *)malloc(1024);
	memset(fn, 0, 1024);
	
	fopt.lpstrFile = fn;
	fopt.nMaxFile = 1024 - 1;//lpstrFile 不为 NULL 的情况下指定这个值的大小

	int nResult = ::GetOpenFileName(&fopt);

	if (nResult == 0)
	{
		free(s);
		free(fn);
		return "";
	}


	std::string r = fopt.lpstrFile;
	
	free(s);
	free(fn);

	return r;
}

//得到一个路径
std::string VFileDialog::GetPath(bool newDir)
{
	//win2000 后可以用这样的
	#define   BIF_NEWDIALOGSTYLE           0x0040   
	#define   BIF_USENEWUI                       (BIF_NEWDIALOGSTYLE   |   BIF_EDITBOX)  

	BROWSEINFO bi;
	char disname[MAX_PATH],path[MAX_PATH];
	ITEMIDLIST *pidl;
	bi.hwndOwner=0;
	bi.pidlRoot=0;
	bi.pszDisplayName=disname;
	bi.lpszTitle="请选择路径:";
	//bi.ulFlags=BIF_RETURNONLYFSDIRS;
	bi.lpfn=0;
	bi.lParam=1;
	bi.iImage=1;

	if (newDir == true)
	{
		//bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_STATUSTEXT;//使之可新建目录
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE ;//使之可新建目录
	}
	else
	{
		//这种情况下有路径搜索,也比较好用,在没有新建时尽量用这种
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX;
	}

	CString m_Encryptfile = "";
	if(pidl = ::SHBrowseForFolder(&bi))
	{
		::SHGetPathFromIDList(pidl, path);
		//UpdateData();
		m_Encryptfile=CString(path);
		//if(m_Encryptfile.Right(1)!="\\")
		//m_Encryptfile+="\\";
		//csFileName=m_Encryptfile;
		//UpdateData(false);
	}

	return m_Encryptfile;
}

//CImageList
//增加一个
void VImageList::AddImage(CImageList * ctrl, UINT idImage)
{
	ctrl->Add(AfxGetApp()->LoadIcon(idImage));//增加选中状态图像

}

void VImageList::Create(CImageList * ctrl, int countImage)
{
	ctrl->Create(16, 16, ILC_COLOR|ILC_COLOR32, countImage, countImage);//建立图像控制
}


//CTreeCtrl
//失去焦点时是否显示当前选择项
void VTreeCtrl::SetStyleHideSel(CTreeCtrl * ctrl, bool hide)
{
	long style;

	style = ::GetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE);

	if (hide == true)
	{//不显示
		style = style & (~TVS_SHOWSELALWAYS);
	}
	else
	{//显示
		style = style | TVS_SHOWSELALWAYS;
	}

	//if not UseStyle then Style := Style and not Value
	//else Style := Style or Value;
	SetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE, style);
}

//删除所有子节点
void VTreeCtrl::DeleteChildren(CTreeCtrl * ctrl, HTREEITEM item)
{
	TreeView_Expand(ctrl->GetSafeHwnd(), item, TVE_COLLAPSE | TVE_COLLAPSERESET);
}

//用线显示层次关系
void VTreeCtrl::SetStyleShowLines(CTreeCtrl * ctrl)
{
	long style;

	style = ::GetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE);

	{//显示
		style = style | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	}

	//if not UseStyle then Style := Style and not Value
	//else Style := Style or Value;
	SetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE, style);
}


//取父节点//如果 return false 则是没有父节点
bool VTreeCtrl::GetParentItem(CTreeCtrl * ctrl, HTREEITEM item, VTreeNode * node)
{
	bool r = false;

	HTREEITEM pitem = TreeView_GetParent(ctrl->GetSafeHwnd(), item);

	if (pitem != NULL)
	{
		//TreeView_GetItem(ctrl->GetSafeHwnd(), pitem);
		//TVITEM tvi;   
		//char szText[250];
		memset(&node->tvi, 0, sizeof(TVITEM));
		node->tvi.mask = TVIF_TEXT | TVIF_PARAM;   
		node->tvi.hItem = pitem;
		node->tvi.pszText = &node->szText[0];
		node->tvi.cchTextMax = sizeof(node->szText) - 1;
		BOOL bSuccess = TreeView_GetItem(ctrl->GetSafeHwnd(), &node->tvi);

		r = true;

	}

	return r;
}



