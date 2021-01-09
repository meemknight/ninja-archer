#include "freeListAllocator.h"
#include <cassert>
#include <cstdint>
#include <iostream>

//char memBlock[GB(1)] = {};
//FreeListAllocator allocator(memBlock, sizeof(memBlock));

extern FreeListAllocator allocator;

void* operator new  (std::size_t count)
{
	auto a = allocator.allocate(count);

	return a;
}

void* operator new[](std::size_t count)
{
	auto a = allocator.allocate(count);

	return a;
}

void operator delete  (void* ptr)
{
	allocator.free(ptr);
}

void operator delete[](void* ptr)
{
	allocator.free(ptr);
}


// todo rename
const uint64_t DUMMY_VALUE = 0xff'ff'ff'ff'ff'ff'ff'ff;

struct FreeBlock
{
	union
	{
		char* next;
		std::uint64_t dummy_;
	};
	std::uint64_t size;

};

struct AllocatedBlock
{
	std::uint64_t size;
	std::uint64_t dummy_; // todo rename
};


void FreeListAllocator::init(void* baseMemory, size_t memorySize)
{
	end = (void*)((size_t)baseMemory + memorySize);

	static_assert(sizeof(FreeBlock) == sizeof(AllocatedBlock), "");

	assert(memorySize > 100);

	this->baseMemory = (char*)baseMemory;

	//move base memory to a 8 byte aligned

	size_t pos = (int)this->baseMemory;

	if (pos % 8 != 0)
	{
		this->baseMemory += 8 - (pos % 8);
		memorySize -= 8 - (pos % 8);
	}

	((FreeBlock*)this->baseMemory)->next = nullptr;
	((FreeBlock*)this->baseMemory)->size = memorySize - sizeof(FreeBlock);


}

void* FreeListAllocator::allocate(size_t size)
{

	FreeBlock* last = nullptr;
	FreeBlock* current = (FreeBlock*)baseMemory;

	int aligned8Size = size;
	if (aligned8Size % 8 != 0)
	{
		aligned8Size += (8 - (aligned8Size % 8));
	}

	while (true)
	{

		//todo handle case when allocate near end
		//todo handle case when a very small block remains

		if (aligned8Size <= ((FreeBlock*)current)->size) // this is a suitable block
		{
			//add block here


			FreeBlock* next = (FreeBlock*)current->next;

			if (next == nullptr || next >= this->end) //this is the last block
			{
				if (last == nullptr || last >= this->end) //this is also the first block so move the base pointer
				{
					void* toReturn = (char*)current + sizeof(AllocatedBlock);
					((AllocatedBlock*)current)->size = aligned8Size;	//size of the new allocated block
					((AllocatedBlock*)current)->dummy_ = DUMMY_VALUE;

					FreeBlock* nextFreeBlock = (FreeBlock*)((char*)toReturn + aligned8Size);
					//next free block is the base memory now

					baseMemory = (char*)nextFreeBlock;
					((FreeBlock*)baseMemory)->next = nullptr; //this is the last block

					size_t size = this->getEnd() - (size_t)nextFreeBlock - (size_t)sizeof(FreeBlock); //set the size of the new last block
					((FreeBlock*)baseMemory)->size = size;

					return toReturn;
				}
				else //this is not the first block so change the last block pointer
				{
					void* toReturn = (char*)current + sizeof(AllocatedBlock);
					((AllocatedBlock*)current)->size = aligned8Size;	//size of the new allcoated block
					((AllocatedBlock*)current)->dummy_ = DUMMY_VALUE;

					FreeBlock* nextFreeBlock = (FreeBlock*)((char*)toReturn + aligned8Size);

					last->next = (char*)nextFreeBlock; //last is relinked
					nextFreeBlock->next = nullptr; //this is the last block

					size_t size = this->getEnd() - (size_t)nextFreeBlock - (size_t)sizeof(FreeBlock); //set the size of the new last block
					nextFreeBlock->size = size;

					return toReturn;
				}

			}
			else //this is not the last free block 
			{

				if (last == nullptr || last >= this->end) // this is the first free block but not the last 
				{
					size_t currentSize = ((FreeBlock*)current)->size;

					void* toReturn = (char*)current + sizeof(AllocatedBlock);

					if (currentSize - aligned8Size < 24)
					{
						//too small block remaining

						if (currentSize - aligned8Size < 0 || (currentSize - aligned8Size) % 8 != 0)
						{
							//heap corrupted
							assert(0);
						}

						aligned8Size += (currentSize - aligned8Size);

						((AllocatedBlock*)current)->size = aligned8Size;	//size of the new allocated block
						((AllocatedBlock*)current)->dummy_ = DUMMY_VALUE;

						FreeBlock* nextFreeBlock = next;
						//next free block is the next block

						baseMemory = (char*)nextFreeBlock;

						return toReturn;

					}
					else
					{
						//add a new block
						FreeBlock* newCreatedBlock = (FreeBlock*)((char*)toReturn + aligned8Size);
						newCreatedBlock->size = currentSize - aligned8Size - sizeof(AllocatedBlock);
						newCreatedBlock->next = (char*)next;

						((AllocatedBlock*)current)->size = aligned8Size;	//size of the new allocated block
						((AllocatedBlock*)current)->dummy_ = DUMMY_VALUE;
						baseMemory = (char*)newCreatedBlock;

						return toReturn;
					}



				}
				else // this is not the first free block and not the last 
				{	//todo fix here

					void* toReturn = (char*)current + sizeof(AllocatedBlock);
					size_t currentSize = ((FreeBlock*)current)->size;

					if (currentSize - aligned8Size < 24)
					{
						//too small block remaining
						if (currentSize - aligned8Size < 0 || (currentSize - aligned8Size) % 8 != 0)
						{
							//heap corrupted
							assert(0);
						}

						aligned8Size += (currentSize - aligned8Size);

						((AllocatedBlock*)current)->size = aligned8Size;	//size of the new allcoated block
						((AllocatedBlock*)current)->dummy_ = DUMMY_VALUE;

						//FreeBlock* nextFreeBlock = (FreeBlock*)((char*)toReturn + aligned8Size);

						last->next = (char*)next; //last is relinked
						//nextFreeBlock->next = (char*)next; //this is not the last block

						//size_t size = this->getEnd() - (size_t)nextFreeBlock - (size_t)sizeof(FreeBlock); //set the size of the new last block
						//nextFreeBlock->size = size;

						return toReturn;

					}
					else
					{
						//add a new block
						FreeBlock* newCreatedBlock = (FreeBlock*)((char*)toReturn + aligned8Size);
						newCreatedBlock->size = currentSize - aligned8Size - sizeof(AllocatedBlock);
						newCreatedBlock->next = (char*)next;

						((AllocatedBlock*)current)->size = aligned8Size;	//size of the new allcoated block
						((AllocatedBlock*)current)->dummy_ = DUMMY_VALUE;

						//FreeBlock* nextFreeBlock = (FreeBlock*)((char*)toReturn + aligned8Size);

						last->next = (char*)newCreatedBlock; //last is relinked
						//nextFreeBlock->next = (char*)next; //this is not the last block

						//size_t size = this->getEnd() - (size_t)nextFreeBlock - (size_t)sizeof(FreeBlock); //set the size of the new last block
						//nextFreeBlock->size = size;

						return toReturn;

					}


				}
			}

		}
		else
		{
			if (current->next == nullptr || current->next >= this->end)
			{
				//that was the last block, no size
				std::cout << "no more memory\n";
				assert(0);
			}
			else
			{
				last = current;
				current = (FreeBlock*)current->next;

			}

		}


	}


	return nullptr;
}

void FreeListAllocator::free(void* mem)
{

	char* memoryBegin = (char*)mem;

	//mem is the beginning of the valid memory, \
	the header is then 16 bytes lower

	void* headerBegin = memoryBegin - sizeof(AllocatedBlock);

	AllocatedBlock* allocatedBLockHeader = (AllocatedBlock*)headerBegin;

#pragma region check validity
	//todo make this a controllable macro

	//todo add optional logging
	assert(allocatedBLockHeader->dummy_ == DUMMY_VALUE); //invalid free or double free
	allocatedBLockHeader->dummy_ = 0;
#pragma endregion

	size_t sizeOfTheFreedBlock = allocatedBLockHeader->size;

	if (headerBegin < this->baseMemory)
	{
		//the freed memory is before the base memory so change the base memory
		//this is the new base memory

		if ((size_t)headerBegin + sizeof(AllocatedBlock) + sizeOfTheFreedBlock == (size_t)this->baseMemory)
		{
			//this merges with the current first free block so merge them

			FreeBlock* firstFreeBlock = (FreeBlock*)allocatedBLockHeader;
			firstFreeBlock->next = ((FreeBlock*)this->baseMemory)->next;
			firstFreeBlock->size = sizeOfTheFreedBlock + sizeof(FreeBlock) + ((FreeBlock*)this->baseMemory)->size;

			this->baseMemory = (char*)firstFreeBlock;

		}
		else if ((size_t)headerBegin + sizeof(AllocatedBlock) + sizeOfTheFreedBlock > (size_t)this->baseMemory)
		{
			//heap corupted
			assert(0);
		}
		else
		{	//this doesn't merge with the next free block so just link them
			FreeBlock* firstFreeBlock = (FreeBlock*)allocatedBLockHeader;

			firstFreeBlock->next = this->baseMemory;
			firstFreeBlock->size = sizeOfTheFreedBlock;

			this->baseMemory = (char*)firstFreeBlock;
		}


	}
	else
	{
		//the freed block is somewhere in the middle
		//first search for the blocks before and after it

		FreeBlock* current = (FreeBlock*)baseMemory;
		FreeBlock* next = (FreeBlock*)current->next;

		while (true)
		{

			if ((current < headerBegin && headerBegin < next)
				||
				(current < headerBegin && (next == nullptr || next >= this->end))
				)
			{
				//the block is between 2 blocks
				FreeBlock* theBlockBefore = current;
				FreeBlock* theBlockAfter = (FreeBlock*)current->next;

				//merge with the block before

				FreeBlock* newCurent = nullptr;
				//check if merged
				if ((size_t)theBlockBefore + sizeof(FreeBlock) + theBlockBefore->size == (size_t)headerBegin)
				{
					//merge
					theBlockBefore->size += sizeof(FreeBlock) + sizeOfTheFreedBlock;

					newCurent = theBlockBefore;
				}
				else if ((size_t)theBlockBefore + sizeof(FreeBlock) + theBlockBefore->size > (size_t)headerBegin)
				{
					//error heap corupted
					assert(0);
				}
				else
				{
					//just link, no merge
					theBlockBefore->next = (char*)headerBegin;
					FreeBlock* thisBlock = (FreeBlock*)headerBegin;
					thisBlock->next = nullptr;
					thisBlock->size = sizeOfTheFreedBlock;

					newCurent = thisBlock;
				}

				//merge / link with next block
				if (theBlockAfter != nullptr && theBlockAfter < this->end)
				{

					if ((size_t)newCurent + sizeof(FreeBlock) + newCurent->size == (size_t)theBlockAfter)
					{
						//merge
						newCurent->size += sizeof(FreeBlock) + theBlockAfter->size;
						newCurent->next = theBlockAfter->next;

					}
					else if ((size_t)newCurent + sizeof(FreeBlock) + newCurent->size > (size_t)theBlockAfter)
					{
						//err
						assert(0);
					}
					else
					{
						//just link
						newCurent->next = (char*)theBlockAfter;

					}

				}


				break;
			}

			current = (FreeBlock*)current->next;
			next = (FreeBlock*)current->next;

			if (current == nullptr || current >= this->end)
			{
				//heap corupted or freed an invalid value
				assert(0);
			}
		}
	}

}


void* FreeListAllocator::threadSafeAllocate(size_t size)
{
	//if(initialized)
	//mu.lock();
	//
	//auto a = this->allocate(size);
	//
	//if (initialized)
	//mu.unlock();

	//return a;
	return 0;
}

void FreeListAllocator::threadSafeFree(void* mem)
{
	//if (initialized)
	//mu.lock();
	//
	//this->free(mem);
	//
	//if (initialized)
	//mu.unlock();
}
