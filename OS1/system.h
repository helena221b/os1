#ifndef SYSTEM_H_
#define SYSTEM_H_

#include "thread.h"
#include "idlethr.h"

#define lock(){asm{pushf;cli;}}

#define unlock(){asm{popf;}}

//typedef void  (interrupt *InterruptRoutine)();

class Queue;
class semQueue;
class System{
public:
	static void boot();
	static void unboot();
	static  void dispatch();
	static int counter;
private:
	static Thread* starting;
	static IdleThread* idle;

	static Queue ListOfThreads;
	static semQueue ListOfSemaphores;

	static volatile char wantToDispatch;
	static volatile unsigned int lockFlag;
	static volatile PCB* running;

	friend class Thread;
	friend class PCB;
	friend class KernelSem;
	friend class KernelEv;

	static Thread* getThreadById(ID id);
	static void semaphores_list_time();
	static void wrapper();
	static void interrupt(*previousRoutine)(...);
	static void interrupt timerInterrupt(...);
	static void routineReplacement();
};

#endif


