#include <Windows.h>
#include <stdio.h>

//DLLPlugin2

extern "C"{
	__declspec(dllexport) void func(){
		printf("TEST2\n");
	}
}

