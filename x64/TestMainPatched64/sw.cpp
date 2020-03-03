#include<stdio.h>
#include<windows.h>

//#pragma comment(lib,"TestDllMainPatched64.lib")

typedef void (*FLPtr)();

//extern "C" void __declspec(dllimport) ThisIsThePlace();

int main()
{
	//ThisIsThePlace();
	
	HANDLE hModule = LoadLibrary(L"TestDllMainPatched64");

	if(!hModule)
	{
		printf("Faild Load Library!!\n");
		return 0;
	}
	FLPtr CalledProtection = (FLPtr)GetProcAddress((HMODULE)hModule, "ThisIsThePlace");

	if(CalledProtection)
	{
		CalledProtection();
	}
}