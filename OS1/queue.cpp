#include "queue.h"
#include "system.h"
#include <stdlib.h>
#include "iostream.h"
#include "pcb.h"

#define Next prev

Elem::Elem(PCB* p){
	lock();
	this->pcb=p;
	unlock();
}
semElem::semElem(KernelSem* sem){
	this->semaphore=sem;
}
Queue::Queue(){
	lock();
	this->first=NULL;
	this->last=NULL;
	unlock();
}
Queue::~Queue(){
	lock();
	Elem* temp;
	while(first!=NULL){
		temp=first;
		first=first->next;
		delete temp;
	}
	unlock();
}
void Queue::put(PCB* p){
	lock();
	Elem *temp;
	temp=new Elem(p);
	temp->next=NULL;
	if(last!=NULL)
		last->next=temp;
	else
		first=temp;
	last=temp;
	unlock();
}
PCB* Queue::get(){
	lock();
	if(first!=NULL){
		Elem* temp;
		temp=first;
		first=first->next;
		if(first==NULL)
			last=first=NULL;

		PCB *tpcb;
		tpcb=temp->pcb;
		delete	temp;
		unlock();
		return tpcb;
	}
	else
	{
		unlock();
		return NULL;
	}
}
PCB* Queue::remove(int id){
	Elem *temp=first;
	prev=NULL;
	while(temp!=NULL){
		if(temp->pcb->id==id)
			break;
		prev=temp;
		temp=temp->next;
	}
	if(temp!=NULL){
		if(prev!=NULL)
			prev->next=temp->next;
		else
			first=temp->next;
		if(temp==last)
			last=prev;
		Next=temp->next;//da bi u nadjacanu mogo da  dodam na temp+ ono sto skino.
		PCB *tpcb=temp->pcb;
		delete	temp;
		return tpcb;
	}
	return NULL;
}
void pQueue::put(PCB* Pcb){
	Elem* temp=first,*newElem;
	int time=0;
	prev=NULL;
	while(temp!=NULL){
		time+=temp->pcb->semaphoreTime;
		if(Pcb->semaphoreTime<time)
			break;
		prev=temp;
		temp=temp->next;
	}
	Pcb->semaphoreTime-=time;
	newElem=new Elem(Pcb);
	newElem->next=temp;
	if(temp!=NULL)
		temp->pcb->semaphoreTime-=Pcb->semaphoreTime;
	if(prev!=NULL)
		prev->next=newElem;
	else
		first=newElem;
	if(temp==NULL)
		last=newElem;//saviti lockove na ove metode?/
}
PCB* pQueue::get(){
	if(first!=NULL){
		if(first->pcb->semaphoreTime==0)
			return Queue::get();
	}
	return NULL;
}
PCB* pQueue::remove(int id){
	PCB* p = Queue::remove(id);
	if(p!=NULL){
		if(Next!=NULL)//ovo ovde nije prev nego next promeni ime u klasi da bude razumevli
			Next->pcb->semaphoreTime+=p->semaphoreTime;//negde se koristi kao prev negde kao next;
		return p;
	}
	else
		return  NULL;
}

semQueue::semQueue(){
	lock();
	this->first=NULL;
	this->last=NULL;
	unlock();
}
semQueue::~semQueue(){
	semElem* temp;
	while(first!=NULL){
		temp=first;
		first=first->next;
		delete temp;
	}
}

void semQueue::put(KernelSem* k){
	lock();
	semElem *temp=new semElem(k);
	temp->next=NULL;
	if(last!=NULL)
		last->next=temp;
	else
		first=temp;
	last=temp;
	unlock();
}
KernelSem* semQueue::get(){
	lock();
	if(first!=NULL){
		semElem* temp=first;
		first=first->next;
		if(first==NULL)
			last=first=NULL;

		KernelSem *k=temp->semaphore;
		delete temp;
		unlock();
		return k;
	}
	else
	{
		unlock();
		return NULL;
	}
}
