#include "Loading.h"

SLoading::SLoading()
{
}

SLoading::~SLoading()
{
}

VOID SLoading::DriverLoading(LPCWSTR lpDriverPath, LPCWSTR lpServiceName)
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		MessageBox(NULL, L"Loading Fail,0xE2000001", L"fail", 0);
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
		MessageBox(NULL, L"Loading Fail,0xE2000002", L"fail", 0);
		CloseServiceHandle(hSCManager);
		return;
	}

	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hSCService);
}

VOID SLoading::DriverActivate(LPCWSTR lpServiceName)
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		MessageBox(NULL, L"Loading Fail,0xE2000011", L"fail", 0);
		return;
	}

	SC_HANDLE hSCService = OpenService(hSCManager, lpServiceName, SERVICE_START);
	if (!hSCManager)
	{
		CloseServiceHandle(hSCManager);
		MessageBox(NULL, L"Loading Fail,0xE2000012", L"fail", 0);
		return;
	}

	if (!StartService(hSCService, NULL, NULL))
	{
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService);
		MessageBox(NULL, L"Loading Fail,0xE2000013", L"fail", 0);
		return;
	}

	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hSCService);
}

VOID SLoading::DriverClose(LPCWSTR lpServiceName)
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		MessageBox(NULL, L"Loading Fail,0xE2000021", L"fail", 0);
		return;
	}

	SC_HANDLE hSCService = OpenService(hSCManager, lpServiceName, SERVICE_STOP);
	if (!hSCManager)
	{
		CloseServiceHandle(hSCManager);
		MessageBox(NULL, L"Loading Fail,0xE2000022", L"fail", 0);
		return;
	}

	SERVICE_STATUS lpServiceStatus = { 0 };
	if (!ControlService(hSCService, SERVICE_CONTROL_STOP, &lpServiceStatus))
	{
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService);
		MessageBox(NULL, L"Loading Fail,0xE2000023", L"fail", 0);
		return;
	}

	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hSCService);
}

VOID SLoading::DriverUnload(LPCWSTR lpServiceName)
{
	SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!hSCManager)
	{
		MessageBox(NULL, L"Loading Fail,0xE2000031", L"fail", 0);
		return;
	}

	SC_HANDLE hSCService = OpenService(hSCManager, lpServiceName, SERVICE_STOP | DELETE);
	if (!hSCManager)
	{
		CloseServiceHandle(hSCManager);
		MessageBox(NULL, L"Loading Fail,0xE2000032", L"fail", 0);
		return;
	}
	if (!DeleteService(hSCService))
	{
		CloseServiceHandle(hSCManager);
		CloseServiceHandle(hSCService);
		MessageBox(NULL, L"Loading Fail,0xE2000033", L"fail", 0);
		return;
	}

	CloseServiceHandle(hSCManager);
	CloseServiceHandle(hSCService);
}