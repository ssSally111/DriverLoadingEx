#pragma once

#include "resource.h"
#include <commdlg.h>
#include "Loading.h"
#include "LoadingEx.h"

using namespace std;

#define WIN_WIDTH 500
#define WIN_HEIGHT 195

SLoading Loading;
SLoadingEx LoadingEx;

VOID InitWin(HWND hWnd, LPARAM lParam);