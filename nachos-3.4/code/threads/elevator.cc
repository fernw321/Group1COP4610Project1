#include "copyright.h"
#include "system.h"
#include "elevator.h"


int nextPersonID = 1;
Lock *personIDLock = new Lock("PersonIDLock");

Condition *waiting_cd = new Condition("no one waiting");

ELEVATOR *e;

void ELEVATOR::start() {
    while(1) {

        // A. Wait until hailed
        e->elevatorLock->Acquire();
        
        while(e->numPersonsWaiting < 1 && e->occupancy < 1)
        {
            waiting_cd->Wait(e->elevatorLock);
        }
        //e->elevatorLock->Release();

        //e->elevatorLock->Acquire();
        printf("waiting...\n");
        if(e->waiting)
            printf("shouldnt be waiting...\n");

        while(1){
            //0. Acquire elevatorLock
            e->elevatorLock->Acquire();

            //1. Signal persons inside elevator to get off (leaving->broadcast(elevatorLock))
            if(e->occupancy>0)
                leaving[e->currentFloor-1]->Broadcast(e->elevatorLock);

            //2. Signal persons atFloor to get in, one at a time, checking occupancyLimit each time
            printf("persons waiting: %d\n", e->personsWaiting[e->currentFloor-1]);
            for(int i = 0; i < e->personsWaiting[currentFloor-1]; i++)
            {
                printf("Person waiting to enter...\n");
                if(e->occupancy == e->maxOccupancy)
                    break;

                entering[e->currentFloor-1]->Signal(e->elevatorLock);
            }
            //2.5 Release elevatorLock
            e->elevatorLock->Release();


            //3. Spin for some time
            for(int j =0 ; j< 1000000; j++) {
                    currentThread->Yield();
                }
            //4. Go to next floor
            //need to figure out a decent way to tell elevator where to go next, cant just keep going one way until empty
            //protect all read/write vars
            e->currentFloor = e->currentFloor+1;

            printf("Elevator arrives on floor %d\n", e->currentFloor-1);
           
        }

        
    }
}

void ElevatorThread(int numFloors) {

    printf("Elevator with %d floors was created!\n", numFloors);

    e = new ELEVATOR(numFloors);

    e->start();


}

ELEVATOR::ELEVATOR(int numFloors) {
    currentFloor = 1;
    numFloors = numFloors;
    numPersonsWaiting = 0;
    direction = 1; //up
    entering = new Condition*[numFloors];
    // Initialize entering
    for (int i = 0; i < numFloors; i++) {
        entering[i] = new Condition("Entering");
    }
    personsWaiting = new int[numFloors];
    elevatorLock = new Lock("ElevatorLock");

    // Initialize leaving
    leaving = new Condition*[numFloors];
    for(int i = 0; i < numFloors; i++)
    {
        leaving[i] = new Condition("Leaving");
    }
}


void Elevator(int numFloors) {
    // Create Elevator Thread 
    printf("Elevator function invoked!\n");
    Thread *t = new Thread("Elevator");
    t->Fork(ElevatorThread, numFloors);
}


void ELEVATOR::hailElevator(Person *p) {
    e->elevatorLock->Acquire();
    // 1. Increment waiting persons atFloor
    if(e->currentFloor != p->atFloor)
    {
        e->numPersonsWaiting++;
        e->personsWaiting[e->currentFloor-1] = e->personsWaiting[e->currentFloor-1]+1;
    }
    // 2. Hail Elevator
    printf("stop waiting...\n");

    waiting_cd->Signal(e->elevatorLock);
    //e->waiting = true;
    // 2.5 Acquire elevatorLock;
    
    // 3. Wait for elevator to arrive atFloor [entering[p->atFloor]->wait(elevatorLock)]
    while(e->currentFloor != p->atFloor)
        entering[p->atFloor-1]->Wait(e->elevatorLock);



    printf("Person %d got into the elevator.\n", p->id);
    // 6. Decrement persons waiting atFloor [personsWaiting[atFloor]++]
    if(e->currentFloor == p->atFloor)
    {
        e->personsWaiting[e->currentFloor-1] = e->personsWaiting[e->currentFloor-1]-1;
        e->numPersonsWaiting--;
        e->occupancy = e->occupancy + 1;
    }
    
    // 7. Increment persons inside elevator [occupancy++]
    
    //e->elevatorLock->Release();
    // 8. Wait for elevator to reach toFloor [leaving[p->toFloor]->wait(elevatorLock)]
    //put condition
    while(e->currentFloor != p->toFloor)
        leaving[p->toFloor-1]->Wait(e->elevatorLock);

    //e->elevatorLock->Acquire();

    // 9. Get out of the elevator
    printf("Person %d got out of the elevator.\n", p->id);
    // 10. Decrement persons inside elevator
    if(e->currentFloor == p->toFloor)
    {
        e->occupancy = e->occupancy - 1;
        // 11. Release elevatorLock;
        e->elevatorLock->Release();
    }
    
}

void PersonThread(int person) {

    Person *p = (Person *)person;

    printf("Person %d wants to go from floor %d to %d\n", p->id, p->atFloor, p->toFloor);

    e->hailElevator(p);

}

int getNextPersonID() {
    int personID = nextPersonID;
    personIDLock->Acquire();
    nextPersonID = nextPersonID + 1;
    personIDLock->Release();
    return personID;
}


void ArrivingGoingFromTo(int atFloor, int toFloor) {

	printf("Person wants to go from floor %d to %d\n", atFloor, toFloor);
    // Create Person struct
    Person *p = new Person;
    p->id = getNextPersonID();
    p->atFloor = atFloor;
    p->toFloor = toFloor;

    // Creates Person Thread
    Thread *t = new Thread("Person " + p->id);
    t->Fork(PersonThread, (int)p);

}