#pragma once
#include <windows.h>

class SLoading
{
public:
	SLoading();
	~SLoading();

	VOID DriverLoading(LPCWSTR lpDriverPath, LPCWSTR lpServiceName);

	VOID DriverActivate(LPCWSTR lpServiceName);

	VOID DriverClose(LPCWSTR lpServiceName);

	VOID DriverUnload(LPCWSTR lpServiceName);

private:

};

