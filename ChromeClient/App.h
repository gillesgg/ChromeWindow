#pragma once


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
	};


};
