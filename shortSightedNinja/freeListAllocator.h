#pragma once

#define KB(x) (x) * 1024
#define MB(x) KB((x)) * 1024
#define GB(x) MB((x)) * 1024

struct FreeListAllocator
{
	char* baseMemory;

	FreeListAllocator() = default;
	FreeListAllocator(void* baseMemory, size_t memorySize)
	{
		init(baseMemory, memorySize);
	}

	void init(void* baseMemory, size_t memorySize);

	void* allocate(size_t size);

	void free(void* mem);

	void* threadSafeAllocate(size_t size);

	void threadSafeFree(void* mem);


	volatile int initialized = 0;
	void* end = {};

	//std::mutex mu = {};

	size_t getEnd()
	{
		return (size_t)end;
	}

};

