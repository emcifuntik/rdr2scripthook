#include "stdafx.h"
#include "CSysAllocator.h"

void* (*_alloc)(size_t size) = nullptr;
void(*_dealloc)(void* mem) = nullptr;

void* CSysAllocator::Alloc(size_t size)
{
	return _alloc(size);
}

void CSysAllocator::Dealloc(void* mem)
{
	_dealloc(mem);
}

CMemory::Hook AllocHooks([] {
	constexpr CMemory::Pattern allocPat("E8 ? ? ? ? 48 85 C0 74 ? 48 83 20 ? 0F 57 C0 0F 11 40 ? 0F 11 40 ? 83 60");
	constexpr CMemory::Pattern deallocPat("E8 ? ? ? ? 0F B6 43 ? 48 8D 7F ?");

	_alloc = allocPat.Search().GetCall<decltype(_alloc)>();
	_dealloc = deallocPat.Search().GetCall<decltype(_dealloc)>();
});