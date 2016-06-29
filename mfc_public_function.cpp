
//ȡ��VC��ʹ��STL�ľ���
#pragma warning(disable:4503)
#pragma warning(disable:4786)

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <math.h>
#include "mfc_public_function.h"

//ȡ��VC��ʹ��STL�ľ���
//#pragma warning(disable:4503)
//#pragma warning(disable:4786)

//CTabCtrl
//����һ��ҳ��//�����Ի����OnInitDialog()������
void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption)
{
	TCITEM   item;
	item.mask   =   TCIF_TEXT;
	item.pszText   =  (char *) caption.c_str();//"��һҳ";

	//ctrl->InsertItem(1, &item);
	ctrl->InsertItem(20, &item);//�о���һ��������ʾ���뵽�ĸ��ط�,�������ܲ��뵽����ȥ
}

//����һ��ҳ��//�����Ի����OnInitDialog()������
void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption, CDialog * dlg, UINT dlgID)
{
	VTabCtrl::AddPage(ctrl, caption);

	//ʵ��������һ����һ�����彨��Ϊ��һ��������Ӵ���
	//�Ի�����Դ�����Ա�����  
	//Border: none   //�߽�Ϊ��,��������û�˱�����   
	//Style: Child   //�������ģ��Ϳ��Ե�����һ�����ڵ��Ӵ�����
	//dlg->Create(dlgID, ctrl->GetParent());
	dlg->Create(dlgID, ctrl);//����Ҳ�ǿ��Ե�,���ǲ�֪���Ƿ�Ӱ����Ϣѭ��
}

//���
void VTabCtrl::Clear(CTabCtrl * ctrl)
{
	ctrl->DeleteAllItems();
}



//����һ��ҳ��
//�����Ի���Ϊ��ǩ�ؼ����һ����ǩѡ��ı䣨TCN_SELCHANGE���Ŀؼ�֪ͨ��Ϣ���Ա����û�ѡ���ǩʱ֪ͨ���Ի��������Ի���ı༭�����һ���ǩ�ؼ���ѡ�����һ���¼�����������������     
//  ���¼�������������´��룬��������     
//  void   CtabdialogDlg::OnTcnSelchangeTab1(NMHDR   *pNMHDR,   LRESULT   *pResult)  

void VTabCtrl::OnSelChange(NMHDR * pNMHDR, LRESULT * pResult, CTabCtrl * ctrl, VTabCtrlPages & pages)
{   

	int sel = ctrl->GetCurSel();

	ShowPage(ctrl, pages, sel);

	*pResult = 0;   
}

//��ʾĳһҳ
void VTabCtrl::ShowPage(CTabCtrl * ctrl, VTabCtrlPages & pages, int sel)
{   
	CRect r;   
	ctrl->GetClientRect(&r);   
	CDialog * dlg = NULL;

	//��ʾ
	//dlg->SetWindowPos(NULL, 10, 30, r.right-20, r.bottom-40, SWP_SHOWWINDOW);   

	//����������
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

	//��ʾ
	dlg = pages[sel];
	if (dlg != NULL)
	{
		dlg->SetWindowPos(NULL, 10, 30, r.right-20, r.bottom-40, SWP_SHOWWINDOW);
		//dlg->ShowWindow(SW_SHOW);//���� wm_show �¼�//����
		::PostMessage(dlg->m_hWnd, WM_SHOWWINDOW, 0, 0);//ok
	}

}



//�ļ��Ի���//http://dev.csdn.net/article/13/13461.shtm
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
	ָ��һ���Կ��ַ������Ĺ����ַ�����һ�����塣�����е����һ���ַ�������������NULL�ַ������� 

	��һ���ַ����ǹ�������������ʾ�ַ��������磬���ı��ļ��������ڶ����ַ�ָ��������ʽ�����磬��*.TXT������ҪΪһ����ʾ�ַ���ָ�����������ʽ��ʹ�÷ֺţ���;�����ָ���ʽ�����磬��*.TXT;*.DOC;*.BAK������һ����ʽ�ַ����п��԰�����Ч���ļ������ַ����Ǻţ�*��ͨ�������������ʽ�ַ����а����ո� 

	ϵͳ���ܸı�������Ĵ�������lpstrFilterָ���Ĵ�����ʾ���ļ�������Ͽ��С� 

	���lpstrFilter��NULL���Ի�������ʾ�κι�������
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

	//Windows 2000���͵Ĵ򿪶Ի���
	OPENFILENAME fopt; 
	memset(&fopt, 0, sizeof(fopt));
	fopt.lStructSize = sizeof(fopt);

	//fopt.lpstrFilter = "*.exe|*.exe;All Files|*.*";

	fopt.lpstrFilter = s;

	//�����ȷ����յ����ַ����Ļ�����
	char * fn = (char *)malloc(1024);
	memset(fn, 0, 1024);
	
	fopt.lpstrFile = fn;
	fopt.nMaxFile = 1024 - 1;//lpstrFile ��Ϊ NULL �������ָ�����ֵ�Ĵ�С

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

//�õ�һ��·��
std::string VFileDialog::GetPath(bool newDir)
{
	//win2000 �������������
	#define   BIF_NEWDIALOGSTYLE           0x0040   
	#define   BIF_USENEWUI                       (BIF_NEWDIALOGSTYLE   |   BIF_EDITBOX)  

	BROWSEINFO bi;
	char disname[MAX_PATH],path[MAX_PATH];
	ITEMIDLIST *pidl;
	bi.hwndOwner=0;
	bi.pidlRoot=0;
	bi.pszDisplayName=disname;
	bi.lpszTitle="��ѡ��·��:";
	//bi.ulFlags=BIF_RETURNONLYFSDIRS;
	bi.lpfn=0;
	bi.lParam=1;
	bi.iImage=1;

	if (newDir == true)
	{
		//bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_STATUSTEXT;//ʹ֮���½�Ŀ¼
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_NEWDIALOGSTYLE ;//ʹ֮���½�Ŀ¼
	}
	else
	{
		//�����������·������,Ҳ�ȽϺ���,��û���½�ʱ����������
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
//����һ��
void VImageList::AddImage(CImageList * ctrl, UINT idImage)
{
	ctrl->Add(AfxGetApp()->LoadIcon(idImage));//����ѡ��״̬ͼ��

}

void VImageList::Create(CImageList * ctrl, int countImage)
{
	ctrl->Create(16, 16, ILC_COLOR|ILC_COLOR32, countImage, countImage);//����ͼ�����
}


//CTreeCtrl
//ʧȥ����ʱ�Ƿ���ʾ��ǰѡ����
void VTreeCtrl::SetStyleHideSel(CTreeCtrl * ctrl, bool hide)
{
	long style;

	style = ::GetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE);

	if (hide == true)
	{//����ʾ
		style = style & (~TVS_SHOWSELALWAYS);
	}
	else
	{//��ʾ
		style = style | TVS_SHOWSELALWAYS;
	}

	//if not UseStyle then Style := Style and not Value
	//else Style := Style or Value;
	SetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE, style);
}

//ɾ�������ӽڵ�
void VTreeCtrl::DeleteChildren(CTreeCtrl * ctrl, HTREEITEM item)
{
	TreeView_Expand(ctrl->GetSafeHwnd(), item, TVE_COLLAPSE | TVE_COLLAPSERESET);
}

//������ʾ��ι�ϵ
void VTreeCtrl::SetStyleShowLines(CTreeCtrl * ctrl)
{
	long style;

	style = ::GetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE);

	{//��ʾ
		style = style | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;
	}

	//if not UseStyle then Style := Style and not Value
	//else Style := Style or Value;
	SetWindowLong(ctrl->GetSafeHwnd(), GWL_STYLE, style);
}


//ȡ���ڵ�//��� return false ����û�и��ڵ�
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



