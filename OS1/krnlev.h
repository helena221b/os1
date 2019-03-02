#ifndef KRNLEV_H_
#define KRNLEV_H_

typedef unsigned char IVTNo;
class Thread;
class KernelSem;

class KernelEv{
public:
	KernelEv(IVTNo ivtnumber);
   ~KernelEv();
   void wait();
   void signal();

private:
   IVTNo ivtNo;
   Thread* ownerThread;
   KernelSem* mySem;
};
#endif
