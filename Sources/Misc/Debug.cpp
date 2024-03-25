#include "pch.hpp"

#include "Debug.hpp"

#ifdef _WIN64
#include <windows.h>
#include <dbghelp.h>
#endif

std::string Sandbox::GetCallStack()
{
#ifdef _WIN64
    void* stack[100];
    unsigned short frames;
    SYMBOL_INFO* symbol;
    HANDLE process;
    std::string callStack;

    process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    frames = CaptureStackBackTrace(1, 100, stack, NULL);
    symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
    symbol->MaxNameLen = 255;
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

    IMAGEHLP_LINE64 line = {0};
    line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
    DWORD displacement;

    for (int i = 0; i < frames; i++)
    {
        DWORD64 address = (DWORD64)(stack[i]);
        SymFromAddr(process, address, 0, symbol);
        char buffer[512]; // Assume this buffer is large enough.

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

    free(symbol);
    return callStack;
#else
    throw std::runtime_error("not implemented");
#endif
}
