#pragma once

#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <conio.h>
#include <shellapi.h>
#include <commctrl.h>
#include <strsafe.h>
#include <thread>

#include "resource.h"

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	EditTime(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void                ShowContextMenu(HWND hwnd, POINT pt);
void				foxed(HWND hWnd);
void				doQs(bool fire);
int					myAtoi(char* str);

HINSTANCE hInst;
std::thread t1;
WCHAR szTitle[100];
WCHAR szWindowClass[100];

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;

NOTIFYICONDATA nid = { sizeof(nid) };

bool stopqs = true, stopmc = true, f3 = false, f2 = false, nade = false, pu = false, run = true, normal = true;
int sleepf = 220;
