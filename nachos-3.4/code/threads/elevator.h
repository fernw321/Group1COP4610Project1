#ifndef ELEVATOR_H
#define ELEVATOR_H

#include "copyright.h"
#include "synch.h"

void Elevator(int numFloors);
void ArrivingGoingFromTo(int atFloor, int toFloor);

typedef struct Person {
    int id;
    int atFloor;
    int toFloor;
} Person;


class ELEVATOR {

public:
    ELEVATOR(int numFloors);
    ~ELEVATOR();
    int GetNumFloors();
    void hailElevator(Person *p);
    void start();

private:
    int currentFloor;
    Condition **entering;
    Condition **leaving;
    int *personsWaiting;
    int occupancy;
    int maxOccupancy;
    Lock *elevatorLock;
    int numFloors;
    bool waiting;

};

#endif