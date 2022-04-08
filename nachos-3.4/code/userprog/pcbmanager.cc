#include "pcbmanager.h"
#include "synch.h"
#include "thread.h"

Lock* pcbManagerLock = new Lock ("pcbManagerLock");

PCBManager::PCBManager(int maxProcesses) {

    bitmap = new BitMap(maxProcesses);
    pcbs = new PCB*[maxProcesses];

    for(int i = 0; i < maxProcesses; i++) {
        pcbs[i] = NULL;
    }

}


PCBManager::~PCBManager() {

    delete bitmap;

    delete pcbs;

}


PCB* PCBManager::AllocatePCB() {

    // Aquire pcbManagerLock
    pcbManagerLock->Acquire();

    int pid = bitmap->Find();

    // Release pcbManagerLock
    pcbManagerLock->Release();

    ASSERT(pid != -1);

    pcbs[pid] = new PCB(pid);

    return pcbs[pid];

}


int PCBManager::DeallocatePCB(PCB* pcb) {

    // Check is pcb is valid -- check pcbs for pcb->pid
    //return -1 if pcb is not valid
    if(NULL == pcbManager->GetPCB(pcb->pid))
        return -1;

     // Aquire pcbManagerLock
    pcbManagerLock->Acquire();


    bitmap->Clear(pcb->pid);

    // Release pcbManagerLock
    pcbManagerLock->Release();


    delete pcbs[pcb->pid];

    pcbs[pcb->pid] = NULL;

    return 0;
}

PCB* PCBManager::GetPCB(int pid) {
    return pcbs[pid];
}