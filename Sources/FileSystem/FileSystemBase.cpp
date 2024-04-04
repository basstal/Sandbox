#include "pch.hpp"

#include "FileSystemBase.hpp"

#ifdef _WIN64
#include <windows.h>

std::string LPWSTRToString(WCHAR* lpwstr)
{
    if (lpwstr == nullptr)
        return std::string();

    // 获取所需的字符数（不包括终止空字符）
    int len = WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, nullptr, 0, nullptr, nullptr);

    // 分配足够的空间
    std::string str(len, '\0');

    // 实际进行转换
    WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, &str[0], len, nullptr, nullptr);

    // 返回转换后的字符串（删除最后的 '\0'）
    return str.substr(0, len - 1);
}

std::filesystem::path Sandbox::FileSystemBase::GetProjectRoot()
{
#ifdef UNICODE
    WCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::filesystem::path executablePath = std::filesystem::path::path(LPWSTRToString(path));
    // ** TODO:这里会因为构建系统不同而导致层级不同，需要想办法解决这个问题
    return executablePath.parent_path().parent_path();
#else
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    std::filesystem::path executablePath = std::filesystem::path::path(std::string(path));
    // ** TODO:这里会因为构建系统不同而导致层级不同，需要想办法解决这个问题
    return executablePath.parent_path().parent_path();
#endif
}
#else
error "Unsupported platform"
#endif
