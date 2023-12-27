#include "framework.h"
#include "WDriverLoadingEx.h"

#define MAX_LOADSTRING 100

WCHAR szFullFileName[MAX_PATH] = { 0 };
WCHAR szFileName[64] = { 0 };
HWND winText;
HWND winComboBox;
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
		case 2102:
		{
			OPENFILENAME ofn{};
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = szFullFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrFilter = L"驱动(*.sys)\0*.sys\0\0";
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
			::GetOpenFileName(&ofn);

			// fullFileName
			SetWindowText(winText, szFullFileName);

			// fileName
			if (szFullFileName)
				for (WORD i = ofn.nFileOffset; i < ofn.nFileExtension - 1; i++)
					szFileName[i - ofn.nFileOffset] = szFullFileName[i];
			break;
		}
		case 2103:
		{
			// r0 load
			if (szFullFileName)
			{
				LRESULT index = SendMessage(winComboBox, CB_GETCURSEL, wParam, lParam);
				switch (index)
				{
				case 0:
				{
					//LoadingEx.Loading(szFileName);
				}
				case 1:
				{
					//LoadingEx.LoadingEx(szFileName);
				}
				case 2:
				{
					//LoadingEx.Loading(szFileName);
				}
				case 3:
				{
					//LoadingEx.Loading(szFileName);
				}
				default:
					break;
				}
			}
			break;
		}
		case 2104:
		{
			// r0 unload
			if (szFullFileName)
			{
				// LoadingEx.UnloadDriver(szFileName);
			}
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

VOID InitWin(HWND hWnd, LPARAM lParam)
{
	HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	winText = CreateWindow(L"STATIC", L"", WS_CAPTION ^ WS_DLGFRAME | WS_CHILD | WS_VISIBLE | SS_LEFTNOWORDWRAP | SS_WORDELLIPSIS | SS_SUNKEN,
		100, 30, 370, 20, hWnd, (HMENU)2101, hInst, NULL);
	SendMessage(winText, WM_SETFONT, (WPARAM)hFont, 1);

	HWND winButnOpenFile = CreateWindow(L"BUTTON", L"选择驱动", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		10, 30, 70, 20, hWnd, (HMENU)2102, hInst, NULL);
	SendMessage(winButnOpenFile, WM_SETFONT, (WPARAM)hFont, 1);

	HWND winButnStartR0 = CreateWindow(L"BUTTON", L"R0加载运行", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		10, 70, 80, 20, hWnd, (HMENU)2103, hInst, NULL);
	SendMessage(winButnStartR0, WM_SETFONT, (WPARAM)hFont, 1);

	HWND winButnEndR0 = CreateWindow(L"BUTTON", L"R0关闭卸载", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		100, 70, 80, 20, hWnd, (HMENU)2104, hInst, NULL);
	SendMessage(winButnEndR0, WM_SETFONT, (WPARAM)hFont, 1);

	winComboBox = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_DROPDOWNLIST,
		220, 70, 248, 20, hWnd, (HMENU)2105, hInst, NULL);
	SendMessage(winComboBox, WM_SETFONT, (WPARAM)hFont, 1);
	const WCHAR* Planets[] = {
		L"模式1 - 正常加载",
		L"模式2 - 扩展加载(只能重启卸载)",
		L"模式3 - 无签驱动正常加载",
		L"模式4 - 无签驱动扩展加载(只能重启卸载)"
	};
	for (int k = 0; k < 4; k++)
	{
		SendMessage(winComboBox, CB_ADDSTRING, k, (LPARAM)Planets[k]);
	}
	SendMessage(winComboBox, CB_SETCURSEL, 2, (LPARAM)0);

	HWND winButnLoadR3 = CreateWindow(L"BUTTON", L"R3加载", WS_TABSTOP | WS_VISIBLE | 0x40000000L | BS_DEFPUSHBUTTON,
		10, 110, 60, 20, hWnd, (HMENU)2106, hInst, NULL);
	SendMessage(winButnLoadR3, WM_SETFONT, (WPARAM)hFont, 1);

	HWND winButnStartR3 = CreateWindow(L"BUTTON", L"R3运行", WS_TABSTOP | WS_VISIBLE | 0x40000000L | BS_DEFPUSHBUTTON,
		143, 110, 60, 20, hWnd, (HMENU)2107, hInst, NULL);
	SendMessage(winButnStartR3, WM_SETFONT, (WPARAM)hFont, 1);

	HWND winButnCloseR3 = CreateWindow(L"BUTTON", L"R3关闭", WS_TABSTOP | WS_VISIBLE | 0x40000000L | BS_DEFPUSHBUTTON,
		276, 110, 60, 20, hWnd, (HMENU)2108, hInst, NULL);
	SendMessage(winButnCloseR3, WM_SETFONT, (WPARAM)hFont, 1);

	HWND winButnUnLoadR3 = CreateWindow(L"BUTTON", L"R3卸载", WS_TABSTOP | WS_VISIBLE | 0x40000000L | BS_DEFPUSHBUTTON,
		409, 110, 60, 20, hWnd, (HMENU)2109, hInst, NULL);
	SendMessage(winButnUnLoadR3, WM_SETFONT, (WPARAM)hFont, 1);
}
