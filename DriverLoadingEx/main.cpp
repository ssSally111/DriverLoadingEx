#include <windows.h>
#include <iostream>
#include <winioctl.h>

#define DRIVER_NAME L"\\\\.\\SLoaderCtl"

#define PATCHCILOAD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x810, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LOADING CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define LOADINGEX CTL_CODE(FILE_DEVICE_UNKNOWN, 0x830, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define UNLOAD CTL_CODE(FILE_DEVICE_UNKNOWN, 0x840, METHOD_BUFFERED, FILE_ANY_ACCESS)

HANDLE g_hDevice;

// PATCHCILOAD ENTRY
typedef struct _PATCHCILOAD_ENTRY
{
	PCWSTR sysName;
	INT loadMode;
}PATCHCILOAD_ENTRY, * PPATCHCILOAD_ENTRY;

VOID Send(HANDLE hDevice, DWORD code, PVOID p, DWORD buffSize)
{
	DWORD dwOpt = 0;
	CHAR optBuff[9];
	if (DeviceIoControl(hDevice, code, p, buffSize, optBuff, 8, &dwOpt, NULL))
	{
		LONG result;
		if (dwOpt == 8 && (result = *(PLONG)optBuff))
		{
			std::cout << "load device fail, error code: " << result << "..." << std::endl;
		}
	}
}

VOID Loading(PCTSTR pSys)
{
	Send(g_hDevice, LOADING, (PVOID)pSys, sizeof(pSys));
}

VOID LoadingEx(PCTSTR pSys)
{
	Send(g_hDevice, LOADINGEX, (PVOID)pSys, sizeof(pSys));
}

VOID PatchLoading(PPATCHCILOAD_ENTRY pPatchLoadEntry)
{
	Send(g_hDevice, LOADING, (PVOID)pPatchLoadEntry, sizeof(pPatchLoadEntry));
}

VOID UnloadDriver(PCWSTR pSysName)
{
	Send(g_hDevice, LOADING, (PVOID)pSysName, sizeof(pSysName));
}

int main(int argc, char* argv[])
{
	g_hDevice = CreateFile(
		DRIVER_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (g_hDevice == INVALID_HANDLE_VALUE)
	{
		std::cout << "fail device, error code: " << GetLastError() << "..." << std::endl;
		return false;
	}

	// argv
	INT i1 = -1;
	INT i2 = 1;
	WCHAR sys[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, argv[1], strlen(argv[1]), sys, MAX_PATH);
	i1 = strtol(argv[2], NULL, 10);
	i2 = strtol(argv[3], NULL, 10);

	// call
	switch (i1)
	{
	case 1:
	{
		Loading(sys);
		break;
	}
	case 2:
	{
		LoadingEx(sys);
		break;
	}
	case 3:
	{
		PPATCHCILOAD_ENTRY entry;
		entry->loadMode = i2;
		entry->sysName = sys;
		PatchLoading(entry);
		break;
	}
	case 4:
	{
		UnloadDriver(sys);
		break;
	}
	default:
	{
		PPATCHCILOAD_ENTRY entry;
		entry->loadMode = i2;
		entry->sysName = sys;
		PatchLoading(entry);
		break;
	}

	}

	CloseHandle(g_hDevice);
	return 0;
}