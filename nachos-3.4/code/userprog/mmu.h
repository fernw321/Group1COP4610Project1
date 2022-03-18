#ifndef MEMORY_H
#define MEMORY_H

#include "synch.h"
#include "bitmap.h"
class MemoryManager {
    public:
        MemoryManager ();
        ~MemoryManager ();
        int AllocatePage();
        int DealloatePage(int which);
        unsigned int GetFreePageCount();


    private:
        BitMap *bitmap;
};

#endif // MEMORY_H
