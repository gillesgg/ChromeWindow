// View.cpp : implementation of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "osutility.h"

//#include "resource.h"



#include "View.h"



LRESULT CView::OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CPaintDC dc(m_hWnd);
	return 0;
}


BOOL CView::PreTranslateMessage(MSG* pMsg)
{
	pMsg;
	return FALSE;
}