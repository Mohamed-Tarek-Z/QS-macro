#include "FoxHunting.h"


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, 100);
	LoadStringW(hInstance, IDC_FOXHUNTING, szWindowClass, 100);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FOXHUNTING));
	MSG msg{};
	GetMessage(&msg, nullptr, 0, 0);
	t1 = std::thread(foxed, msg.hwnd);

	while (GetMessage(&msg, nullptr, 0, 0) && run)
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
	WNDCLASSEXW wcex{};

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FOXHUNTING));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_FOXHUNTING);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_FOXHUNTING));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, 500, 500, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

void ShowContextMenu(HWND hwnd, POINT pt)
{
	HMENU hMenu = LoadMenu(hInst, MAKEINTRESOURCE(IDC_CONTEXTMENU));
	if (hMenu)
	{
		HMENU hSubMenu = GetSubMenu(hMenu, 0);
		if (hSubMenu)
		{

			SetForegroundWindow(hwnd);

			UINT uFlags = TPM_RIGHTBUTTON;
			if (GetSystemMetrics(SM_MENUDROPALIGNMENT) != 0)
			{
				uFlags |= TPM_RIGHTALIGN;
			}
			else
			{
				uFlags |= TPM_LEFTALIGN;
			}

			TrackPopupMenuEx(hSubMenu, uFlags, pt.x, pt.y, hwnd, NULL);
		}
		DestroyMenu(hMenu);
	}
}

void foxed(HWND hWnd)
{
	while (run)
	{
		if (GetAsyncKeyState(VK_END))
		{
			run = false;
		}
		CURSORINFO cursor_info = { 0 };
		cursor_info.cbSize = sizeof(CURSORINFO);
		GetCursorInfo(&cursor_info);
		if (GetAsyncKeyState(VK_F3))
		{
			f3 = true;
			f2 = false;
			normal = false;
			Beep(1500, 500);
		}
		if (GetAsyncKeyState(VK_F1) || GetAsyncKeyState(VK_F2))
		{
			f3 = false;
			f2 = true;
			normal = false;
			Beep(2000, 500);
		}
		if (GetAsyncKeyState(VK_HOME))
		{
			pu = !pu;
			Beep(660, pu ? 1000 : 500);
		}
		if (GetAsyncKeyState(VK_DELETE))
		{
			f3 = false;
			f2 = false;
			normal = true;
			Beep(200, normal ? 1000 : 500);
		}
		if (!(cursor_info.flags & CURSOR_SHOWING) == 1 && !pu)
		{
			if (GetAsyncKeyState(0x31))
			{
				if (f3) {
					stopqs = false;
					stopmc = true;
				}
				else if (f2)
				{
					stopqs = true;
					stopmc = false;
				}
			}
			if (GetAsyncKeyState(0x32))
			{
				stopqs = true;
				stopmc = false;
			}
			if (GetAsyncKeyState(0x33) || GetAsyncKeyState(0x35))
			{
				stopqs = true;
				stopmc = true;
			}
			if (GetAsyncKeyState(0x34))
			{
				stopqs = true;
				stopmc = true;
				nade = true;
			}
			if (GetAsyncKeyState(VK_LBUTTON))
			{
				if (f3 && !stopqs && !normal)
				{
					doQs(false);
					while (GetAsyncKeyState(VK_LBUTTON))
						Sleep(1);
				}
				else if (!stopmc && !normal) {
					doQs(true);
					while (GetAsyncKeyState(VK_LBUTTON))
					{
						doQs(true);
						Sleep(2);
					}
				}
				else
				{

					keybd_event(VK_OEM_5, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);

					while (GetAsyncKeyState(VK_LBUTTON))
						Sleep(1);

					keybd_event(VK_OEM_5, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);

					if (nade)
					{
						if (f3)
							stopqs = false;
						if (f2)
							stopmc = false;
						nade = false;
					}
				}
				if (GetAsyncKeyState(VK_RBUTTON) && nade)
				{
					if (f3)
						stopqs = false;
					if (f2)
						stopmc = false;
					nade = false;
				}
			}
		}
		Sleep(5);
	}
	{
		Beep(900, 500);
		ShowWindow(hWnd, SW_NORMAL);
		Shell_NotifyIcon(NIM_DELETE, &nid);
		PostQuitMessage(0);
	}
}

void doQs(bool fireOnly)
{
	keybd_event(VK_OEM_5, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
	keybd_event(VK_OEM_5, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	if (fireOnly)
		return;
	keybd_event(0x51, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
	keybd_event(0x51, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
	Sleep(sleepf);
	keybd_event(0x31, 0x45, KEYEVENTF_EXTENDEDKEY | 0, 0);
	keybd_event(0x31, 0x45, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	try
	{
		switch (message)
		{
		case WM_CREATE:
		{
			nid.hWnd = hWnd;
			nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE | NIF_SHOWTIP;
			nid.uCallbackMessage = WMAPP_NOTIFYCALLBACK;
			nid.hIcon = (HICON)LoadImage(NULL, L"FoxHunting.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE | LR_SHARED);
			LoadString(hInst, IDS_APP_TITLE, nid.szTip, ARRAYSIZE(nid.szTip));
			Shell_NotifyIcon(NIM_ADD, &nid);
			nid.uVersion = NOTIFYICON_VERSION_4;
			Shell_NotifyIcon(NIM_SETVERSION, &nid);
			ShowWindow(hWnd, SW_HIDE);
		}
		break;
		case WM_SIZE:
		{
			if (LOWORD(wParam) == SIZE_MINIMIZED)
				ShowWindow(hWnd, SW_HIDE);
		}
		break;
		case WM_COMMAND:
		{

			int wmId = LOWORD(wParam);
			// Parse the menu selections:
			switch (wmId)
			{
			case ID_FILE_EDITTIME:
				DialogBox(hInst, MAKEINTRESOURCE(IDD_FORMVIEW), hWnd, EditTime);
				break;
			case IDM_EXIT:
				run = false;
				if (t1.joinable())
					t1.join();
				Shell_NotifyIcon(NIM_DELETE, &nid);
				PostQuitMessage(0);
				exit(0);
				break;
			default:
				return DefWindowProc(hWnd, message, wParam, lParam);
			}
		}
		break;
		case WM_DESTROY:
		{
			run = false;
			if (t1.joinable())
				t1.join();
			Shell_NotifyIcon(NIM_DELETE, &nid);
			PostQuitMessage(0);
			exit(0);
		}
		break;
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
		}
		break;
		case WMAPP_NOTIFYCALLBACK:
		{

			switch (LOWORD(lParam))
			{
			case NIN_SELECT:
			{
				ShowWindow(hWnd, SW_NORMAL);
			}
			break;
			case WM_CONTEXTMENU:
			{
				POINT const pt = { LOWORD(wParam), HIWORD(wParam) };
				ShowContextMenu(hWnd, pt);
			}
			break;
			}
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

	}
	catch (const std::exception&)
	{
		t1.join();
	}
	return 0;
}

INT_PTR CALLBACK EditTime(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	pu = true;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
			char bu[5];
			GetDlgItemTextW(hDlg, IDC_TIMEVAR, (LPWSTR)bu, 4);
			sleepf = myAtoi(bu);
			wchar_t buffer[5];
			wsprintf(buffer, L"%d", sleepf);
			SetDlgItemTextW(hDlg, IDC_TIMELABL, buffer);
			MessageBox(NULL, buffer, buffer, MB_OK);
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			pu = false;
			return (INT_PTR)TRUE;
			break;
		default:
			break;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


int myAtoi(char* str)
{
	int res = 0;
	for (int i = 0; i < 5; ++i) {
		if (str[i] == '\0') {
			continue;
		}
		else {
			res = res * 10 + str[i] - '0';
		}
	}

	return res;
}