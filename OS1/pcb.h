#ifndef PCB_H_
#define PCB_H_

#include "thread.h"

class Thread;
class Queue;

class PCB{
public:
	PCB(Thread * myThr, StackSize stackSize,Time timeSlice);
	~PCB();

	Thread* myThread;
	static ID identifier;
	ID id;

	unsigned int sp,ss,bp;
	unsigned int signaled;

	StackSize pcbStackSize;
	unsigned * pcbSP;
	void createStack();

	static const int NEW, READY, BLOCKED, OVER;
	volatile int state;

	Time passedTimeCounter;
	Time pcbTimeSlice;
	Time semaphoreTime;

	Queue* waitQueue;
};
#endif
