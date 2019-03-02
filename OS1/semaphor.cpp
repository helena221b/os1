#include "semaphor.h"
#include "krnlsem.h"
#include "system.h"
#include "schedule.h"
#include "pcb.h"
#include "iostream.h"
#define UnlimitedTime 0

Semaphore::Semaphore(int init){
	lock();
	myImpl = new KernelSem(init);
	unlock();
}

Semaphore::~Semaphore(){
	lock();
	delete myImpl;
	unlock();
}
int Semaphore::wait(Time maxTimeToWait){
	lock();
	unsigned int t = myImpl->wait(maxTimeToWait);
	unlock();
	return t;
}

void Semaphore::signal(){
	lock();
	myImpl->signal();
	unlock();
}

int Semaphore::val() const{
	return myImpl->val();
}

KernelSem::KernelSem(int init){
	value=init;
	System::ListOfSemaphores.put(this);
}

int KernelSem::wait(Time maxTimeToWait){

	if(--value<0){
		System::running->state=PCB::BLOCKED;
		if(maxTimeToWait!=UnlimitedTime){
			System::running->semaphoreTime=maxTimeToWait;
			time_limited_th_q.put((PCB*)System::running);
		}
			all_waited_th_q.put((PCB*)System::running);
		System::dispatch();
		if(System::running->signaled)
			return 1;
		else
			return 0;
	}
	return 1;
}

void KernelSem::signal(){
	if(value++<0){
		PCB* temp = all_waited_th_q.get();
		if(temp!=NULL){
			time_limited_th_q.remove(temp->id);
			temp->signaled=1;
			temp->state= PCB::READY;
			Scheduler::put(temp);
		}
	}
}

int KernelSem::val() const{
	return value;
}
