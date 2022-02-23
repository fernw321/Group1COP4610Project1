// synch.cc 
//	Routines for synchronizing threads.  Three kinds of
//	synchronization routines are defined here: semaphores, locks 
//   	and condition variables (the implementation of the last two
//	are left to the reader).
//
// Any implementation of a synchronization routine needs some
// primitive atomic operation.  We assume Nachos is running on
// a uniprocessor, and thus atomicity can be provided by
// turning off interrupts.  While interrupts are disabled, no
// context switch can occur, and thus the current thread is guaranteed
// to hold the CPU throughout, until interrupts are reenabled.
//
// Because some of these routines might be called with interrupts
// already disabled (Semaphore::V for one), instead of turning
// on interrupts at the end of the atomic operation, we always simply
// re-set the interrupt state back to its original value (whether
// that be disabled or enabled).
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "synch.h"
#include "system.h"

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	Initialize a semaphore, so that it can be used for synchronization.
//
//	"debugName" is an arbitrary name, useful for debugging.
//	"initialValue" is the initial value of the semaphore.
//----------------------------------------------------------------------

Semaphore::Semaphore(const char* debugName, int initialValue)
{
    name = debugName;
    value = initialValue;
    queue = new List;
}

//----------------------------------------------------------------------
// Semaphore::Semaphore
// 	De-allocate semaphore, when no longer needed.  Assume no one
//	is still waiting on the semaphore!
//----------------------------------------------------------------------

Semaphore::~Semaphore()
{
    delete queue;
}

//----------------------------------------------------------------------
// Semaphore::P
// 	Wait until semaphore value > 0, then decrement.  Checking the
//	value and decrementing must be done atomically, so we
//	need to disable interrupts before checking the value.
//
//	Note that Thread::Sleep assumes that interrupts are disabled
//	when it is called.
//----------------------------------------------------------------------

void
Semaphore::P()
{
    IntStatus oldLevel = interrupt->SetLevel(IntOff);	// disable interrupts
    
    while (value == 0) { 			// semaphore not available
	queue->Append((void *)currentThread);	// so go to sleep
	currentThread->Sleep();
    } 
    value--; 					// semaphore available, 
						// consume its value
    
    (void) interrupt->SetLevel(oldLevel);	// re-enable interrupts
}

//----------------------------------------------------------------------
// Semaphore::V
// 	Increment semaphore value, waking up a waiter if necessary.
//	As with P(), this operation must be atomic, so we need to disable
//	interrupts.  Scheduler::ReadyToRun() assumes that threads
//	are disabled when it is called.
//----------------------------------------------------------------------

void
Semaphore::V()
{
    Thread *thread;
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    thread = (Thread *)queue->Remove();
    if (thread != NULL)	   // make thread ready, consuming the V immediately
	scheduler->ReadyToRun(thread);
    value++;
    (void) interrupt->SetLevel(oldLevel);
}

#if defined(CHANGED)

// New initialization code for the Lock class. Switched debugname to be char* from const char*

Lock::Lock(char* debugName) {
    name = debugName;
    threadQueue = new List;
    lockIsFree = true;
    threadHoldingLock = NULL;
}

// Deconstructor

Lock::~Lock() {

    delete threadQueue; // delete the data structure

}

bool Lock::isHeldByCurrentThread() {
    bool result;
    // Is the current thread using the CPU the same as the thread holding the lock?
    result = (currentThread == threadHoldingLock);
    return result;
}

// New acquire method for locks

void Lock::Acquire() {


    // Disable interrupts (same as Semaphores)
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // While the lock is not free, queue all consecutive threads that try to access that lock
    // and put them to sleep
    while (!lockIsFree) {
        threadQueue->Append((void *)currentThread);
        currentThread->Sleep();
    }
    // Once the lock is free, the thread will exit the while loop above. Then, the thread
    // will acquire the lock and set the lock as not free.
    threadHoldingLock = currentThread;
    lockIsFree = FALSE;

    // Activate interrupts again (same as Semaphores)
    (void) interrupt->SetLevel(oldLevel);


}

// New release method for locks

void Lock::Release() {
    // This is the next thread that will run after the previous thread releases the lock
    Thread *nextThread;

    // Disable interrupts (same as Semaphores)
    IntStatus oldLevel = interrupt->SetLevel(IntOff);

    // If the current thread is not holding the lock
    if (!isHeldByCurrentThread()) {
        // ***** Do nothing *****
    } else { // Else, if the current thread IS holding the lock and wants to release it

        // The next thread to be run is taken from the queue
        nextThread = (Thread *)threadQueue->Remove();
        // If the new thread is not NULL, schedule this thread to run
        if (nextThread != NULL)
            scheduler->ReadyToRun(nextThread);
        // If the thread is NULL, declare the lock free and nullify the placeholder
        lockIsFree = TRUE;
        threadHoldingLock = NULL;
    }

}

Condition::Condition(char* debugName) {
  name = debugName;
  blockedThreads = new List;
}

Condition::~Condition() {
  delete blockedThreads;
}

void Condition::Wait(Lock* conditionLock) 
{ 
  // Disable interrupts (same as for Semaphores or Locks)
  IntStatus oldLevel = interrupt->SetLevel(IntOff);

  ASSERT(conditionLock->isHeldByCurrentThread());
  
  if(!conditionLock->isHeldByCurrentThread()) {
        // ***** Do nothing *****
    } else {
      // Release the conditional lock if the lock is being held by the same thread
      conditionLock->Release();  
      // Add the current thread to the list of waiting threads
      blockedThreads->Append((void *)currentThread);  
      // Put the current thread to sleep  
      currentThread->Sleep();  
      // Acquire the conditional lock               
      conditionLock->Acquire();   
    }
    // Activate interuppts again (same as Semaphores and Locks)
  (void) interrupt->SetLevel(oldLevel);
  
}

//----------------------------------------------------------------------
// Condition::Signal
//   This function wakes up one of the threads that is waiting on
//   the condition.
//----------------------------------------------------------------------

void Condition::Signal(Lock* conditionLock) 
{
  // Create a new thread
  Thread *newThread;
  //Disable interrupts (same as Semaphores and Locks)
  IntStatus oldLevel = interrupt->SetLevel(IntOff);  // disable interrupts

  ASSERT(conditionLock->isHeldByCurrentThread());

  //If the conditional locks is not held by the current thread, do nothing
  if(!conditionLock->isHeldByCurrentThread())
    {
      // ***** Do nothing *****
      
    } else {
      // Remove one of the blocked threads from the list 
      newThread = (Thread *)blockedThreads->Remove();
      // If there is actually a thread that is not null
      if(newThread != NULL) {
        scheduler->ReadyToRun(newThread);
      }      
    }
    // Disable interrupts (Same as semaphores and Locks)
  (void) interrupt->SetLevel(oldLevel);                 
         
}

//----------------------------------------------------------------------
// Condition::Broadcast
//   This function wakes up all threads that are waiting on
//   the condition.
//----------------------------------------------------------------------
void Condition::Broadcast(Lock* conditionLock) 
{
  // Create a new thread
  Thread *newThread; 
  IntStatus oldLevel = interrupt->SetLevel(IntOff);

  ASSERT(conditionLock->isHeldByCurrentThread());

  // If the conditional lock is not being held by the current thread, do nothing
  if(conditionLock->isHeldByCurrentThread())
    {
      // While the list is not empty, remove the threads and make them run
      while(!blockedThreads->IsEmpty()) { 
        newThread = (Thread *)blockedThreads->Remove();
        scheduler->ReadyToRun(newThread);
      }
    }

  // Renable interrupts (same as Semaphores and Locks)
  (void) interrupt->SetLevel(oldLevel);                 
}






#else // end for defined CHANGED and HW1_LOCKS

// Dummy functions -- so we can compile our later assignments 
// Note -- without a correct implementation of Condition::Wait(), 
// the test case in the network assignment won't work!
Lock::Lock(const char* debugName) {}
Lock::~Lock() {}
void Lock::Acquire() {}
void Lock::Release() {}

Condition::Condition(const char* debugName) { }
Condition::~Condition() { }
void Condition::Wait(Lock* conditionLock) { ASSERT(FALSE); }
void Condition::Signal(Lock* conditionLock) { }
void Condition::Broadcast(Lock* conditionLock) { }

#endif
