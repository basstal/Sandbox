#pragma once
#include <memory>


namespace Sandbox
{
    // Wrapper functions for aligned memory allocation
    // There is currently no standard for this in C++ that works across all platforms and vendors, so we abstract this
    void *AlignedAlloc(size_t size, size_t alignment);
}
