#include "copyright.h"
#include "system.h"
#include "elevator.h"


int nextPersonID = 1;
Lock *personIDLock = new Lock("PersonIDLock");

Condition *noPerson = new Condition("no one waiting");

ELEVATOR *e;

void ELEVATOR::start() {
    // printf("all the snakes...\n");
    // noPerson->Wait(elevatorLock);
// for(int j =0 ; j< 1000000; j++) 
// {
//         currentThread->Yield();
// }
    // printf("are dead!\n");

    while(1) {
        // A. Wait until hailed
        //printf("waiting...\n");

        //check if anyone is waiting
        //printf("floors: %d\n", e->numFloors);
        for(int i = 0; i < e->numFloors; i++)
        { 
            printf("we are here...\n");
            if(e->personsWaiting[i] > 0)
            {
                printf("We got peeps: %d\n", e->personsWaiting[i]);
                e->waiting = true;
                if(e->waiting)
                    printf("waiting\n");
                break;
            }
        }

        //B. While there are active persons, loop doing the following
        while(e->occupancy > 0 || e->waiting){
            // //0. Acquire elevatorLock
            // e->elevatorLock->Acquire();
            printf("We are in...\n");
            // //1. Signal persons inside elevator to get off (leaving->broadcast(elevatorLock))
            // leaving[currentFloor-1]->Broadcast(elevatorLock);

            // //2. Signal persons atFloor to get in, one at a time, checking occupancyLimit each time
            // for(int i = 0; i < personsWaiting[currentFloor-1]; i++)
            // {
            //     if(e->occupancy == maxOccupancy)
            //         break;
            //     //need to establish signals
            //     entering[currentFloor-1]->Signal(e->elevatorLock);
            // }
            // //2.5 Release elevatorLock
            // e->elevatorLock->Release();


            // //3. Spin for some time
            // for(int j =0 ; j< 1000000; j++) 
            // {
            //         currentThread->Yield();
            // }
            //4. Go to next floor
            //need to figure out a decent way to tell elevator where to go next, cant just keep going one way until empty
            e->currentFloor = e->currentFloor+1;

            printf("Elevator arrives on floor %d", e->currentFloor);
           
        }

        //e->elevatorLock->Release();

        
    }
}

void ElevatorThread(int numFloors) {

    printf("Elevator with %d floors was created!\n", numFloors);

    e = new ELEVATOR(numFloors);

    printf("numfloors: %d\n", e->numFloors);
    for(int j =0 ; j< 1000000; j++) 
    {
        currentThread->Yield();
    }

    e->start();


}

ELEVATOR::ELEVATOR(int numFloors) {
    currentFloor = 1;
    numFloors = numFloors;
    entering = new Condition*[numFloors];
    // Initialize entering
    for (int i = 0; i < numFloors; i++) 
    {
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
    // 1. Increment waiting persons atFloor
    e->personsWaiting[e->currentFloor-1] = e->personsWaiting[e->currentFloor-1]+1;
    printf("People waiting: %d\n", e->personsWaiting[e->currentFloor-1]);
    // 2. Hail Elevator
    noPerson->Signal(elevatorLock);
    // 2.5 Acquire elevatorLock;
    //e->elevatorLock->Acquire();
    // 3. Wait for elevator to arrive atFloor [entering[p->atFloor]->wait(elevatorLock)]
    //entering[p->atFloor-1]->Wait(e->elevatorLock);   
    
    //printf("Person %d got into the elevator.\n", p->id);
    // 6. Decrement persons waiting atFloor [personsWaiting[atFloor]++]
    //e->personsWaiting[currentFloor-1] = e->personsWaiting[currentFloor-1]-1;
    // 7. Increment persons inside elevator [occupancy++]
    //e->occupancy = e->occupancy + 1;
    // 8. Wait for elevator to reach toFloor [leaving[p->toFloor]->wait(elevatorLock)]
    //leaving[p->toFloor-1]->Wait(e->elevatorLock);

    // 9. Get out of the elevator
    //printf("Person %d got out of the elevator.\n", p->id);
    // 10. Decrement persons inside elevator
    //e->occupancy = e->occupancy - 1;
    // 11. Release elevatorLock;
    //e->elevatorLock->Release();
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