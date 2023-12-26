#include "framework.h"
#include "WDriverLoadingEx.h"
#include <stdio.h>
#include <iostream>

#define MAX_LOADSTRING 100

HWND winText;
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

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX,
		CW_USEDEFAULT, 0, WIN_WIDTH, WIN_HEIGHT, nullptr, nullptr, hInstance, nullptr);

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
	case WM_CREATE:
	{
		InitWin(hWnd, lParam);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case CBN_SELCHANGE:
		{
			// todo: temp on ComboBox msg
			int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL,(WPARAM)0, (LPARAM)0);
			WCHAR  ListItem[256];
			SendMessage((HWND)lParam, (UINT)CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
			MessageBox(hWnd, (LPCWSTR)ListItem, L"Item Selected", MB_OK);
			break;
		}
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

VOID InitWin(HWND hWnd, LPARAM lParam)
{
	winText = CreateWindow(L"STATIC", L"", WS_CAPTION ^ 0x00400000L | 0x40000000L | WS_VISIBLE | SS_LEFTNOWORDWRAP | SS_WORDELLIPSIS | SS_SUNKEN,
		100, 30, 320, 20, hWnd, (HMENU)1, hInst, NULL);

	HWND winButnOpenFile = CreateWindow(L"BUTTON", L"选择驱动", WS_TABSTOP | WS_VISIBLE | 0x40000000L | BS_DEFPUSHBUTTON,
		10, 30, 70, 20, hWnd, (HMENU)2, hInst, NULL);

	HWND winButnStartR0 = CreateWindow(L"BUTTON", L"R0加载运行", WS_TABSTOP | WS_VISIBLE | 0x40000000L | BS_DEFPUSHBUTTON,
		10, 70, 80, 20, hWnd, (HMENU)3, hInst, NULL);

	HWND winButnEndR0 = CreateWindow(L"BUTTON", L"R0关闭卸载", WS_TABSTOP | WS_VISIBLE | 0x40000000L | BS_DEFPUSHBUTTON,
		110, 70, 80, 20, hWnd, (HMENU)4, hInst, NULL);

	HWND winComboBox = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | CBS_HASSTRINGS | 0x40000000L | WS_OVERLAPPED | WS_VISIBLE,
		200, 70, 170, 20, hWnd, (HMENU)5, hInst, NULL);

	WCHAR Planets[4][19] =
	{
		L"模式1-Loading",L"模式2-LoadingEx",L"模式3-PatchLoading",L"模式3-PatchLoadingEx"
	};

	WCHAR list[40];
	memset(&list, 0, sizeof(list));
	for (int k = 0; k < 4; k++)
	{
		wcscpy_s(list, sizeof(list) / sizeof(WCHAR), (WCHAR*)Planets[k]);
		SendMessage(winComboBox, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)list);
	}
	SendMessage(winComboBox, CB_SETCURSEL, (WPARAM)2, (LPARAM)0);

	// ...

}