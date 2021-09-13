#pragma once

#include "theme.h"

namespace ChromeClient
{
	using namespace System::Windows;
	using namespace System;

	public ref class App : Application
	{
	public:
		App();
	
	public:		
		void onloaded(Object^ sender, RoutedEventArgs^ ev);
		
		void CommandBinding_Executed_Minimize(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e);
		void CommandBinding_Executed_Maximize(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e);
		void CommandBinding_Executed_Close(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e);
		void CommandBinding_Executed_Restore(System::Object^ sender, System::Windows::Input::ExecutedRoutedEventArgs^ e);
		void CanExecuteAddPhrase(System::Object^ sender, System::Windows::Input::CanExecuteRoutedEventArgs^ e);
	private:
		HRESULT Initialize();
		Window^ WindowFromXaml();
		HRESULT InitializeTheme();
		HRESULT ApplyTheme();
		HRESULT LoadHTMLView();
		void	ApplyWindowsTheme();
		HRESULT	LoadResourcestofile();
		HRESULT LoadResourcetofile(std::wstring folder, std::wstring filename, int id, std::wstring name);
		std::wstring GetFolder(std::wstring folder, std::wstring file);
		std::wstring GetExecutableName(std::wstring path);
		std::wstring App::GetExecutableName();

	private:
		

	private:
		ChromeClient::theme *windowstheme_;
		void OnUserPreferenceChanged(System::Object^ sender, Microsoft::Win32::UserPreferenceChangedEventArgs^ e);

	protected:
		//void OnSourceInitialized(System::EventArgs e) override;
		//void OnSourceInitialized(System::Object^ sender, System::EventArgs^ e);
	};


};
