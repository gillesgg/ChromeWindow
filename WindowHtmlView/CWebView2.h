#include "osutility.h"

class WebView2Impl
{
public:
	wil::com_ptr<ICoreWebView2Environment> webViewEnvironment_ = nullptr;
	wil::com_ptr<ICoreWebView2>            webView_ = nullptr;
	wil::com_ptr<ICoreWebView2Controller>  webController_ = nullptr;
	wil::com_ptr<ICoreWebView2Settings>    webSettings_ = nullptr;
};

enum class CallbackType
{
	CreationCompleted,
	NavigationCompleted,
	TitleChanged,
	AuthenticationCompleted,
};


template<typename T>
class CWebView2 
{
	BEGIN_MSG_MAP(CWebView2)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MESSAGE_HANDLER(MSG_RUN_ASYNC_CALLBACK, OnCallBack)
	END_MSG_MAP()

public:
	CWebView2(std::wstring url, std::wstring webView2Version, std::wstring webView2Channel, std::wstring_view webViewFolder)
		: url_(url)
		, webView2Version_(webView2Version)
		, webView2Channel_(webView2Channel)
		, webViewFolder_(webViewFolder)
	{
	}

private:
	using CallbackFunc = std::function<void(void)>;


protected:

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		InitEnv();
		auto hr = InitWebView();


		RegisterCallback(CallbackType::CreationCompleted, [this]()
			{
				ATLTRACE("message=%s\n", "CallbackType::CreationCompleted");
			});

		RegisterCallback(CallbackType::NavigationCompleted, [this]()
			{
				ATLTRACE("message=%s\n", "CallbackType::NavigationCompleted");
			});

		RegisterCallback(CallbackType::AuthenticationCompleted, [this]()
			{
				ATLTRACE("message=%s\n", "CallbackType::AuthenticationCompleted");
			});


		return (bHandled = FALSE);

	}


	LRESULT	OnCallBack(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		auto* task = reinterpret_cast<CallbackFunc*>(wParam);
		(*task)();
		delete task;

		return bHandled = TRUE;
	}

	void RegisterCallback(CallbackType const type, CallbackFunc callback)
	{
		m_callbacks[type] = callback;
	}


	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		ResizeToClientArea();
		return FALSE;
	}
private:

	using CallbackFunc = std::function<void(void)>;

	bool									isnavigating_ = false;
	std::wstring							url_;
	std::wstring							browserDirectory_;
	std::wstring							userDataDirectory_;

	std::unique_ptr<WebView2Impl>			webview2imp_;
	std::map<CallbackType, CallbackFunc>	m_callbacks;

	EventRegistrationToken					m_navigationStartingToken = {};
	EventRegistrationToken					m_navigationCompletedToken = {};
	EventRegistrationToken					m_documentTitleChangedToken = {};
	EventRegistrationToken					webresourcerequestedToken_ = {};

	std::wstring_view						webView2Version_;
	std::wstring_view						webView2Channel_;
	std::wstring_view						webViewFolder_;


private:

private:
	HRESULT InitEnv()
	{

		// Verify that the WebView2 runtime is installed.
		PWSTR edgeVersionInfo = nullptr;
		HRESULT hr = ::GetAvailableCoreWebView2BrowserVersionString(nullptr, &edgeVersionInfo);
		if (FAILED(hr) || (edgeVersionInfo == nullptr))
		{
			ATLTRACE("The WebView2 runtime is not installed\n");
			::MessageBoxW(nullptr, L"Please install the WebView2 runtime before running this application,"
				L" available on https://go.microsoft.com/fwlink/p/?LinkId=2124703", L"Missing WebView2 runtime",
				MB_OK | MB_ICONERROR);
			return -1;
		}
		ATLTRACE("Found installed WebView version %ls\n", edgeVersionInfo);

		if (webView2Version_.empty())
		{   // User did not provided specific WebView2 versions and channels.
			// Set WebView2 version and channel to default values. 
			std::wstring_view edgeVersionInfoStr = edgeVersionInfo;
			size_t pos = edgeVersionInfoStr.find(L' ');

			if ((edgeVersionInfoStr.size() > 0) && (pos < edgeVersionInfoStr.size() - 1))
			{   // Assume Edge version with format 'x.y.z.t channel"
				webView2Version_ = edgeVersionInfoStr.substr(0, pos);
				edgeVersionInfo[pos] = L'\0'; // Ensure webView2Version is null-terminated.
				webView2Channel_ = edgeVersionInfoStr.substr(pos + 1, edgeVersionInfoStr.size() - pos - 1);
			}
			else
			{   // Assume Edge version with format 'x.y.z.t"
				webView2Version_ = edgeVersionInfoStr;
			}

			ATLTRACE("Using WebView2 version=%ls\n", webView2Version_.data());
			ATLTRACE("Using WebView2 channel=%ls\n", webView2Channel_.data());
		}

		webview2imp_ = std::make_unique<WebView2Impl>();
		m_callbacks[CallbackType::CreationCompleted] = nullptr;
		m_callbacks[CallbackType::NavigationCompleted] = nullptr;
		m_callbacks[CallbackType::AuthenticationCompleted] = nullptr;
		
		if (url_.empty())
			url_ = L"http://msdn.microsoft.com";


		browserDirectory_ = osutility::GetBrowserDirectory(webView2Version_, webView2Channel_, webViewFolder_);
		userDataDirectory_ = osutility::GetUserDataDirectory(webView2Channel_);


		return (hr);
	}


	void RunAsync(CallbackFunc callback)
	{
		auto* task = new CallbackFunc(callback);
		static_cast<T*>(this)->PostMessage(MSG_RUN_ASYNC_CALLBACK, reinterpret_cast<WPARAM>(task), 0);
	}


	HRESULT InitWebView()
	{
		ATLTRACE("function=%s\n", __func__);
		ATLTRACE("Using user data directory %ls\n", userDataDirectory_.data());

		auto options = Microsoft::WRL::Make<CoreWebView2EnvironmentOptions>();
		HRESULT hr = options->put_AllowSingleSignOnUsingOSPrimaryAccount(TRUE);
		if FAILED(hr)
		{
			ATLTRACE("Failed to enable SSO");
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		}

		std::wstring langid(osutility::GetUserMUI());
		hr = options->put_Language(langid.c_str());
		if FAILED(hr)
		{
			ATLTRACE("Failed to set language to en-us");
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		}

		hr = CreateCoreWebView2EnvironmentWithOptions(browserDirectory_.empty() ? nullptr : browserDirectory_.data(),
			userDataDirectory_.data(), options.Get(),
			Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(this, &CWebView2::OnCreateEnvironmentCompleted).Get());

		if FAILED(hr)
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		}
		if (HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) == hr)
		{
			static_cast<T*>(this)->MessageBox(L"Unable to find the WebView2 engine", L"WebView_Sample", MB_OK | MB_ICONERROR);
		}
		return (hr);
	}


	HRESULT OnCreateEnvironmentCompleted(HRESULT result, ICoreWebView2Environment* environment)
	{
		ATLTRACE("function=%s\n", __func__);

		if (result == S_OK)
		{
			if (!webview2imp_)
			{
				ATLTRACE("function=%s message=webview is null\n", __func__);
				result = E_INVALIDARG;
				return result;
			}

			result = environment->QueryInterface(IID_PPV_ARGS(&webview2imp_->webViewEnvironment_));
			if FAILED(result)
			{
				ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(result).data(), result);
				return (result);
			}
			result = webview2imp_->webViewEnvironment_->CreateCoreWebView2Controller(static_cast<T*>(this)->m_hWnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(this, &CWebView2::OnCreateWebViewControllerCompleted).Get());
			if FAILED(result)
			{
				ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(result).data(), result);
			}
		}
		else
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(result).data(), result);
		}
		return (result);

	}


	HRESULT OnCreateWebViewControllerCompleted(HRESULT result, ICoreWebView2Controller* controller)
	{
		ATLTRACE("function=%s\n", __func__);

		HRESULT hr = S_OK;

		if (result != S_OK || controller == nullptr)
		{
			ATLTRACE("function=%s, message=cannot create webview environment\n", __func__);
			return E_INVALIDARG;
		}

		webview2imp_->webController_ = controller;
		if FAILED(hr = controller->get_CoreWebView2(&webview2imp_->webView_))
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
			return hr;
		}
		if FAILED(hr = webview2imp_->webView_->get_Settings(&webview2imp_->webSettings_))
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
			return hr;
		}
		if FAILED(hr = RegisterEventHandlers())
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
			return hr;
		}
		if FAILED(hr = ResizeToClientArea())
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
			return hr;
		}
		auto& callback = m_callbacks[CallbackType::CreationCompleted];
		if (callback == nullptr)
		{
			ATLTRACE("function=%s, message=unable to create callback", __func__);
			return E_FAIL;
		}
		RunAsync(callback);
		return hr;
	}


	HRESULT RegisterEventHandlers()
	{
		ATLTRACE("function=%s\n", __func__);

		HRESULT hr;

		// NavigationCompleted handler
		hr = webview2imp_->webView_->add_NavigationCompleted(Microsoft::WRL::Callback<ICoreWebView2NavigationCompletedEventHandler>(
			[this](ICoreWebView2*, ICoreWebView2NavigationCompletedEventArgs* args) -> HRESULT
			{
				isnavigating_ = false;
				HRESULT hr;
				BOOL success;
				hr = args->get_IsSuccess(&success);

				if (!success)
				{
					COREWEBVIEW2_WEB_ERROR_STATUS webErrorStatus{};
					hr = args->get_WebErrorStatus(&webErrorStatus);
					if (webErrorStatus == COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED)
					{
						ATLTRACE("function=%s message=COREWEBVIEW2_WEB_ERROR_STATUS_DISCONNECTED\n", __func__);
					}
				}

				wil::unique_cotaskmem_string uri;
				webview2imp_->webView_->get_Source(&uri);

				if (wcscmp(uri.get(), L"about:blank") == 0)
				{
					uri = wil::make_cotaskmem_string(L"");
				}

				auto& callback = m_callbacks[CallbackType::NavigationCompleted];
				if (callback != nullptr)
					RunAsync(callback);

				return S_OK;
			})
			.Get(), &m_navigationCompletedToken);
		if FAILED(hr)
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		}
		// NavigationStarting handler
		hr = webview2imp_->webView_->add_NavigationStarting(
			Microsoft::WRL::Callback<ICoreWebView2NavigationStartingEventHandler>(
				[this](ICoreWebView2*, ICoreWebView2NavigationStartingEventArgs* args) -> HRESULT
				{
					wil::unique_cotaskmem_string uri;
					args->get_Uri(&uri);

					isnavigating_ = true;

					return S_OK;
				}).Get(), &m_navigationStartingToken);


		hr = webview2imp_->webView_->AddWebResourceRequestedFilter(L"*", COREWEBVIEW2_WEB_RESOURCE_CONTEXT_ALL);
		if FAILED(hr)
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		}
		hr = webview2imp_->webView_->add_WebResourceRequested(
			Microsoft::WRL::Callback<ICoreWebView2WebResourceRequestedEventHandler>(
				[this](
					ICoreWebView2*,
					ICoreWebView2WebResourceRequestedEventArgs* args) -> HRESULT
				{
					wil::com_ptr <ICoreWebView2WebResourceRequest> request = nullptr;
					wil::com_ptr <ICoreWebView2HttpRequestHeaders> headers = nullptr;

					auto hr = args->get_Request(&request);
					if FAILED(hr)
					{
						ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
						return (hr);
					}
					hr = request->get_Headers(&headers);
					if FAILED(hr)
					{
						ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
						return (hr);
					}
					BOOL hasheader = FALSE;
					hr = headers->Contains(L"Authorization", &hasheader);
					if (hasheader == TRUE && hr == S_OK)
					{
						LPTSTR authV = new TCHAR[1000];
						hr = headers->GetHeader(L"Authorization", &authV);
						if (hr == S_OK)
						{
							ATLTRACE(L"name=%s value=%s\n", L"Authorization", authV);
							auto& callback = m_callbacks[CallbackType::AuthenticationCompleted];
							if (callback == nullptr)
							{
								ATLTRACE("function=%s message=unable to create callback\n", __func__);
								return E_FAIL;
							}
							RunAsync(callback);
							return S_OK;
						}
					}
					return hr;

				}).Get(), &webresourcerequestedToken_);


		if FAILED(hr)
		{
			ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
		}
		if (!url_.empty())
			hr = webview2imp_->webView_->Navigate(url_.c_str());
		else
			hr = webview2imp_->webView_->Navigate(L"about:blank");
		return (hr);
	}


	HRESULT ResizeToClientArea()
	{
		HRESULT hr = S_OK;
		if (webview2imp_ && webview2imp_->webController_)
		{
			RECT bounds;
			static_cast<T*>(this)->GetClientRect(&bounds);
			hr = webview2imp_->webController_->put_Bounds(bounds);
			if (FAILED(hr))
			{
				ATLTRACE("function=%s, message=%s, hr=%d\n", __func__, std::system_category().message(hr).data(), hr);
			}
		}
		return (hr);
	}
};