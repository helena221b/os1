#include "pcb.h"
#include <dos.h>
#include "thread.h"
#include "system.h"
#include "queue.h"
#include <stdlib.h>
#include "iostream.h"
ID PCB::identifier=0;

const int PCB::NEW=0;
const int PCB::READY=1;
const int PCB::BLOCKED=2;
const int PCB::OVER=3;

PCB::PCB(Thread* myThr,StackSize stackSize,Time timeSlice){
	lock();
	this->myThread = myThr;
	this->pcbStackSize = stackSize;
	this->pcbTimeSlice = timeSlice;
	this->state = NEW;
	this->pcbSP = 0;
	this->passedTimeCounter=0;
	this->id=identifier++;
	this->waitQueue = new Queue();
	this->signaled=1;
	System::ListOfThreads.put(this);
	unlock();
}
PCB::~PCB(){
	lock();
	delete pcbSP;
	delete  waitQueue;
	unlock();
}
void PCB::createStack(){//ja sama cacno ovde
	/*lock();
	static int n = this->pcbStackSize/sizeof(unsigned);

	this->pcbSP= new unsigned int[n]; //alociraj prostor

	this->pcbSP[n-1]=0x200; // dole dole postavi psw

	this->pcbSP[n-2]=FP_SEG(&(System::wrapper)); // pa postavi segmentni deo
	this->pcbSP[n-3]=FP_OFF(&(System::wrapper));//pa postavi offsenti deo

	this->sp=FP_OFF(this->pcbSP+n-12); // postavi sp na -12 zato sto sam savio ukupno 12 stvari na stek.
	this->ss=FP_SEG(this->pcbSP+n-12);
	this->bp=FP_OFF(this->pcbSP+n-12);
	unlock();*/
	lock();
	static int n = this->pcbStackSize/sizeof(unsigned);
	static unsigned int newSS,newSP,newPC,newCS,k=0,oldSS,oldSP;
	this->pcbSP= new unsigned int[n];
	this->sp=FP_OFF(this->pcbSP+n-1);
	this->ss=FP_SEG(this->pcbSP+n-1);
	newSP=this->sp;
	newSS=this->ss;
	newPC=FP_OFF(System::wrapper);
	newCS=FP_SEG(System::wrapper);
	asm{
		 mov oldSS , ss
		 mov oldSP , sp

		 mov ss , newSS
		 mov sp , newSP

		 pushf
		 pop ax
		 or ax, 1000000000b
		 push ax

		 mov ax, newCS
		 push ax

		 mov ax, newPC
		 push ax

		 push	ax
		 push	bx
		 push	cx
		 push	dx
		 push	es
		 push	ds

		 push	si
		 push	di
		 push	k
		 mov newSP, sp
		 mov newSS, ss
		 mov sp, oldSP
	     mov ss, oldSS
	}
	this->sp = newSP;
	this->ss =  newSS;
	unlock();
}
