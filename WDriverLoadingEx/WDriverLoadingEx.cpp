#include "framework.h"
#include "WDriverLoadingEx.h"

#define MAX_LOADSTRING 100

HANDLE g_hDevice;
HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"WDriverLoadingEx";
WCHAR szWindowClass[MAX_LOADSTRING] = L"WDriverLoadingEx";

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此处放置代码。


	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_WDRIVERLOADINGEX, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WDRIVERLOADINGEX));

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}




ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WDRIVERLOADINGEX));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_WDRIVERLOADINGEX);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
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

VOID InitDriverControls()
{
	HANDLE hDevice = InitDevice();
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		// Attempt to load driver.
		LPCWSTR lpServiceName = L"SLoader";
		LPCWSTR lpDriverPath = L".\\SLoader.sys";
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

VOID Send(HANDLE hDevice, DWORD code, PVOID p, DWORD buffSize)
{
	DWORD dwOpt = 0;
	CHAR optBuff[9];
	if (DeviceIoControl(hDevice, code, p, buffSize, optBuff, 8, &dwOpt, NULL))
	{
		if (dwOpt == 8 && *(PLONG)optBuff)
		{
			CHAR msg[] = "load device fail 0xC0001000,";
			optBuff[8] = '\0';
			strcat_s(msg, optBuff);
			MessageBoxA(NULL, msg, "init fail", MB_OK);
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