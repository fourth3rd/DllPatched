// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include<string>

extern "C" int __stdcall GetAddress();

extern "C" void __declspec(dllexport) ThisIsThePlace()
{
	MessageBoxEx(NULL, L"This is the place where you are looking for", 0, 0, 0);
}

void StartProtection()
{
	MessageBoxEx(NULL, L"Start Protection", 0, 0, 0);
}

extern "C" void __declspec(dllexport) CalledFunction()
{
	MessageBoxEx(NULL, L" Called Function ", 0, 0, 0);
}

void Decrypted(HMODULE hModule)
{

	GetAddress();
	char* BaseAddress = (char*)hModule;
	long long int* i64BaseAddrss = (long long int*)hModule;
	int* i32BaseAddrss = (int*)hModule;
	int LoadMemorySize = BaseAddress[0x140 / 4];
	int Start = 0x241e0;

	int TotalIndex = i64BaseAddrss[0x241e0 / 8];
	//memcpy((void*)&TotalIndex, (int*)&BaseAddress[0x1e5b0], 4);

//	printf("%d\n", TotalIndex);
	for(int i = 1; i <= TotalIndex; i++)
	{
		int idx = i32BaseAddrss[Start / 4 + i];
		//memcpy((void*)&idx, &i32BaseAddrss[Start/4 + i], 4);
		long long int RawToRva = idx + 0x11000 - 0x400;//
		for(int i = 0; i < 5; i++)
		{
			BaseAddress[RawToRva + i] ^= 8;
		}
	}

}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		{
			Decrypted(hModule);
			ThisIsThePlace();
			StartProtection();
			break;
		}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

