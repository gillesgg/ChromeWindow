// DumpColors.cpp : Defines the entry point for the application.
//

#include "pch.h"
#include "framework.h"
#include "DumpColors.h"

HINSTANCE hInst;
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
int nWidth = 600, nHeight = 400;
#define IDC_LISTVIEW 10
DWORD nTicksOld = 0;

void FillListView(HWND hWndLV);

typedef int (WINAPI* PGetImmersiveColorSetCount)();
typedef DWORD(WINAPI* PGetImmersiveColorFromColorSetEx)(UINT dwImmersiveColorSet, UINT dwImmersiveColorType, bool bIgnoreHighContrast, UINT dwHighContrastCacheMode);
typedef WCHAR** (WINAPI* PGetImmersiveColorNamedTypeByIndex)(UINT dwImmersiveColorType);
typedef int (WINAPI* PGetImmersiveColorTypeFromName)(const WCHAR* name);
typedef int (WINAPI* PGetImmersiveUserColorSetPreference)(bool bForceCheckRegistry, bool bSkipCheckOnFail);

PGetImmersiveColorSetCount pGetImmersiveColorSetCount = NULL;
PGetImmersiveColorFromColorSetEx pGetImmersiveColorFromColorSetEx = NULL;
PGetImmersiveColorNamedTypeByIndex pGetImmersiveColorNamedTypeByIndex = NULL;
PGetImmersiveColorTypeFromName pGetImmersiveColorTypeFromName = NULL;
PGetImmersiveUserColorSetPreference pGetImmersiveUserColorSetPreference = NULL;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{

	HMODULE hUxTheme = LoadLibrary(TEXT("uxtheme.dll"));
	pGetImmersiveColorSetCount = (PGetImmersiveColorSetCount)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(94));
	pGetImmersiveColorFromColorSetEx = (PGetImmersiveColorFromColorSetEx)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(95));
	pGetImmersiveColorNamedTypeByIndex = (PGetImmersiveColorNamedTypeByIndex)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(100));
	pGetImmersiveColorTypeFromName = (PGetImmersiveColorTypeFromName)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(96));
	pGetImmersiveUserColorSetPreference = (PGetImmersiveUserColorSetPreference)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(98));
	if (!pGetImmersiveColorSetCount || !pGetImmersiveColorFromColorSetEx || !pGetImmersiveColorNamedTypeByIndex || !pGetImmersiveColorTypeFromName || !pGetImmersiveUserColorSetPreference)
		return MessageBox(NULL, TEXT("Cannot load Immersive colors APIs !"), TEXT("Error"), MB_ICONERROR | MB_OK);

	hInst = hInstance;
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst, LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), NULL, TEXT("WindowClass"), NULL,
	};
	if (!RegisterClassEx(&wcex))
		return MessageBox(NULL, TEXT("Cannot register class !"), TEXT("Error"), MB_ICONERROR | MB_OK);
	int nX = (GetSystemMetrics(SM_CXSCREEN) - nWidth) / 2, nY = (GetSystemMetrics(SM_CYSCREEN) - nHeight) / 2;
	HWND hWnd = CreateWindowEx(0, wcex.lpszClassName, TEXT("Test Immersive colors"), WS_OVERLAPPEDWINDOW, nX, nY, nWidth, nHeight, NULL, NULL, hInst, NULL);
	if (!hWnd)
		return MessageBox(NULL, TEXT("Cannot create window !"), TEXT("Error"), MB_ICONERROR | MB_OK);
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HWND hWndButton = NULL;
	static HWND hWndLV = NULL;
	int wmId, wmEvent;
	switch (message)
	{
	case WM_CREATE:
	{
		hWndLV = CreateWindowEx(0, WC_LISTVIEW, TEXT(""), WS_VISIBLE | WS_CHILD | WS_BORDER | LVS_REPORT | LVS_SINGLESEL, 10, 10, nWidth - 35, nHeight - 60, hWnd, (HMENU)IDC_LISTVIEW, hInst, NULL);

		DWORD dwExStyle = ListView_GetExtendedListViewStyle(hWndLV);
		dwExStyle |= LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_LABELTIP | LVS_EX_DOUBLEBUFFER;
		ListView_SetExtendedListViewStyle(hWndLV, dwExStyle);

		LVCOLUMN lvCol;
		TCHAR wszBuf[32];
		wsprintf(wszBuf, TEXT("Color"));
		ZeroMemory(&lvCol, sizeof(lvCol));
		lvCol.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
		lvCol.fmt = LVCFMT_LEFT;
		lvCol.cx = 440;
		lvCol.pszText = wszBuf;
		ListView_InsertColumn(hWndLV, 0, &lvCol);
		wsprintf(wszBuf, TEXT("ARGB"));
		lvCol.pszText = wszBuf;
		lvCol.cx = 100;
		ListView_InsertColumn(hWndLV, 1, &lvCol);
		FillListView(hWndLV);
		return 0;
	}
	break;
	case WM_NOTIFY:
	{
		LPNMHDR pnmhdr = (LPNMHDR)lParam;
		LPNMLISTVIEW pnmListView = (LPNMLISTVIEW)lParam;
		switch (pnmhdr->code)
		{
		case NM_CUSTOMDRAW:
		{
			NMLVCUSTOMDRAW* pnmlv = (NMLVCUSTOMDRAW*)pnmhdr;
			int nItem = pnmlv->nmcd.dwItemSpec;
			int nSubItem = pnmlv->iSubItem;
			BOOL bHighlighted = 0;

			if (pnmlv->nmcd.dwDrawStage == CDDS_PREPAINT)
			{
				return CDRF_NOTIFYITEMDRAW;
			}
			else if (pnmlv->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
			{
				bHighlighted = (pnmlv->nmcd.uItemState & (CDIS_SELECTED | CDIS_FOCUS));
				if (bHighlighted)
				{
					pnmlv->clrText = RGB(255, 255, 0);
					pnmlv->clrTextBk = RGB(255, 0, 0);
					ListView_SetItemState(pnmhdr->hwndFrom, nItem, 0, LVIS_SELECTED);
				}
				else
				{
					WCHAR wsText[255] = L"";
					ListView_GetItemText(pnmhdr->hwndFrom, nItem, 1, wsText, ARRAYSIZE(wsText));
					DWORD nColor = _wtof(wsText);
					COLORREF color = RGB(0xFF & nColor, (0xFF00 & nColor) >> 8, (0xFF0000 & nColor) >> 16);
					pnmlv->clrTextBk = color;
					if (color == 0)
						pnmlv->clrText = RGB(255, 255, 255);
				}
				return CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;
			}
			else if (pnmlv->nmcd.dwDrawStage == CDDS_ITEMPOSTPAINT)
			{

			}
		}
		break;
		}
	}
	break;
	case WM_SIZE:
	{
		MoveWindow(hWndLV, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	}
	break;
	case WM_SETTINGCHANGE:
	{
		WCHAR* plParam = (LPWSTR)lParam;
		if (plParam)
		{
			if (lstrcmp(plParam, TEXT("ImmersiveColorSet")) == 0)
			{
				DWORD nTicks = GetTickCount();
				if (nTicks - nTicksOld > 5000 || nTicksOld == 0)
				{
					Beep(6000, 50);
					ListView_DeleteAllItems(hWndLV);
					FillListView(hWndLV);
				}
				nTicksOld = nTicks;
			}
		}
		return 0;
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void FillListView(HWND hWndLV)
{
	TCHAR wsName[255] = TEXT("");
	TCHAR wsNameImmersive[300] = TEXT("");
	TCHAR wszBuf[32];

	if (pGetImmersiveColorSetCount && pGetImmersiveColorFromColorSetEx && pGetImmersiveColorNamedTypeByIndex && pGetImmersiveColorTypeFromName)
	{
		int nItem = 0;
		for (int i = 0; i <= 1250; i++)
		{
			WCHAR** pName = pGetImmersiveColorNamedTypeByIndex(i);
			if (pName)
			{
				lstrcpy(wsName, *pName);

				LVITEM lvItem = { 0 };
				lvItem.mask = LVIF_TEXT;
				lvItem.cchTextMax = 256;
				lvItem.iItem = i;
				lvItem.pszText = wsName;
				ListView_InsertItem(hWndLV, &lvItem);

				lstrcpy(wsNameImmersive, TEXT("Immersive"));
				lstrcat(wsNameImmersive, wsName);
				DWORD rgba = pGetImmersiveColorFromColorSetEx(pGetImmersiveUserColorSetPreference(false, false), pGetImmersiveColorTypeFromName(wsNameImmersive), false, 0);
				lvItem.iSubItem = 1;
				wsprintf(wszBuf, TEXT("%lu"), rgba);
				lvItem.pszText = wszBuf;
				ListView_SetItem(hWndLV, &lvItem);
			}
		}
	}
}



//#define MAX_LOADSTRING 100
//// Global Variables:
//HINSTANCE hInst;                                // current instance
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
//
//// Forward declarations of functions included in this code module:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//                     _In_opt_ HINSTANCE hPrevInstance,
//                     _In_ LPWSTR    lpCmdLine,
//                     _In_ int       nCmdShow)
//{
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//    // TODO: Place code here.
//
//    // Initialize global strings
//    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//    LoadStringW(hInstance, IDC_DUMPCOLORS, szWindowClass, MAX_LOADSTRING);
//    MyRegisterClass(hInstance);
//
//    // Perform application initialization:
//    if (!InitInstance (hInstance, nCmdShow))
//    {
//        return FALSE;
//    }
//
//    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DUMPCOLORS));
//
//    MSG msg;
//
//    // Main message loop:
//    while (GetMessage(&msg, nullptr, 0, 0))
//    {
//        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//        {
//            TranslateMessage(&msg);
//            DispatchMessage(&msg);
//        }
//    }
//
//    return (int) msg.wParam;
//}
//
//
//
////
////  FUNCTION: MyRegisterClass()
////
////  PURPOSE: Registers the window class.
////
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//    WNDCLASSEXW wcex;
//
//    wcex.cbSize = sizeof(WNDCLASSEX);
//
//    wcex.style          = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc    = WndProc;
//    wcex.cbClsExtra     = 0;
//    wcex.cbWndExtra     = 0;
//    wcex.hInstance      = hInstance;
//    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DUMPCOLORS));
//    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
//    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_DUMPCOLORS);
//    wcex.lpszClassName  = szWindowClass;
//    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//    return RegisterClassExW(&wcex);
//}
//
////
////   FUNCTION: InitInstance(HINSTANCE, int)
////
////   PURPOSE: Saves instance handle and creates main window
////
////   COMMENTS:
////
////        In this function, we save the instance handle in a global variable and
////        create and display the main program window.
////
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // Store instance handle in our global variable
//
//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}
//
////
////  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  PURPOSE: Processes messages for the main window.
////
////  WM_COMMAND  - process the application menu
////  WM_PAINT    - Paint the main window
////  WM_DESTROY  - post a quit message and return
////
////
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message)
//    {
//    case WM_COMMAND:
//        {
//            int wmId = LOWORD(wParam);
//            // Parse the menu selections:
//            switch (wmId)
//            {
//            case IDM_ABOUT:
//                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//                break;
//            case IDM_EXIT:
//                DestroyWindow(hWnd);
//                break;
//            default:
//                return DefWindowProc(hWnd, message, wParam, lParam);
//            }
//        }
//        break;
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            // TODO: Add any drawing code that uses hdc here...
//            EndPaint(hWnd, &ps);
//        }
//        break;
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}
//
//// Message handler for about box.
//INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    UNREFERENCED_PARAMETER(lParam);
//    switch (message)
//    {
//    case WM_INITDIALOG:
//        return (INT_PTR)TRUE;
//
//    case WM_COMMAND:
//        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
//        {
//            EndDialog(hDlg, LOWORD(wParam));
//            return (INT_PTR)TRUE;
//        }
//        break;
//    }
//    return (INT_PTR)FALSE;
//}
