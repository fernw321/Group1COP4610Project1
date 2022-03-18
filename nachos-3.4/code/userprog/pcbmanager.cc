#include "pcbmanager.h"

PCBManager::PCBManager(int maxProcesses) {

    bitmap = new Bitmap(maxProcesses);
    pcbs = new PCB*[maxProcesses];
}


PCBManager::~PCBManager() {

    delete bitmap;
    delete pcbs;
}


PCB* PCBManager::AllocatePCB() {

    // Acquire pcbManagerLock

    int pid = bitmap.Find();

    // Release pcbManagerLock

    ASSERT(pid != -1);

    pcbs[pid] = new PCB(pid);

}


int PCBManager::DeallocatePCB(PCB* pcb) {

    // Check if pcb is valid -- check pcbs for pcb->pid
    
    // Acquire pcbManagerLock

    bitmap.Clear(pcb->pid);

    // Release pcbManagerLock
    
    delete pcbs[pcb->pid];

}