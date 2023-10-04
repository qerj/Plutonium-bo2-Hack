#include <iostream>
#include <Windows.h>


float normalFov = 106.0f;

DWORD WINAPI Trainer(LPVOID param)
{
	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);

	uintptr_t plutoniumModule = (uintptr_t)GetModuleHandleA("plutonium-bootstrapper-win32.exe");

	float fov = 106.0f;
	bool noHud = false;

	DWORD old_protect;
	unsigned char* noHudAddress = (unsigned char*)(plutoniumModule + 0x1DE53A);
	VirtualProtect((void*)noHudAddress, 3, PAGE_EXECUTE_READWRITE, &old_protect);

	while (!GetAsyncKeyState(VK_END))
	{
		
		*(float*)(plutoniumModule + 0x2641440) = fov;


		if (GetAsyncKeyState(VK_F1) & 1)
		{
			fov++;
		}
		if (GetAsyncKeyState(VK_F2) & 1)
		{
			fov--;
		}

		if (GetAsyncKeyState(VK_F3) & 1)
		{
			noHud = !noHud;
		}

		// no hud is a little broken as I didn't test this in anything else but single-player.
		// if you have it enabled and get killed, a KILLCAM texture will overlap like your watching a killcam.
		// if i find out how to make this a perm functionality I will add it but for no this is a broken hack
		// aye but fov works though?
		if (noHud) {
			*noHudAddress = 0xC1;
			*(noHudAddress + 1) = 0xE0;
			*(noHudAddress + 2) = 0x4;
			//0xC1 0xE0 0x04
		}
		else if (!noHud) {
			*(noHudAddress + 2) = 0x15;
			//0xC1 0xE0 0x15
		}
	}

	//reset our hacks
	*(float*)(plutoniumModule + 0x2641440) = normalFov;
	*noHudAddress = 0xC1;
	*(noHudAddress + 1) = 0xE0;
	*(noHudAddress + 2) = 0x4;

	fclose(f);
	FreeConsole();
	FreeLibraryAndExitThread((HMODULE)param, 0); 
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_THREAD_ATTACH:  break;
	case DLL_THREAD_DETACH:  break;
	case DLL_PROCESS_ATTACH:
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Trainer, hModule, 0, nullptr);
		DisableThreadLibraryCalls(hModule);
		break;
	}
	return TRUE;
}
