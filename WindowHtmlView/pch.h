// pch.h: This is a precompiled header file.
// Files listed below are compiled only once, improving build performance for future builds.
// This also affects IntelliSense performance, including code completion and many code browsing features.
// However, files listed here are ALL re-compiled if any one of them is updated between builds.
// Do not add files here that you will be updating frequently as this negates the performance advantage.

#ifndef PCH_H
#define PCH_H

// error C2039: 'wait_for': is not a member of 'winrt::impl'
#include <winrt/base.h>
namespace winrt::impl
{
	template <typename Async>
	auto wait_for(Async const& async, Windows::Foundation::TimeSpan const& timeout);
}


// Atl header files
#include <atlbase.h>
#include <atlapp.h>

extern CAppModule _Module;

// C RunTime Header Files
#include <cstdlib>
#include <cstdio>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <string>
#include <string_view>
#include <filesystem>
#include <map>
#include <functional>

#include <atlwin.h>

#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>

// Com wrapper
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result.h>
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <wrl.h>


// WebView
#include <WebView2.h>
#include <WebView2EnvironmentOptions.h>

namespace		fs = std::filesystem;

#include "framework.h"

static constexpr UINT MSG_RUN_ASYNC_CALLBACK = WM_APP + 124;

#endif //PCH_H
