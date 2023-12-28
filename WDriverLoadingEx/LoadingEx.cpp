#include "LoadingEx.h"

SLoadingEx::SLoadingEx()
{
	InitDriverControls();
}

SLoadingEx::~SLoadingEx()
{
	if (g_hDevice) CloseHandle(g_hDevice);
}

HANDLE InitDevice()
{
	return CreateFile(
		DRIVER_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
}

VOID SLoadingEx::InitDriverControls()
{
	HANDLE hDevice = InitDevice();
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		// Attempt to load driver.
		LPCWSTR lpServiceName = L"SLoader";
		LPCWSTR lpDriverPath = L".\\SLoader.sys";

		if (_waccess(lpDriverPath, 0) == ENOENT)
		{
			MessageBox(NULL, L"load device fail 0x00001101", L"init fail", MB_OK);
			return;
		}


		SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
		if (!hSCManager)
		{
			MessageBox(NULL, L"load device fail 0x00001000", L"init fail", MB_OK);
			return;
		}

		SC_HANDLE hSCService = CreateService(
			hSCManager,
			lpServiceName,
			lpServiceName,
			SERVICE_ALL_ACCESS,
			SERVICE_KERNEL_DRIVER,
			SERVICE_DEMAND_START,
			SERVICE_ERROR_IGNORE,
			lpDriverPath,
			NULL,
			NULL,
			NULL,
			NULL,
			NULL);
		if (!hSCService)
		{
			MessageBox(NULL, L"load device fail 0x00002000", L"init fail", MB_OK);
			CloseServiceHandle(hSCManager);
			return;
		}

		if (!StartService(hSCService, NULL, NULL))
		{
			CloseServiceHandle(hSCManager);
			CloseServiceHandle(hSCService);
			MessageBox(NULL, L"load device fail 0x00003000", L"init fail", MB_OK);
			return;
		}

		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService);

		HANDLE hDevice = InitDevice();
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			MessageBox(NULL, L"load device fail 0x00004000", L"init fail", MB_OK);
			return;
		}
	}
	g_hDevice = hDevice;
}

inline VOID SLoadingEx::Send(DWORD code, PVOID p, DWORD buffSize)
{
	if (!g_hDevice)
	{
		MessageBox(NULL, L"load device fail 0x00009010", L"init fail", MB_OK);
		return;
	}

	DWORD dwOpt = 0;
	CHAR optBuff[9];
	if (DeviceIoControl(g_hDevice, code, p, buffSize, optBuff, 8, &dwOpt, NULL))
	{
		if (dwOpt == 8 && *(PLONG)optBuff)
		{
			CHAR msg[] = "fail 0xC0001000,";
			optBuff[8] = '\0';
			strcat_s(msg, optBuff);
			MessageBoxA(NULL, msg, "fail", MB_OK);
		}
	}
}

VOID SLoadingEx::Loading(PCTSTR pSys)
{
	Send(LOADING, (PVOID)pSys, sizeof(pSys));
}

VOID SLoadingEx::LoadingEx(PCTSTR pSys)
{
	Send(LOADINGEX, (PVOID)pSys, sizeof(pSys));
}

VOID SLoadingEx::PatchLoading(PPATCHCILOAD_ENTRY pPatchLoadEntry)
{
	Send(LOADING, (PVOID)pPatchLoadEntry, sizeof(pPatchLoadEntry));
}

VOID SLoadingEx::UnloadDriver(PCWSTR pSysName)
{
	Send(LOADING, (PVOID)pSysName, sizeof(pSysName));
}
