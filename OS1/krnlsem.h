#ifndef KRNLSEM_H_
#define KRNLSEM_H_

#include "queue.h"

class KernelSem{
public:
	KernelSem(int init =1);

	int wait (Time maxTimeToWait);
	void signal();
	int val () const;
private:
	friend class System;

	int value;
	Queue all_waited_th_q;
	pQueue time_limited_th_q;

};
#endif
