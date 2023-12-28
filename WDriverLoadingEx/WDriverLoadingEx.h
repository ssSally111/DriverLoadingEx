#pragma once

#include "resource.h"
#include <commdlg.h>
#include <shellapi.h>
#include "Loading.h"
#include "LoadingEx.h"

#pragma warning(disable:4996)

using namespace std;

#define WIN_WIDTH 500
#define WIN_HEIGHT 195

#define MAX_FILE_NAME 64

SLoading Loading;
SLoadingEx LoadingEx;

VOID InitWin(HWND hWnd, LPARAM lParam);