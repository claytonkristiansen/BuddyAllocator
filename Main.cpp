#include "Ackerman.h"
#include "BuddyAllocator.h"
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <iostream>

void easytest(BuddyAllocator *ba)
{
    // be creative here
    // know what to expect after every allocation/deallocation cycle

    //int TEST = sizeof(BlockHeader);

    // here are a few examples
    ba->printlist();
    // allocating a byte
    char *mem = ba->alloc(1);

    //memset(mem, 30, 45 * sizeof(char));

    // now print again, how should the list look now
    ba->printlist();

    ba->free(mem);   // give back the memory you just allocated
    ba->printlist(); // shouldn't the list now look like as in the beginning
}

int main(int argc, char **argv)
{
    int basic_block_size = 128, memory_length = 128 * 1024 * 1024;

    char option;
    while((option = getopt(argc, argv, ":b:s:")) != -1)
    {
        switch(option)
        {
            case 'b':
                basic_block_size = atoi(optarg);
                break;
            case 's':
                memory_length = atoi(optarg);
                break;
        }
    }

    // create memory manager
    BuddyAllocator *allocator = new BuddyAllocator(basic_block_size, memory_length);

    // the following won't print anything until you start using FreeList and replace the "new" with your own implementation
    easytest(allocator);

    // stress-test the memory manager, do this only after you are done with small test cases
    Ackerman *am = new Ackerman();
    am->test(allocator); // this is the full-fledged test.

    // destroy memory manager
    delete allocator;
}
