// View.h : interface of the CView class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "CWebView2.h"



class CView :	public CWindowImpl<CView>,
				public CWebView2<CView>
{
public:

	CView() : CWebView2(L"",L"", L"", L"")
	{

	};

	CView(std::wstring url) : CWebView2(url, L"", L"", L"")
	{

	};


	CView(std::wstring url, std::wstring webView2Version, std::wstring webView2Channel, std::wstring_view webViewFolder) : CWebView2(url, webView2Version, webView2Channel, webViewFolder)
	{

	};

public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg);

	BEGIN_MSG_MAP(CView)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		CHAIN_MSG_MAP(CWebView2<CView>)
	END_MSG_MAP()

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	
};
