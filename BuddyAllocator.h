#ifndef _BuddyAllocator_h_                   // include file only once
#define _BuddyAllocator_h_
#include <iostream>
#include <vector>
#include <math.h>
using namespace std;
typedef unsigned int uint;

/* declare types as you need */

class BlockHeader{
public:
	// think about what else should be included as member variables
	int block_size;  // size of the block
	BlockHeader *next; // pointer to the next block
    BlockHeader()
    {
        next = nullptr;
        block_size = 0;
    }
};

class LinkedList{
	// this is a special linked list that is made out of BlockHeader s. 
public:
	BlockHeader* head;		// you need a head of the list
public:
    LinkedList()
    {
        head = nullptr;
    }
    LinkedList(BlockHeader *b)
    {
        head = b;
    }

	void insert (BlockHeader* b)	// adds a block to the list
    {
        if(head == nullptr)
        {
            head = b;
            head->next = nullptr;
            return;
        }
        b->next = head->next;
        head->next = b;
	}

	void remove (BlockHeader* b)  // removes a block from the list
    {
        if(head == nullptr)
        {
            return;
        }

        if(head == b)
        {
            head = head->next;
            return;
        }

        BlockHeader *curr = head;
        while(curr->next != nullptr)
        {
            if(curr->next == b)
            {
                curr->next = b->next;
                break;
            }
            curr = curr->next;
        }
	}
};


class BuddyAllocator{
private:
	/* declare more member variables as necessary */
    char *mem;
	vector<LinkedList> FreeList;
	int basic_block_size;
	int total_memory_size;

private:
	/* private function you are required to implement
	 this will allow you and us to do unit test */
	
	BlockHeader* getbuddy (BlockHeader * addr); 
	// given a block address, this function returns the address of its buddy 
	
	bool arebuddies (BlockHeader* block1, BlockHeader* block2);
	// checks whether the two blocks are buddies are not

	BlockHeader* merge (BlockHeader* block1, BlockHeader* block2);
	// this function merges the two blocks returns the beginning address of the merged block
	// note that either block1 can be to the left of block2, or the other way around

	BlockHeader* split (BlockHeader* block);
	// splits the given block by putting a new header halfway through the block
	// also, the original header needs to be corrected
    BlockHeader* SplitRec(BlockHeader *b, int _length);


public:
	BuddyAllocator (int _basic_block_size, int _total_memory_length); 
	/* This initializes the memory allocator and makes a portion of 
	   ’_total_memory_length’ bytes available. The allocator uses a ’_basic_block_size’ as 
	   its minimal unit of allocation.  
	*/ 

	~BuddyAllocator(); 
	/* Destructor that gives any allocated memory back to the operating system. 
	   There should not be any memory leakage (i.e., memory staying allocated).
	*/ 

	char* alloc(int _length); 
	/* Allocate _length number of bytes of free memory and returns the 
		address of the allocated portion. Returns 0 when out of memory. */ 

	int free(char* _a); 
	/* Frees the section of physical memory previously allocated 
	   using ’my_malloc’. Returns 0 if everything ok. */ 

    bool isFree(BlockHeader *b)
    {
        if(!ValidMemoryLocation(b))
        {
            return false;
        }
        int index = log2(b->block_size / basic_block_size);
        BlockHeader *curr = FreeList[index].head;
        while(curr != nullptr)
        {
            if(curr == b)
            {
                return true;
            }
            curr = curr->next;
        }
        return false;
    }

    bool ValidMemoryLocation(BlockHeader* block);
   
	void printlist ();
	/* Mainly used for debugging purposes and running short test cases */
	/* This function should print how many free blocks of each size belong to the allocator
	at that point. The output format should be the following (assuming basic block size = 128 bytes):

	[0] (128): 5
	[1] (256): 0
	[2] (512): 3
	[3] (1024): 0
	....
	....
	 which means that at this point, the allocator has 5 128 byte blocks, 3 512 byte blocks and so on.*/
};

#endif 
