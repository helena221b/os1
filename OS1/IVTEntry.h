#ifndef IVTENTRY_H_
#define IVTENTRY_H_


typedef void interrupt  (*InterruptRoutine)(...);
class KernelEv;
class IVTEntry{
public:
	IVTEntry(int,InterruptRoutine intRoutine);
   ~IVTEntry();
    void signal();
    void callPreviousRoutine();

private:
    friend class KernelEv;

    static IVTEntry* IVTTable[];
    unsigned int ivtNo;
    KernelEv* event;
    InterruptRoutine previousRoutine;
};

#endif
