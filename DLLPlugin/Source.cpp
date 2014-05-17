#include <Windows.h>
#include <stdio.h>

//DLLPlugin1

extern "C"{
	__declspec(dllexport) void func(){
		printf("TEST1\n");
	}
}