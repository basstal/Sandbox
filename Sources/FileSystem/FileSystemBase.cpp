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

std::string Sandbox::FileSystemBase::GetExecutablePath()
{
#ifdef UNICODE
    WCHAR path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    return LPWSTRToString(path);
#else
    char path[MAX_PATH];
    GetModuleFileName(NULL, path, MAX_PATH);
    return std::string(path);
#endif
}
#else
error "Unsupported platform"
#endif
