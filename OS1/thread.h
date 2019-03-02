#ifndef THREAD_H_
#define THREAD_H_

class PCB;

typedef unsigned long StackSize;
const StackSize defaultStackSize =4096;//4096;
typedef unsigned int Time;//time x 55ms
const Time defaultTimeSlice = 2;
typedef int ID;

class Thread{
public:

	void start();
	void waitToComplete();
	virtual ~Thread();

	ID getId();
	static ID getRunningId();
	static Thread * getThreadById(ID id);//treba napraviti niz threadova..

protected:
	friend class PCB;
	friend class System;
	friend class IdleThread;

	Thread(StackSize stackSize = defaultStackSize,Time timeSlice = defaultTimeSlice);
	virtual void run(){};

private: //Da zamenim na private!
	PCB* myPCB;
};

void dispatch();
#endif
