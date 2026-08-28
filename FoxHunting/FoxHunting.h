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
#include <random>

#include "resource.h"

enum class WeaponMode
{
    Normal,
    Rifles,
    Sniper
};

struct InputState
{
    WeaponMode mode = WeaponMode::Normal;

    bool grenade = false;
    bool stopQuickSwitch = true;
    bool stopFastClicker = true;
};

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	EditTime(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void                ShowContextMenu(HWND hwnd, POINT pt);
void				keyThread(HWND hWnd);
void				customFire(bool fire);
void				defaultFire();
int					randomInt(int min, int max);
bool                isCursorVisible();
void                resetGrenadeState(InputState& state);
void                handleModeKeys(InputState& state);
void                handleWeaponKeys(InputState& state);
void                handleFire(InputState& state);
void                cleanup(HWND hWnd);

HINSTANCE hInst;
std::thread t1;
WCHAR szTitle[100];
WCHAR szWindowClass[100];

UINT const WMAPP_NOTIFYCALLBACK = WM_APP + 1;

NOTIFYICONDATA nid = { sizeof(nid) };

std::atomic_bool running{ true };
std::atomic_bool pause{ false };


extern int fireKey = VK_XBUTTON1;
extern int sleepDuration = 150;
