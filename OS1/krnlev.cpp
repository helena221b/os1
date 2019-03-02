#include "krnlev.h"
#include "IVTEntry.h"
#include "system.h"
#include "thread.h"
#include "pcb.h"
#include "krnlsem.h"

KernelEv::KernelEv (IVTNo ivtNo) {
	if(IVTEntry::IVTTable[ivtNo]) {
	this->ownerThread=System::running->myThread;
	this->ivtNo=ivtNo;
	this->mySem = new KernelSem(0);//POSTAVIO KERNEL SEM
	IVTEntry::IVTTable[ivtNo]->event=this;
	}
}
KernelEv::~KernelEv () {
	delete mySem;
}
void KernelEv::wait () {
	if (this->ownerThread==System::running->myThread)
		mySem->wait(0);

}

void KernelEv::signal() {
	if (mySem->val()+1==1) return;
	mySem->signal();
}
