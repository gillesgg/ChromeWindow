// WindowHtmlView.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "View.h"
#include "WindowHtmlView.h"


//// This is the constructor of a class that has been exported.
CWindowHtmlView::CWindowHtmlView()
{
    pView = nullptr;
}

/// <summary>
/// Load the view 
/// </summary>
/// <param name="url"></param>
/// <param name="hwndParent"></param>
/// <param name="hwndview"></param>
/// <returns></returns>
HRESULT CWindowHtmlView::CreateView(std::wstring *url, HWND hwndParent, HWND* hwndview)
{
    pView = new CView(*url);
    if (pView)
    {
        *hwndview = pView->Create(hwndParent);
    }
    return S_OK;
}

void CWindowHtmlView::DeleteView()
{
    if (pView)
    {
        pView->DestroyWindow();
        delete (pView);
    }
}