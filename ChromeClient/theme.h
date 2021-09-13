#pragma once

#include <uxtheme.h>
namespace ChromeClient
{

	typedef int (WINAPI* TGetImmersiveUserColorSetPreference)(bool bForceCheckRegistry, bool bSkipCheckOnFail);
	typedef int (WINAPI* TGetImmersiveColorSetCount)();
	typedef DWORD(WINAPI* TGetImmersiveColorFromColorSetEx)(UINT dwImmersiveColorSet, UINT dwImmersiveColorType, bool bIgnoreHighContrast, UINT dwHighContrastCacheMode);
	typedef const wchar_t** (WINAPI* TGetImmersiveColorNamedTypeByIndex)(UINT dwImmersiveColorType);
	typedef int (WINAPI* TGetImmersiveColorTypeFromName)(const wchar_t* name);

	static TGetImmersiveUserColorSetPreference GetImmersiveUserColorSetPreference;
	static TGetImmersiveColorFromColorSetEx GetImmersiveColorFromColorSetEx;
	static TGetImmersiveColorSetCount GetImmersiveColorSetCount;
	static TGetImmersiveColorNamedTypeByIndex GetImmersiveColorNamedTypeByIndex;
	static TGetImmersiveColorTypeFromName GetImmersiveColorTypeFromName;

	enum class system_color { accent, background, taskbar };
	
	public class themecolor
	{
	public :
		std::wstring name;
		DWORD		 rgb;
	};

	public class theme
	{
		private:
			std::map<std::wstring, DWORD> colors;
		
		private:
			static HRESULT LoadFunction()
			{

				HMODULE hUxTheme = LoadLibrary(L"uxtheme.dll");
				if (!IsWindows10OrGreater() && hUxTheme != nullptr)
				{
					GetImmersiveUserColorSetPreference = (TGetImmersiveUserColorSetPreference)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(98));
					GetImmersiveColorFromColorSetEx = (TGetImmersiveColorFromColorSetEx)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(95));
					GetImmersiveColorSetCount = (TGetImmersiveColorSetCount)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(94));
					GetImmersiveColorNamedTypeByIndex = (TGetImmersiveColorNamedTypeByIndex)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(100));
					GetImmersiveColorTypeFromName = (TGetImmersiveColorTypeFromName)GetProcAddress(hUxTheme, MAKEINTRESOURCEA(96));

					if (GetImmersiveUserColorSetPreference && GetImmersiveColorFromColorSetEx && GetImmersiveColorSetCount && GetImmersiveColorNamedTypeByIndex && GetImmersiveColorTypeFromName)
					{
						return S_OK;
					}
				}
				return (ERROR_INVALID_FUNCTION);
			}

			void loadallcolors()
			{
				if (SUCCEEDED(LoadFunction()))
				{
					int nItem = 0;
					for (int i = 0; i <= 1250; i++)
					{
						const wchar_t** pName = GetImmersiveColorNamedTypeByIndex(i);
						if (pName)
						{
							std::wstring name = *pName;
							std::wstring nameimmersive = L"Immersive" + name;
							DWORD rgba = GetImmersiveColorFromColorSetEx(GetImmersiveUserColorSetPreference(false, false), GetImmersiveColorTypeFromName(nameimmersive.c_str()), false, 0);
							colors.insert(std::make_pair(nameimmersive, rgba));
						}
					}
				}
			}
		public:
			DWORD getcolor(std::wstring name)
			{
				if (colors.size() == 0)
				{
					loadallcolors();
				}

				if (colors.find(name) != colors.end()) {
					return colors[name];
				}
				else 
				{
					return 0;
				}
			}
		
			DWORD getcolor(bool reload, std::wstring name)
			{

				colors.clear();

				if (colors.size() == 0)
				{
					loadallcolors();
				}

				if (colors.find(name) != colors.end()) {
					return colors[name];
				}
				else
				{
					return 0;
				}
			}		
	};
};