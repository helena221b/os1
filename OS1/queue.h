#ifndef QUEUE_H_
#define QUEUE_H_

class PCB;
class KernelSem;

class Elem{
public:
	Elem(PCB* p);
	PCB* pcb;
	Elem* next;
};
class semElem{
public:
	semElem(KernelSem* sem);
	KernelSem* semaphore;
	semElem* next;
};
class Queue{
public:
	Queue();
	~Queue();

	virtual void  put(PCB* Pcb);
	virtual PCB* get();
	virtual PCB* remove(int id);
protected:
	friend class System;
	 Elem *first,*last,*prev;
};

class pQueue : public Queue{
public:
	void put(PCB* Pcb);
	PCB* get();
	PCB* remove(int id);
};

class semQueue{
public:
	semQueue();
	~semQueue();

	void  put(KernelSem* Pcb);
	KernelSem* get();
protected:
	friend class System;
	 semElem *first,*last;
};
#endif
