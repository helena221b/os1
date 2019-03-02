#include "IVTEntry.h"
#include "system.h"
#include "krnlev.h"

#include <dos.h>

IVTEntry* IVTEntry::IVTTable[256];

IVTEntry::IVTEntry(int ivtNo,InterruptRoutine intRoutine) {
	this->event = new KernelEv(ivtNo);
	this->ivtNo=ivtNo;
	this->IVTTable[ivtNo]=this;
	lock();
	this->previousRoutine = getvect(ivtNo);
		setvect(this->ivtNo,intRoutine);
	unlock();
}
IVTEntry::~IVTEntry() {
	lock();
	setvect(ivtNo,previousRoutine);
	unlock();
}
void IVTEntry::signal() {
	this->event->signal();
}
void IVTEntry::callPreviousRoutine(){
	previousRoutine();
}
