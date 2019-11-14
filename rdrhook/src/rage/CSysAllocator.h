#pragma once
#include "scripting/CSingleton.h"

class CSysAllocator: public CSingleton<CSysAllocator>
{
public:
	void* Alloc(size_t size);
	void Dealloc(void* mem);
};

