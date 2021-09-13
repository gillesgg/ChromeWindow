#include "stdafx.h"
#include "App.h"
#include "theme.h"
#include "FormViewHwndHost.h"

namespace ChromeClient
{
	using namespace System;
	using namespace System::Windows;
	using namespace System::Xml;
	using namespace System::Xaml;
	using namespace System::IO;
	using namespace System::Windows::Input;
	using namespace System::Windows::Media;
	using namespace Microsoft::Win32;




	App::App()
	{
		windowstheme_ = new ChromeClient::theme();


		auto hr = this->Initialize();
		if (hr != S_OK)
		{
			Environment::Exit(hr);
		}
	}

	void App::ApplyWindowsTheme()
	{
		if (windowstheme_ != nullptr && (Resources->MergedDictionaries->Count > 0))
		{
			auto bkcolor = windowstheme_->getcolor(true,L"ImmersiveApplicationBackground");
			if (bkcolor != 0)
			{
				System::Windows::Media::Color c = System::Windows::Media::Color::FromArgb((byte)((0xFF000000 & bkcolor) >> 24), (byte)(0x000000FF & bkcolor), (byte)((0x0000FF00 & bkcolor) >> 8), (byte)((0x00FF0000 & bkcolor) >> 16));
				System::Windows::Media::SolidColorBrush^ bk = gcnew System::Windows::Media::SolidColorBrush(c);

				if (!c.Equals(((System::Windows::Media::SolidColorBrush^)this->MainWindow->Background)->Color))
				{
					this->MainWindow->Background = bk;
				}			
			}
			bkcolor = windowstheme_->getcolor(true, L"ImmersiveApplicationText");
			if (bkcolor != 0)
			{
				System::Windows::Media::Color c = System::Windows::Media::Color::FromArgb((byte)((0xFF000000 & bkcolor) >> 24), (byte)(0x000000FF & bkcolor), (byte)((0x0000FF00 & bkcolor) >> 8), (byte)((0x00FF0000 & bkcolor) >> 16));
				System::Windows::ResourceDictionary^ dic = Resources->MergedDictionaries[0];
				System::Windows::Media::SolidColorBrush^ bk = gcnew System::Windows::Media::SolidColorBrush(c);

				if (!c.Equals(((System::Windows::Media::SolidColorBrush^)dic["ControlGlythColour"])->Color))
				{
					dic->Remove("ControlGlythColour");
					dic->Add("ControlGlythColour", bk);
				}
			}
		}
	}
	

	void App::OnUserPreferenceChanged(System::Object^ sender, Microsoft::Win32::UserPreferenceChangedEventArgs^ e)
	{
		ApplyWindowsTheme();	
	}


	HRESULT App::LoadResourcestofile()
	{

		std::wstring themefile = L"WindowsTheme.xaml";
		std::wstring themefolder = L"Themes";
		std::wstring xamlfolder = L"XAML";
		std::wstring MainWindowfile = L"MainWindow.xaml";

		HRESULT hr;

		if (FAILED(hr = LoadResourcetofile(themefolder, themefile, IDR_THEME_APP, L"THEME")))
		{
			return hr;
		}

		hr = LoadResourcetofile(xamlfolder, MainWindowfile, IDR_XAML_MAINWINDOW, L"XAML");
		
		return hr;
		

	}

	HRESULT App::LoadResourcetofile(std::wstring folder, std::wstring filename, int id, std::wstring name)
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

	std::wstring App::GetExecutableName(std::wstring path)
	{
		return fs::path(path).stem().wstring();
	}

	std::wstring App::GetExecutableName()
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

	std::wstring App::GetFolder(std::wstring folder, std::wstring file)
	{
		LPWSTR				wszPath = nullptr;
		HRESULT				hr = S_OK;
		fs::path			configfile;

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
		ATLTRACE(L"the file %s do not exist", configfile.generic_wstring().c_str());
		return std::wstring();

	}

	HRESULT App::Initialize()
	{
		HRESULT hr;
		
		SystemEvents::UserPreferenceChanged += gcnew Microsoft::Win32::UserPreferenceChangedEventHandler(this, &ChromeClient::App::OnUserPreferenceChanged);

		if (FAILED(hr = LoadResourcestofile()))
		{
			return hr;
		}
	

		if (FAILED(hr = InitializeTheme()))
			return hr;

		this->MainWindow = WindowFromXaml();

		if (FAILED(hr = ApplyTheme()))
			return hr;

		if (this->MainWindow != nullptr)
		{
			ApplyWindowsTheme();
			this->MainWindow->Left = 0;
			this->MainWindow->Top = 0;
			this->MainWindow->Loaded += gcnew RoutedEventHandler(this, &App::onloaded);
			LoadHTMLView();
			this->MainWindow->Show();
			return S_OK;
		}
		else
		{
			return E_FAIL;
		}
	}
	HRESULT App::LoadHTMLView()
	{
		FormViewHwndHost^ host = gcnew FormViewHwndHost();

		Controls::Grid^ maingrid = (Controls::Grid^ ) this->MainWindow->FindName("MainGrid");
		if (maingrid != nullptr)
		{
			Controls::Grid^ windowgrid = gcnew Controls::Grid();
			System::Windows::Media::SolidColorBrush^ br = gcnew  System::Windows::Media::SolidColorBrush(Colors::Yellow);
			windowgrid->Background = br;
			windowgrid->Children->Add(host);
			maingrid->Children->Add(windowgrid);
			return S_OK;
		}
		return E_FAIL;
	}

	HRESULT App::ApplyTheme()
	{
		if (Resources->MergedDictionaries->Count > 0)
		{
			System::Windows::ResourceDictionary^ dic = Resources->MergedDictionaries[0];
			System::Windows::Style^ st = (System::Windows::Style^)dic["MainWindowStyle"];
			this->MainWindow->Style = st;
		}
		return S_OK;
	}

	HRESULT App::InitializeTheme()
	{
		FileStream^ fs = nullptr;
		String^ xamlFile = nullptr;
		std::wstring themefile = L"WindowsTheme.xaml";
		std::wstring themefolder = L"Themes";


		try
		{
			std::wstring destfile = GetFolder(themefolder, themefile);
			
			String^ stylefile = msclr::interop::marshal_as<String^>(destfile.c_str());


			if (File::Exists(stylefile))
			{
				fs = gcnew FileStream(stylefile, FileMode::Open, FileAccess::Read, FileShare::Read);
				auto dic = (ResourceDictionary^)Markup::XamlReader::Load(fs);
				Resources->MergedDictionaries->Clear();
				Resources->MergedDictionaries->Add(dic);
			}
			else
			{
				std::wstring file = msclr::interop::marshal_as<std::wstring>(xamlFile);

				ATLTRACE(L"The file %s do not exist, terminate the application\n", file.c_str());
				return ERROR_FILE_NOT_FOUND;
			}
		}
		catch (Markup::XamlParseException^ parseexception)
		{
			std::wstring exception = msclr::interop::marshal_as<std::wstring>(parseexception->InnerException->ToString());
			std::wstring file = msclr::interop::marshal_as<std::wstring>(xamlFile);
			ATLTRACE(L"Unable to parse the file:%s ,exception=%s \n", file.c_str(), exception.c_str());
			return parseexception->HResult;
		}
		finally
		{
			if (fs != nullptr)
				fs->Close();
		}
		return S_OK;
	}

	Window^ App::WindowFromXaml()
	{
		FileStream		^fs			= nullptr;
		String			^xamlFile	= nullptr;
		std::wstring xamlfolder = L"XAML";
		std::wstring MainWindowfile = L"MainWindow.xaml";

		try
		{

			std::wstring destfile = GetFolder(xamlfolder, MainWindowfile);

			xamlFile = msclr::interop::marshal_as<String^>(destfile.c_str());

			if (File::Exists(xamlFile))
			{
				fs = gcnew FileStream(xamlFile, FileMode::Open, FileAccess::Read, FileShare::Read);
				Window^ window = (Window^)Markup::XamlReader::Load(fs);
				return window;
			}
			else
			{
				std::wstring file = msclr::interop::marshal_as<std::wstring>(xamlFile);

				ATLTRACE(L"The file %s do not exist, terminate the application\n", file.c_str());
				return nullptr;
			}
		}
		catch (Markup::XamlParseException ^parseexception)
		{
			std::wstring exception = msclr::interop::marshal_as<std::wstring>(parseexception->InnerException->ToString());
			std::wstring file = msclr::interop::marshal_as<std::wstring>(xamlFile);
			ATLTRACE(L"Unable to parse the file:%s ,exception=%s \n", file.c_str(), exception.c_str());
			return nullptr;
		}
		finally
		{
			if (fs != nullptr)
				fs->Close();
		}
	}

	void App::onloaded(Object^ sender, RoutedEventArgs^ ev)
	{
		this->MainWindow->CommandBindings->Add(gcnew CommandBinding(SystemCommands::CloseWindowCommand,
			gcnew ExecutedRoutedEventHandler(this, &App::CommandBinding_Executed_Close),
			gcnew CanExecuteRoutedEventHandler(this, &App::CanExecuteAddPhrase)));

		this->MainWindow->CommandBindings->Add(gcnew CommandBinding(SystemCommands::MaximizeWindowCommand,
			gcnew ExecutedRoutedEventHandler(this, &App::CommandBinding_Executed_Maximize),
			gcnew CanExecuteRoutedEventHandler(this, &App::CanExecuteAddPhrase)));

		this->MainWindow->CommandBindings->Add(gcnew CommandBinding(SystemCommands::MinimizeWindowCommand,
			gcnew ExecutedRoutedEventHandler(this, &App::CommandBinding_Executed_Minimize),
			gcnew CanExecuteRoutedEventHandler(this, &App::CanExecuteAddPhrase)));

		this->MainWindow->CommandBindings->Add(gcnew CommandBinding(SystemCommands::RestoreWindowCommand,
			gcnew ExecutedRoutedEventHandler(this, &App::CommandBinding_Executed_Restore),
			gcnew CanExecuteRoutedEventHandler(this, &App::CanExecuteAddPhrase)));

		
	}

	void App::CommandBinding_Executed_Minimize(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e)
	{
		SystemCommands::MinimizeWindow(this->MainWindow);
	}
	void App::CommandBinding_Executed_Maximize(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e)
	{
		if (this->MainWindow->WindowState == WindowState::Maximized)
		{
			SystemCommands::RestoreWindow(this->MainWindow);
		}
		else
		{
			SystemCommands::MaximizeWindow(this->MainWindow);
		}
	}
	void App::CommandBinding_Executed_Close(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e)
	{
		SystemCommands::CloseWindow(this->MainWindow);
	}
	void App::CommandBinding_Executed_Restore(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e)
	{
		SystemCommands::RestoreWindow(this->MainWindow);
	}

	void App::CanExecuteAddPhrase(System::Object^ sender, System::Windows::Input::CanExecuteRoutedEventArgs^ e)
	{
		e->CanExecute = true;
	}

};






