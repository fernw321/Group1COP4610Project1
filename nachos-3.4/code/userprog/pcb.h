#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "bitmap.h"
#include "list.h"

class Thread;

class PCB {
    public:
        PCB(int id);
        ~PCB();
        int pid;
        PCB* parent;
        List* children;
        Thread* thread;

    private:
        void AddChild(PCB* pcb);
        int RemoveChild(PCB* pcb);
};

#endif // PCB_H