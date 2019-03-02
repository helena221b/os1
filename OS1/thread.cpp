#include "thread.h"
#include "system.h"
#include "pcb.h"
#include "schedule.h"
#include "queue.h"
#include "iostream.h"
#include "semaphore.h"

Thread::Thread(StackSize stackSize,Time timeSlice){
	lock();
	myPCB = new PCB(this,stackSize,timeSlice);
	unlock();
}
Thread::~Thread(){
	lock();
	waitToComplete();
	delete myPCB;
	unlock();
}
void Thread::start(){
	lock();
	myPCB->state= PCB::READY;
	myPCB->createStack();
	Scheduler::put(myPCB);
	unlock();
}
void Thread::waitToComplete(){
	lock();
///	cout<<"wait to complete called"<<endl;
	if(this == System::idle){unlock();return;}
	if(this == System::starting){unlock();return;}
	if(myPCB->state == PCB::OVER || myPCB->state == PCB::NEW){unlock();return;}
	if(myPCB == (PCB*)System::running){unlock();return;}

	System::running->state = PCB::BLOCKED;
	myPCB->waitQueue->put((PCB*)System::running);
	unlock();
	System::dispatch();

}
ID Thread::getRunningId(){
	return System::running->id;
}
ID Thread::getId(){
	return myPCB->id;
}
Thread* Thread::getThreadById(ID id){
	return System::getThreadById(id);
}
void dispatch(){
	lock();
	System::dispatch();
	unlock();
}
//void tick();
