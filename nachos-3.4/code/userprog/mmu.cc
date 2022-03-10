#include "mmu.h"
#include "machine.h"
#include "system.h"

MemoryManager::MemoryManager() {
	bitmap = new BitMap(NumPhysPages);
}

MemoryManager::~MemoryManager() {
	delete bitmap;
}

int MemoryManager::AllocatePage() {
	return bitmap->Find();
}

int MemoryManager::DealloatePage(int which) {
	if (bitMap->Test(which) == false) return -1;
	else {
		bitMap->Clear(which);
		return 0;
	}
}

unsigned int MemoryManager::GetFreePageCount() {
	return bitmap->NumClear();
}
