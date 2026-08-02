#include "stdafx.h"

#include "CSysAllocator.h"

namespace
{
void* (*Allocate)(size_t size) = nullptr;
void (*Deallocate)(void* memory) = nullptr;

CMemory::Hook AllocatorHooks([] {
    constexpr CMemory::Pattern allocatePattern(
        "E8 ? ? ? ? 48 85 C0 74 ? 48 83 20 ? 0F 57 C0 0F 11 40 ? 0F 11 40 ? 83 60");
    constexpr CMemory::Pattern deallocatePattern(
        "E8 ? ? ? ? 0F B6 43 ? 48 8D 7F ?");

    Allocate = allocatePattern.Search().GetCall<decltype(Allocate)>();
    Deallocate = deallocatePattern.Search().GetCall<decltype(Deallocate)>();
});
}

void* CSysAllocator::Alloc(size_t size)
{
    return Allocate(size);
}

void CSysAllocator::Dealloc(void* memory)
{
    Deallocate(memory);
}
