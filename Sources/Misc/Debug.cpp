#include "pch.hpp"

#include "Debug.hpp"

#ifdef _WIN64
#include <windows.h>
// "dbghelp.h" 必须在 <windows.h> 后面，因此使用 "" 以避免被自动格式化到前面去
#include "dbghelp.h"
#endif

std::string Sandbox::GetCallStack()
{
#ifdef _WIN64
    void*          stack[100];
    unsigned short frames;
    SYMBOL_INFO*   symbol;
    HANDLE         process = GetCurrentProcess();
    std::string    callStack;

    SymInitialize(process, NULL, TRUE);
    frames               = CaptureStackBackTrace(0, 100, stack, NULL);
    symbol               = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen   = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    IMAGEHLP_LINE64 line = {0};
    line.SizeOfStruct    = sizeof(IMAGEHLP_LINE64);
    DWORD displacement;

    char buffer[512];  // Buffer for formatting the output, moved outside the loop
    bool inStdLibBlock = false;  // Flag to track blocks of standard library calls
    int  stdLibCount   = 0;  // Count consecutive standard library frames

    for (int i = 0; i < frames; i++)
    {
        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, 0, symbol);

        bool isStdLib = strstr(symbol->Name, "std::") != nullptr;
        if (isStdLib)
        {
            if (!inStdLibBlock)
            {
                inStdLibBlock = true;
                stdLibCount   = 1;
            }
            else
            {
                stdLibCount++;
            }
        }
        else
        {
            if (inStdLibBlock)
            {
                sprintf_s(buffer, "[std library calls x%d]\n", stdLibCount);
                callStack += buffer;
                inStdLibBlock = false;
            }
            if (SymGetLineFromAddr64(process, address, &displacement, &line))
            {
                sprintf_s(buffer, "%i: %s - 0x%0llX (File: %s:%lu)\n", frames - i - 1, symbol->Name, symbol->Address, line.FileName, line.LineNumber);
            }
            else
            {
                sprintf_s(buffer, "%i: %s - 0x%0llX\n", frames - i - 1, symbol->Name, symbol->Address);
            }
            callStack += buffer;
        }
    }

    if (inStdLibBlock)
    {
        sprintf_s(buffer, "[std library calls x%d]\n", stdLibCount);
        callStack += buffer;
    }

    free(symbol);
    return callStack;
#else
    throw std::runtime_error("not implemented");
#endif
}