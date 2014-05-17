#include <Windows.h>
#include <stdio.h>

//DLLPlugin3

extern "C"{
	__declspec(dllexport) void func(){
		printf("TEST3\n");
	}
}