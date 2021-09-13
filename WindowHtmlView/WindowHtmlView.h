// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the WINDOWHTMLVIEW_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// WINDOWHTMLVIEW_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef WINDOWHTMLVIEW_EXPORTS
#define WINDOWHTMLVIEW_API __declspec(dllexport)
#else
#define WINDOWHTMLVIEW_API __declspec(dllimport)
#endif



class CView;

class WINDOWHTMLVIEW_API CWindowHtmlView
{
public:
	CWindowHtmlView(void);
	HRESULT CreateView(std::wstring *url, HWND hwndParent, HWND* hwndview);
	void DeleteView();
private:
	CView* pView;
};
