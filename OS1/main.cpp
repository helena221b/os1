#include "system.h"
#include "iostream.h"
#include "stdlib.h"
extern int userMain(int argc, char* argv[]);
int main(int argc, char* argv[]){
	System::boot();
	int value=userMain(argc,argv);
	cout<<value<<endl;
	System::unboot();
	return value;
}

