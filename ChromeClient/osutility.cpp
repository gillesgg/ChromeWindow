#include "stdafx.h"
#include "resource.h"
#include "osutility.h"

namespace ChromeClient
{

	std::wstring osutility::themefolder()
	{
		return 	L"Themes";
	}

	std::wstring osutility::xamlfolder()
	{
		return 	L"xaml";
	}
	std::wstring osutility::xamlmainwindowfile()
	{
		return 	L"MainWindow.xaml";
	}
	std::wstring osutility::themefile()
	{
		return 	L"WindowsTheme.xaml";
	}

	HRESULT osutility::LoadResourcestofile()
	{
		HRESULT hr;

		if (FAILED(hr = LoadResourcetofile(themefolder(), themefile(), IDR_THEME_APP, L"THEME")))
		{
			return hr;
		}

		hr = LoadResourcetofile(xamlfolder(), xamlmainwindowfile(), IDR_XAML_MAINWINDOW, L"XAML");

		return hr;
	}

	HRESULT osutility::LoadResourcetofile(std::wstring folder, std::wstring filename, int id, std::wstring name)
	{
		HRSRC findResourceHandle;
		HGLOBAL resourceHandle;


		findResourceHandle = FindResource(GetModuleHandle(nullptr), MAKEINTRESOURCE(id), name.c_str());

		if (findResourceHandle == nullptr)
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}
		unsigned int resourceSize = SizeofResource(NULL, findResourceHandle);
		resourceHandle = LoadResource(NULL, findResourceHandle);
		if (resourceHandle == NULL)
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		unsigned char* registerResource = (unsigned char*)LockResource(resourceHandle);
		unsigned long actuallyWritten;

		std::wstring destfile = GetFolder(folder, filename);

		HANDLE fileHandle = CreateFile(destfile.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (fileHandle == INVALID_HANDLE_VALUE)
		{
			return HRESULT_FROM_WIN32(GetLastError());
		}

		WriteFile(fileHandle, registerResource, resourceSize, &actuallyWritten, NULL);
		CloseHandle(fileHandle);
		return S_OK;
	}

	std::wstring osutility::GetExecutableName(std::wstring path)
	{
		return fs::path(path).stem().wstring();
	}

	std::wstring osutility::GetExecutableName()
	{
		wchar_t			processname[MAX_PATH];
		DWORD			dwSize = MAX_PATH;
		std::wstring	sprocessname;


		auto hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, false, GetCurrentProcessId());
		if (hProcess != nullptr)
		{
			if (QueryFullProcessImageName(hProcess, 0, processname, &dwSize) != 0)
			{

				sprocessname = GetExecutableName(processname);
			}
			CloseHandle(hProcess);
		}
		return sprocessname;
	}

	std::wstring osutility::GetFolder(std::wstring folder, std::wstring file)
	{
		LPWSTR				wszPath = nullptr;
		HRESULT				hr = S_OK;
		fs::path			configfile;

		try
		{
			std::wstring executablename = GetExecutableName();

			if (SUCCEEDED(hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, KF_FLAG_CREATE, nullptr, &wszPath)))
			{
				configfile = fs::path(wszPath).append(executablename);
				configfile = fs::path(configfile).append(folder);
				configfile = fs::path(configfile).append(file);
				if (fs::exists(configfile))
					return (configfile.native());
				else
				{
					auto folderexecutable = fs::path(wszPath).append(executablename);

					if (!fs::exists(folderexecutable))
					{
						fs::create_directory(folderexecutable);

					}
					folderexecutable = fs::path(folderexecutable).append(folder);
					if (!fs::exists(folderexecutable))
					{
						fs::create_directory(folderexecutable);
					}
					auto finalfile = fs::path(folderexecutable).append(file);
					return finalfile.native();
				}
			}
			else
			{
				ATLTRACE(L"Unable to get the folder, GetLastError=%d\n", HRESULT_FROM_WIN32(GetLastError()));
				return std::wstring();
			}
			
			return std::wstring();

		}
		catch (std::filesystem::filesystem_error &e)
		{
			ATLTRACE(L"Unable to create folders error=%s\n", e.what());
			return std::wstring();
		}

	}
};