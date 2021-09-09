#include "stdafx.h"
#include "FormViewHwndHost.h"


namespace ChromeClient
{
	using namespace System::Windows;

	FormViewHwndHost::FormViewHwndHost()
	{
		pView_ = nullptr;
	}
	System::Runtime::InteropServices::HandleRef FormViewHwndHost::BuildWindowCore(System::Runtime::InteropServices::HandleRef hwndParent)
	{
		HWND			hwndview	= nullptr;
		std::wstring	url			= L"https://www.microsoft.com";
		
		pView_ = new CWindowHtmlView();
		if (pView_)
		{
			pView_->CreateView(&url, (HWND)hwndParent.Handle.ToInt64(), &hwndview);
		}
		return System::Runtime::InteropServices::HandleRef(this, IntPtr(hwndview));		
	}

	void FormViewHwndHost::DestroyWindowCore(System::Runtime::InteropServices::HandleRef hwnd)
	{	
		if (pView_)
		{
			pView_->DeleteView();
			delete (pView_);
			pView_ = nullptr;
		}
	}
};