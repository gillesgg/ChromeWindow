#pragma once
namespace ChromeClient
{
	class osutility
	{
	public:
		static HRESULT LoadResourcestofile();
		static std::wstring osutility::GetFolder(std::wstring folder, std::wstring file);
		
		static std::wstring themefolder();
		static std::wstring xamlfolder();
		static std::wstring xamlmainwindowfile();
		static std::wstring themefile();

	public:
		

	private:
		static HRESULT LoadResourcetofile(std::wstring folder, std::wstring filename, int id, std::wstring name);
		static std::wstring GetExecutableName(std::wstring path);
		static std::wstring GetExecutableName();
	};
};