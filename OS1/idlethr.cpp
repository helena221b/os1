#include "idlethr.h"
#include "system.h"
#include "pcb.h"
#include "iostream.h"
#define minTimeSlice 1
#define minStackSize 256

IdleThread::IdleThread():Thread(minStackSize,minTimeSlice){}

void IdleThread::run() {
	while (1);
}
void IdleThread::start(){
	lock();
	myPCB->state=PCB::READY;
	myPCB->createStack();
	unlock();
}
