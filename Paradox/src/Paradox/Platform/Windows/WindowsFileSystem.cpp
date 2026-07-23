#include "pxpch.h"
#include "Paradox/Core/FileSystem.h"

#include "Paradox/Core/Application.h"

#include <shobjidl.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace Paradox
{
#define CHECK_HRESULT(hr, msg)\
    if (FAILED(hr))\
    {\
        PX_CORE_ERROR("{0}: {1}", msg, hr);\
        return {};\
    }

    std::filesystem::path FileSystem::SelectFile(const std::string& title, const std::string& filter)
    {
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        CHECK_HRESULT(hr, "Failed to initialize COM library");

        struct ComGuard
        {
            ~ComGuard() { CoUninitialize(); }
        } guard;

        ComPtr<IFileOpenDialog> dialog;

        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
        FILEOPENDIALOGOPTIONS options;
        dialog->GetOptions(&options);
        dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_FILEMUSTEXIST);
        CHECK_HRESULT(hr, "Failed to create dialog instance");

        std::wstring wTitle(title.begin(), title.end());
        dialog->SetTitle(wTitle.c_str());

        std::wstring wFilter(filter.begin(), filter.end());
        size_t pos = wFilter.find(L'|');
        std::wstring name = wFilter.substr(0, pos);
        std::wstring pattern = wFilter.substr(pos + 1);
        COMDLG_FILTERSPEC filterSpec = { name.c_str(), pattern.c_str() };
        dialog->SetFileTypes(1, &filterSpec);

        hr = dialog->Show(GetActiveWindow());
        if (FAILED(hr))
            return {}; // Dialog window was probably closed without selecting anything

        ComPtr<IShellItem> pItem;
        hr = dialog->GetResult(&pItem);
        CHECK_HRESULT(hr, "Failed to get dialog result");

        PWSTR pszFilePath;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
        CHECK_HRESULT(hr, "Failed to get file path");

        std::filesystem::path result = std::filesystem::path(pszFilePath);
        CoTaskMemFree(pszFilePath);
        return result;
    }

	std::filesystem::path FileSystem::SelectFolder(const std::string& title)
	{
        HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
        CHECK_HRESULT(hr, "Failed to initialize COM library");

        struct ComGuard
        {
            ~ComGuard() { CoUninitialize(); }
        } guard;

        ComPtr<IFileOpenDialog> dialog;

        hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&dialog));
        FILEOPENDIALOGOPTIONS options;
        dialog->GetOptions(&options);
        dialog->SetOptions(options | FOS_FORCEFILESYSTEM | FOS_PICKFOLDERS);
        CHECK_HRESULT(hr, "Failed to create dialog instance");

        std::wstring wTitle(title.begin(), title.end());
        dialog->SetTitle(wTitle.c_str());
        hr = dialog->Show(GetActiveWindow());
        if (FAILED(hr))
			return {}; // Dialog window was probably closed without selecting anything
        
        ComPtr<IShellItem> pItem;
        hr = dialog->GetResult(&pItem);
		CHECK_HRESULT(hr, "Failed to get dialog result");

        PWSTR pszFilePath;
        hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
		CHECK_HRESULT(hr, "Failed to get file path");

        std::filesystem::path result = std::filesystem::path(pszFilePath);
        CoTaskMemFree(pszFilePath);
        return result;
	}

    void FileSystem::ShowFolder(std::filesystem::path path)
    {
		ShellExecuteW(NULL, L"explore", path.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
    }

    void FileSystem::OpenFileWithDefaultProgram(std::filesystem::path path)
    {
        ShellExecuteW(NULL, NULL, path.wstring().c_str(), NULL, NULL, SW_SHOWNORMAL);
    }
}