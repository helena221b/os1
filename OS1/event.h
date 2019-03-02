#ifndef EVENT_H_
#define EVENT_H_

/*#define PREPAREENTRY(ivtno,old)\
void interrupt routine##ivtno(...);\
IVTEntry ent##ivtno(ivtno,routine##ivtno);\
void interrupt routine##ivtno(...) {\
	if (old) ent##ivtno.callPreviousRoutine();\
	ent##ivtno.signal();\
}*/
#include "IVTEntry.h"

#define PREPAREENTRY(numEntry, callOld)\
void interrupt inter##numEntry(...); \
IVTEntry newEntry##numEntry(numEntry, inter##numEntry); \
void interrupt inter##numEntry(...) {\
newEntry##numEntry.signal();\
if (callOld == 1)\
newEntry##numEntry.callPreviousRoutine();\
}

typedef unsigned char IVTNo;
class KernelEv;

class Event{
public:
	Event(IVTNo ivtNo);
   ~Event();

   void wait();
protected:
   friend class KernelEv;
   void signal();
private:
 	 KernelEv* myImpl;
};

#endif
