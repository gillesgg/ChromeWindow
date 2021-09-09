// ChromeClient.cpp : main source file for ChromeClient.exe
//
#include "stdafx.h"
#include "resource.h"
#include "App.h"

CAppModule _Module;

int Run(LPTSTR /*lpstrCmdLine*/ = NULL, int nCmdShow = SW_SHOWDEFAULT)
{
	CMessageLoop theLoop;
	_Module.AddMessageLoop(&theLoop);

	ChromeClient::App^ app = gcnew ChromeClient::App();
	app->Run();

	_Module.RemoveMessageLoop();
	return 0;
}

[System::STAThreadAttribute]
int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int nCmdShow)
{
	HRESULT hRes = S_OK;

	AtlInitCommonControls(ICC_BAR_CLASSES);	

	hRes = _Module.Init(NULL, hInstance);
	ATLASSERT(SUCCEEDED(hRes));

	int nRet = Run(lpstrCmdLine, nCmdShow);

	_Module.Term();
	::CoUninitialize();

	return nRet;
}
