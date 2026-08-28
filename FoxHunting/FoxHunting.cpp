#include "FoxHunting.h"

namespace Input
{
	constexpr SHORT KEY_DOWN = 0x8000;

	bool isDown(int key)
	{
		return (GetAsyncKeyState(key) & KEY_DOWN) != 0;
	}

	void keyDown(BYTE key)
	{
		INPUT input{};
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = key;
		input.ki.dwFlags = KEYEVENTF_EXTENDEDKEY;

		SendInput(1, &input, sizeof(INPUT));
	}

	void keyUp(BYTE key)
	{
		INPUT input{};
		input.type = INPUT_KEYBOARD;
		input.ki.wVk = key;
		input.ki.dwFlags =
			KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP;

		SendInput(1, &input, sizeof(INPUT));
	}

	void tap(BYTE key)
	{
		keyDown(key);
		keyUp(key);
	}
}

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
	t1 = std::thread(keyThread, msg.hwnd);

	while (GetMessage(&msg, nullptr, 0, 0) && running)
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

void keyThread(HWND hWnd)
{
	InputState state;

	while (running.load())
	{
		if (Input::isDown(VK_END))
		{
			running.store(false);
			break;
		}
		handleModeKeys(state);
		if (!pause.load() && !isCursorVisible())
		{
			handleWeaponKeys(state);
			handleFire(state);
		}
		Sleep(5);
	}
	cleanup(hWnd);
}

void customFire(bool fireOnly)
{
	Input::tap(static_cast<BYTE>(fireKey));

	if (fireOnly)
	{
		Sleep(randomInt(10, 30));
		return;
	}

	Input::tap('3');

	Sleep(randomInt(70, sleepDuration));

	Input::tap('1');
}

void defaultFire()
{
	Input::keyDown(static_cast<BYTE>(fireKey));

	while (Input::isDown(VK_LBUTTON))
		Sleep(1);

	Input::keyUp(static_cast<BYTE>(fireKey));
}

int randomInt(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 generator(rd());

	if (min > max)
		std::swap(min, max);

	std::uniform_int_distribution<int> distribution(min, max);

	return distribution(generator);
}

bool isCursorVisible()
{
	CURSORINFO cursorInfo{};
	cursorInfo.cbSize = sizeof(cursorInfo);

	if (!GetCursorInfo(&cursorInfo))
		return true;

	return (cursorInfo.flags & CURSOR_SHOWING) != 0;
}

void resetGrenadeState(InputState& state)
{
	if (state.mode == WeaponMode::Sniper)
		state.stopQuickSwitch = false;

	if (state.mode == WeaponMode::Rifles)
		state.stopFastClicker = false;

	state.grenade = false;
}

void handleModeKeys(InputState& state)
{
	static bool previousF3 = false;
	static bool previousF1 = false;
	static bool previousF2 = false;
	static bool previousHome = false;
	static bool previousDelete = false;

	// --------------------------------------------------------
	// F3 = Sniper
	// --------------------------------------------------------

	const bool f3 = Input::isDown(VK_F3);

	if (f3 && !previousF3)
	{
		state.mode = WeaponMode::Sniper;

		Beep(1500, 500);
	}

	previousF3 = f3;


	// --------------------------------------------------------
	// F1 / F2 = Rifles
	// --------------------------------------------------------

	const bool f1 = Input::isDown(VK_F1);
	const bool f2 = Input::isDown(VK_F2);

	if ((f1 && !previousF1) ||
		(f2 && !previousF2))
	{
		state.mode = WeaponMode::Rifles;

		Beep(2000, 500);
	}

	previousF1 = f1;
	previousF2 = f2;


	// --------------------------------------------------------
	// HOME = Pause
	// --------------------------------------------------------

	const bool home = Input::isDown(VK_HOME);

	if (home && !previousHome)
	{
		pause = !pause.load();

		Beep(660, pause ? 1000 : 500);
	}

	previousHome = home;


	// --------------------------------------------------------
	// DELETE = Normal
	// --------------------------------------------------------

	const bool deleteKey = Input::isDown(VK_DELETE);

	if (deleteKey && !previousDelete)
	{
		state.mode = WeaponMode::Normal;

		Beep(200, 1000);
	}

	previousDelete = deleteKey;
}

void handleWeaponKeys(InputState& state)
{

	if (Input::isDown('1'))
	{
		if (state.mode == WeaponMode::Sniper)
		{
			state.stopQuickSwitch = false;
			state.stopFastClicker = true;
		}
		else if (state.mode == WeaponMode::Rifles)
		{
			state.stopQuickSwitch = true;
			state.stopFastClicker = false;
		}
	}
	if (Input::isDown('2'))
	{
		state.stopQuickSwitch = true;
		state.stopFastClicker = false;
	}
	if (Input::isDown('3') ||
		Input::isDown('5'))
	{
		state.stopQuickSwitch = true;
		state.stopFastClicker = true;
	}
	if (Input::isDown('4'))
	{
		state.stopQuickSwitch = true;
		state.stopFastClicker = true;
		state.grenade = true;
	}
}

void handleFire(InputState& state)
{
	if (!Input::isDown(VK_LBUTTON))
		return;

	if (state.mode == WeaponMode::Sniper &&
		!state.stopQuickSwitch)
	{
		customFire(false);

		while (Input::isDown(VK_LBUTTON))
			Sleep(1);

		return;
	}

	if (!state.stopFastClicker &&
		state.mode != WeaponMode::Normal)
	{
		customFire(true);

		while (Input::isDown(VK_LBUTTON))
		{
			customFire(true);
			Sleep(2);
		}

		return;
	}

	defaultFire();

	if (state.grenade)
		resetGrenadeState(state);

	if (Input::isDown(VK_RBUTTON) &&
		state.grenade)
	{
		resetGrenadeState(state);
	}
}

void cleanup(HWND hWnd) {
	Beep(900, 500);
	ShowWindow(hWnd, SW_NORMAL);
	Shell_NotifyIcon(NIM_DELETE, &nid);
	PostQuitMessage(0);
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
				running = false;
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
			running = false;
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
	pause = true;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDC_BUTTON1:
		{
			BOOL success;
			int value = GetDlgItemInt(hDlg, IDC_TIMEVAR, &success, TRUE);

			if (success) {
				sleepDuration = value;
			}
			wchar_t buffer[5];
			wsprintf(buffer, L"%d", sleepDuration);
			MessageBox(NULL, buffer, buffer, MB_OK);
		}
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			pause = false;
			return (INT_PTR)TRUE;
			break;
		default:
			break;
		}
		break;
	}
	}
	return (INT_PTR)FALSE;
}