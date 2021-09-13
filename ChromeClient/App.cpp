#include "stdafx.h"
#include "App.h"
#include "theme.h"
#include "osutility.h"
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


	HRESULT App::Initialize()
	{
		HRESULT hr;
		
		SystemEvents::UserPreferenceChanged += gcnew Microsoft::Win32::UserPreferenceChangedEventHandler(this, &ChromeClient::App::OnUserPreferenceChanged);

		if (FAILED(hr = osutility::LoadResourcestofile()))
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


		try
		{
			std::wstring destfile = osutility::GetFolder(osutility::themefolder(), osutility::themefile());
			
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

		try
		{

			std::wstring destfile = osutility::GetFolder(osutility::xamlfolder(), osutility::xamlmainwindowfile());

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






