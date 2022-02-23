// threadtest.cc 
//	Simple test case for the threads assignment.
//
//	Create two threads, and have them context switch
//	back and forth between themselves by calling Thread::Yield, 
//	to illustratethe inner workings of the thread system.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"

#if defined(CHANGED)
#include "synch.h"
int numThreadsActive;
#endif

// testnum is set in main.cc
int testnum = 1;

//----------------------------------------------------------------------
// SimpleThread
// 	Loop 5 times, yielding the CPU to another ready thread 
//	each iteration.
//
//	"which" is simply a number identifying the thread, for debugging
//	purposes.
//----------------------------------------------------------------------

#if defined(CHANGED) && defined(HW1_SEMAPHORES)

// EXERCISE 1: SimpleThread implementation with Semaphores

int SharedVariable;

// Initialize the barrier semaphore with a value of 0
Semaphore Barrier("BARRIER",0);

//Initialize the sempahore responsible for protecting the barrierCount variable
Semaphore CountMutex("COUNTMUTEX",1);

// Initialize the semaphore that will protect the increment to SharedVariable. We set the value
// to 1 because we don't want the thread to go immediately to sleep
Semaphore Semaphore("SEMAPHORE",1);

// The count that makes sure all threads output the same value
int barrierCount = 0;


void
SimpleThread(int which)
{
    printf("ENTER SIMPLETHREAD SEMAPHORES\n");
    int num, val;
    for(num = 0; num <5; num++) {

        // ********* Entry section ***********

        Semaphore.P(); // Semaphore calls wait 

        // ********* Critical Section **********

        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val+1;

        // ********** Exit Section **********

        Semaphore.V(); // Semaphore signals

        currentThread->Yield();
    }

    
    // Keeps the count mutually exclusive and safe from being corrupted
    CountMutex.P();
    barrierCount++;
    CountMutex.V();

    // Checks to see if the barrierCount variable is equal to the number of threads
    // inputted, then print their final values
    // numThreadsActive is first set when calling the ThreadTest() function in main.cc
    if(barrierCount == numThreadsActive) {
        Barrier.V();
    }

    Barrier.P();
    Barrier.V();

    // Sets the local val variable to the final value
    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);

}

#elif defined(CHANGED) && defined(HW1_LOCKS)

// EXERCISE 2: SimpleThread Implementation with Locks

int SharedVariable;

// Create lock

Lock LOCK("LOCK");

// Semaphores needed for locks implementation

// Initialize the barrier semaphore with a value of 0
Semaphore Barrier("BARRIER",0);

//Initialize the sempahore responsible for protecting the barrierCount variable
Semaphore CountMutex("COUNTMUTEX",1);

// The count that makes sure all threads output the same value
int barrierCount = 0;

void
SimpleThread(int which)
{
    printf("ENTER SIMPLETHREAD LOCKS\n");
    int num, val;
    for(num = 0; num <5; num++) {

        // ********* Entry section ***********

        LOCK.Acquire(); // Thread acquires lock

        // ********* Critical Section **********

        val = SharedVariable;
        printf("*** thread %d sees value %d\n", which, val);
        currentThread->Yield();
        SharedVariable = val+1;

        // ********** Exit Section **********

        LOCK.Release(); // Thread releases lock

        currentThread->Yield();
    }

    // We maintain Semaphores here as in HW1_SEMAPHORES
    // Keeps the count mutually exclusive and safe from being corrupted
    CountMutex.P();
    barrierCount++;
    CountMutex.V();

    // Checks to see if the barrierCount variable is equal to the number of threads
    // inputted, then print their final values
    // numThreadsActive is first set when calling the ThreadTest() function in main.cc
    if(barrierCount == numThreadsActive) {
        Barrier.V();
    }

    Barrier.P();
    Barrier.V();

    // Sets the local val variable to the final value
    val = SharedVariable;
    printf("Thread %d sees final value %d\n", which, val);

}


#endif

#if defined(CHANGED) 

void
ThreadTest(int n) {
    DEBUG('t', "Entering SimpleTest");
    Thread *t;
    numThreadsActive = n;
    printf("NumthreadsActive = %d\n", numThreadsActive);

    for(int i=1; i<n; i++)
    {
        t = new Thread("forked thread");
        t->Fork(SimpleThread,i);
    }
    SimpleThread(0);
}

#else // end block for CHANGED, HW1_SEMAPHORES, and HW1_LOCKS

void
SimpleThread(int which)
{
    int num;
    
    for (num = 0; num < 5; num++) {
	printf("*** thread %d looped %d times\n", which, num);
        currentThread->Yield();
    }
}

//----------------------------------------------------------------------
// ThreadTest1
// 	Set up a ping-pong between two threads, by forking a thread 
//	to call SimpleThread, and then calling SimpleThread ourselves.
//----------------------------------------------------------------------

void
ThreadTest1()
{
    DEBUG('t', "Entering ThreadTest1");

    Thread *t = new Thread("forked thread");

    t->Fork(SimpleThread, 1);
    SimpleThread(0);
}

//----------------------------------------------------------------------
// ThreadTest
// 	Invoke a test routine.
//----------------------------------------------------------------------

void
ThreadTest()
{
    switch (testnum) {
    case 1:
	ThreadTest1();
	break;
    default:
	printf("No test specified.\n");
	break;
    }
}

#endif // end for no preprocessor variables

