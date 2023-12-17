#include <windows.h>
#include <iostream>
#include <winioctl.h>

#define DRIVER_NAME L"\\\\.\\SLoaderCtl"

#define LOADING CTL_CODE(FILE_DEVICE_UNKNOWN, 0x820, METHOD_BUFFERED, FILE_ANY_ACCESS)

int main(int argc, char* argv[])
{
	HANDLE hDevice = CreateFile(
		DRIVER_NAME,
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		std::cout << "fail device, error code: " << GetLastError() << "..." << std::endl;
		return false;
	}

	DWORD dwOpt = 0;
	UCHAR optBuff[] = { 0 };
	WCHAR sys[MAX_PATH] = { 0 };
	MultiByteToWideChar(CP_UTF8, 0, argv[1], strlen(argv[1]), sys, MAX_PATH);
	if (DeviceIoControl(hDevice, LOADING, sys, sizeof(sys), optBuff, 0, &dwOpt, NULL))
	{
		std::cout << "result:";
		for (int i = 0; i < (int)dwOpt; i++)
		{
			std::cout << std::hex << (int)optBuff[i];
		}
	}

	CloseHandle(hDevice);
	return 0;
}