#pragma once

#include "../WindowHtmlView/WindowHtmlView.h"

#include "Resource.h"

namespace ChromeClient
{
	using namespace System;
	using namespace System::Windows;
	using namespace System::Runtime::InteropServices;

	ref class FormViewHwndHost : System::Windows::Interop::HwndHost
	{
	public:
		FormViewHwndHost();
	private :
		CWindowHtmlView* pView_;

	protected:
		virtual HandleRef BuildWindowCore(System::Runtime::InteropServices::HandleRef hwndParent) override;
		virtual void DestroyWindowCore(System::Runtime::InteropServices::HandleRef hwnd) override;
	};

};
