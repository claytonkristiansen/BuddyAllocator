#include "BuddyAllocator.h"
#include <iostream>
using namespace std;

bool ValidMemoryLocation(BlockHeader *block);

BuddyAllocator::BuddyAllocator(int _basic_block_size, int _total_memory_length)
{
    if (log2(_basic_block_size) != (int)log2(_basic_block_size))
    {
        throw(std::runtime_error("Block size must be power of two"));
    }
    int nearestPowTwo = NearestPowTwo(_total_memory_length / _basic_block_size);
    total_memory_size = _basic_block_size * pow(2, nearestPowTwo);
    basic_block_size = _basic_block_size;
    mem = new char[total_memory_size];

    for (int i = 0; i < nearestPowTwo; ++i)
    {
        FreeList.push_back(LinkedList());
    }
    BlockHeader *b = (BlockHeader *)mem;
    b->block_size = total_memory_size;
    b->next = nullptr;
    FreeList.push_back(LinkedList(b));
}

BuddyAllocator::~BuddyAllocator()
{
    delete[] mem;
}

char *BuddyAllocator::alloc(int _length)
{
    double base = ((double)(_length + sizeof(BlockHeader))) / ((double)basic_block_size);
    int index = NearestPowTwo(ceil(base));
    for (; FreeList[index].head == nullptr && index <= FreeList.size(); ++index)
    {
    }
    BlockHeader *allocatedMemory = SplitRec(FreeList[index].head, _length);
    FreeList[log2(allocatedMemory->block_size / basic_block_size)].remove(allocatedMemory);
    return reinterpret_cast<char *>(reinterpret_cast<long>(allocatedMemory) + sizeof(BlockHeader));
}

int BuddyAllocator::free(char *_a)
{
    BlockHeader *block = reinterpret_cast<BlockHeader *>(reinterpret_cast<long>(_a) - sizeof(BlockHeader));

    if (!ValidMemoryLocation(block))
    {
        return 1;
    }

    BlockHeader *buddy = getbuddy(block);
    if (isFree(buddy))
    {
        merge(block, buddy);
    }
    else
    {
        FreeList[NearestPowTwo(block->block_size / basic_block_size)].insert(block);
    }
    return 0;
}

BlockHeader *BuddyAllocator::merge(BlockHeader *block1, BlockHeader *block2)
{
    int indexRem = NearestPowTwo(block2->block_size / basic_block_size);
    int indexAdd = indexRem + 1;
    block1->block_size *= 2;
    FreeList[indexRem].remove(block1);
    FreeList[indexRem].remove(block2);
    FreeList[indexAdd].insert(block1);
    BlockHeader *buddy = getbuddy(block1);
    if (isFree(buddy))
    {
        merge(block1, buddy);
    }
    return block1;
}

BlockHeader *BuddyAllocator::split(BlockHeader *block)
{
    int indexRem = NearestPowTwo(block->block_size / basic_block_size);
    int indexAdd = indexRem - 1;
    block->block_size /= 2;
    BlockHeader *block2 = getbuddy(block);
    block2->block_size = block->block_size;
    FreeList[indexRem].remove(block);
    FreeList[indexAdd].insert(block);
    FreeList[indexAdd].insert(block2);
    return block;
}

void BuddyAllocator::printlist()
{
    cout << "Printing the Freelist in the format \"[index] (block size) : # of blocks\"" << endl;
    for (int i = 0; i < FreeList.size(); i++)
    {
        if (FreeList[i].head != nullptr)
        {
            cout << "[" << i << "] (" << FreeList[i].head->block_size /*((1 << i) * basic_block_size)*/ << ") : "; // block size at index should always be 2^i * bbs
        }
        else
        {
            cout << "[" << i << "] (" << ((1 << i) * basic_block_size) << ") : "; // block size at index should always be 2^i * bbs
        }
        int count = 0;
        BlockHeader *b = FreeList[i].head;
        // go through the list from head to tail and count
        while (b)
        {
            count++;
            // block size at index should always be 2^i * bbs
            // checking to make sure that the block is not out of place
            if (b->block_size != (1 << i) * basic_block_size)
            {
                cerr << "ERROR:: Block is in a wrong list" << endl;
                exit(-1);
            }
            b = b->next;
        }
        cout << count << endl;
    }
}

BlockHeader *BuddyAllocator::getbuddy(BlockHeader *addr)
{
    BlockHeader *block2 = reinterpret_cast<BlockHeader *>(((reinterpret_cast<long>(addr) - reinterpret_cast<long>(mem)) ^ addr->block_size) + reinterpret_cast<long>(mem));
    return block2;
}

BlockHeader *BuddyAllocator::SplitRec(BlockHeader *b, int _length)
{
    if ((_length + sizeof(BlockHeader)) * 2 <= b->block_size && b->block_size > basic_block_size)
    {
        split(b);
        SplitRec(b, _length);
    }
    return b;
}

bool BuddyAllocator::ValidMemoryLocation(BlockHeader *block)
{
    if ((reinterpret_cast<long>(mem) <= reinterpret_cast<long>(block) && reinterpret_cast<long>(block) < reinterpret_cast<long>(mem) + total_memory_size))
    {
        return true;
    }
    return false;
}