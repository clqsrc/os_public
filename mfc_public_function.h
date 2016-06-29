#ifndef _MFC_PUBLIC_FUNCTION_H_
#define _MFC_PUBLIC_FUNCTION_H_

//mfc �Ĺ��ú���//mfc �Ƚφ���,�и����ú����ȽϺ�

//ȡ��VC��ʹ��STL�ľ���
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

//ȡ��VC��ʹ��STL�ľ���
//#pragma warning(disable:4503)
//#pragma warning(disable:4786)

//��Ӧ CTabCtrl �����������
typedef std::map<int, CDialog *> VTabCtrlPages;//��ֵ��

class VTabCtrl
{
public:
	//����һ��ҳ��
	static void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption);

	//����һ��ҳ��//�����Ի����OnInitDialog()������
	static void VTabCtrl::AddPage(CTabCtrl * ctrl, std::string caption, CDialog * dlg, UINT dlgID);

	//����һ��ҳ��
	//�����Ի���Ϊ��ǩ�ؼ����һ����ǩѡ��ı䣨TCN_SELCHANGE���Ŀؼ�֪ͨ��Ϣ���Ա����û�ѡ���ǩʱ֪ͨ���Ի��������Ի���ı༭�����һ���ǩ�ؼ���ѡ�����һ���¼�����������������     
	//  ���¼�������������´��룬��������     
	//  void   CtabdialogDlg::OnTcnSelchangeTab1(NMHDR   *pNMHDR,   LRESULT   *pResult)  

	static void VTabCtrl::OnSelChange(NMHDR * pNMHDR, LRESULT * pResult, CTabCtrl * ctrl, VTabCtrlPages & pages);

	//��ʾĳһҳ
	static void VTabCtrl::ShowPage(CTabCtrl * ctrl, VTabCtrlPages & pages, int sel);

	//���
	static void VTabCtrl::Clear(CTabCtrl * ctrl);
	
};


//�ļ��Ի���//http://dev.csdn.net/article/13/13461.shtm
class VFileDialog
{
public:		
	static std::string VFileDialog::GetOpenFileName(std::string filter);
	//�õ�һ��·��
	static std::string VFileDialog::GetPath(bool newDir);

};

//CImageList
class VImageList
{
public:
	//����һ��
	static void VImageList::AddImage(CImageList * ctrl, UINT idImage);
	static void VImageList::Create(CImageList * ctrl, int countImage);
};

//CTreeCtrl
//TV_ITEM.pszText ��ҪԤ�ȷ����ڴ��,����Ū���򵥵ı�����
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
	//ʧȥ����ʱ�Ƿ���ʾ��ǰѡ����
	static void VTreeCtrl::SetStyleHideSel(CTreeCtrl * ctrl, bool hide);
	//ɾ�������ӽڵ�
	static void VTreeCtrl::DeleteChildren(CTreeCtrl * ctrl, HTREEITEM item);
	//������ʾ��ι�ϵ
	static void VTreeCtrl::SetStyleShowLines(CTreeCtrl * ctrl);
	//ȡ���ڵ�//��� return false ����û�и��ڵ�
	static bool VTreeCtrl::GetParentItem(CTreeCtrl * ctrl, HTREEITEM item, VTreeNode * node);

};

#endif




