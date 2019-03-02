#include <stdlib.h>
#include "system.h"
#include <dos.h>
#include "pcb.h"
#include "queue.h"
#include "scheduler.h"
#include "iostream.h"
#include "krnlsem.h"
#include "semaphore.h"
#define startingThreadStackSize 0x1F000
#define startingThreadTimeSlice 1

#define timeIncrement 1
#define timeDecrement 1


volatile char System::wantToDispatch=0;
volatile unsigned int System::lockFlag=1;
void interrupt (*System::previousRoutine)(...)=0;
volatile PCB* System::running = 0;
Thread* System::starting = 0; //0 crveno al ok je..
IdleThread* System::idle = 0;
int System::counter=0;
Queue System::ListOfThreads;
semQueue System::ListOfSemaphores;

//void tick(){
//}
void System::boot(){
	lock();
	routineReplacement();
	starting = new Thread(startingThreadStackSize,startingThreadTimeSlice);
	starting->myPCB->state = PCB::READY;
	running = starting->myPCB;
	idle = new IdleThread();
	idle->start();
	unlock();
}
void System::unboot(){
	lock();
	if(running == starting->myPCB){
		setvect(0x08, previousRoutine);
		lock();
		cout<<"mainFinished"<<counter<<endl;
		unlock();
		delete idle;
		delete starting;
		unlock();
	}
	else{
		unlock();
		return;
	}

}
void System::wrapper(){
	static PCB *temp;
	running->myThread->run();
	lock();
	running->state= PCB::OVER;
	while((temp=running->waitQueue->get())!=NULL){
		//cout<<"unblock";
		temp->state=PCB::READY;
		Scheduler::put(temp);
	}
	unlock();
	dispatch();
}
void interrupt System::timerInterrupt(...){
	static volatile unsigned int tsp,tss,tbp; // koristim za promenu kontexta
	static volatile PCB *newThread; // da bi dohvatio novu nit

	/*if(!wantToDispatch){ //kad se ne pozove explicitno onda se pozovu ove dole
			tick(); 	//rekli su da se poziva
			(*previousRoutine)(); // // stara prekidna rutina da se pozove
			cout<<"PREKID"<<endl;
		}

	if(lockFlag){ //zakljucavanje preko flaga
		if(!wantToDispatch){//doslo preko timera znaci
			if(running->pcbTimeSlice!= 0)//ako nije beskonacna nit
				running->passedTimeCounter+=timeIncrement;// da povecam za time Increment
			//semaphores_list_time();
			if(running->passedTimeCounter<running->pcbTimeSlice||running->pcbTimeSlice==0)//i ako nije proslo vreme vrati se
				return;
		}

		if(running->state==PCB::READY && running!=idle->myPCB){//savi nit u scheduleru
			if(!wantToDispatch)
				running->passedTimeCounter=0;  //resetuj timer te niti na 0
			Scheduler::put((PCB*)running); // savio nit
		}
		wantToDispatch=0;
		//ovde savi za semafore
		newThread= Scheduler::get(); // uzmi novu iz schedulera
		if(newThread==0) //ako nije imo nista da uzme savi idle da radi
			newThread=idle->myPCB;
			asm{       //sacuvaj sp ss i bp
			mov tsp, sp
			mov tss, ss
			//mov tbp, bp
		}
			           //postavi sacuvano na running da zna dokle je stiglo
		running->sp=tsp;
		running->ss=tss;
		//running->bp=tbp;

		running = newThread; // uzmi novu i postavi kao running

		tsp=running->sp;  //sacuvaj da bi moglo da se postavi posle
		tss=running->ss;
		tbp=running->bp;

		asm{            //postavi u registre nove vrednosti
			mov sp,tsp
			mov ss,tss
			//mov bp,tbp
		}

	}*/

	if(!wantToDispatch){
		if(running->pcbTimeSlice==0)
			return;
		running->passedTimeCounter++;
		counter++;
		System::semaphores_list_time();
	}
	if(running->passedTimeCounter==running->pcbTimeSlice || wantToDispatch){
		asm{
			mov tsp, sp
			mov tss, ss
		}
		if(!wantToDispatch)
			running->passedTimeCounter=0;
		running->sp=tsp;
		running->ss=tss;

		if(running->state==PCB::READY && running!=idle->myPCB)
			Scheduler::put((PCB *)running);
		newThread=Scheduler::get();
		if(newThread==NULL)
			running=idle->myPCB;
		else
			running=newThread;

		tsp = running->sp;
		tss = running->ss;

		asm {
			mov sp, tsp
			mov ss, tss
		}
	}
	if(!wantToDispatch){
	//	tick();
		(*previousRoutine)();
	}
	wantToDispatch=0;
}
void  System::dispatch(){
	lock();
	wantToDispatch=1;
//	cout<<"DISPATCH "<<endl;
	timerInterrupt();
	wantToDispatch=0;
	unlock();
}
void System::routineReplacement(){
	lock();
	previousRoutine = getvect(0x08);
	setvect(0x08, timerInterrupt);
	unlock();
}

Thread* System::getThreadById(ID identity){
	volatile Elem* temp =ListOfThreads.first;
	while(temp!=NULL){
		if(temp->pcb->id==identity)
			break;
		temp=temp->next;
	}
	if(temp!=NULL)
		return temp->pcb->myThread;
	else
		return NULL;
}

void System::semaphores_list_time(){
	static semElem* tempSem;
	static Elem* tempThread;
	static PCB* tempPcb;
	tempSem = ListOfSemaphores.first; //za listu semafora
	while(tempSem!=NULL){  //ide kroz listu semafora
		tempThread = tempSem->semaphore->time_limited_th_q.first; //za jedan semafor niti
		if(tempThread!=NULL){ //ako ima blokiranu nit
			tempThread->pcb->semaphoreTime-=timeDecrement;//smanji joj vreme
			while(tempThread!=NULL && tempThread->pcb->semaphoreTime==0){//ako je na 0

				tempPcb= tempSem->semaphore->time_limited_th_q.get();//skini je i menja first na sled
				tempSem->semaphore->all_waited_th_q.remove(tempPcb->id);
				tempPcb->signaled=0;//savi da je vreme isteklo pa zato skinuta
				tempPcb->state= PCB::READY;
				Scheduler::put(tempPcb);//savi u skedjuler
				tempThread = tempSem->semaphore->time_limited_th_q.first;//pogledaj susednu
			}//dal je 0 ili ne i ponovi ako jeste
		}
		tempSem=tempSem->next;//prodji na sledeci semafor.
	}
}
