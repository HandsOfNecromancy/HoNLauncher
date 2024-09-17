#define _UNICODE
#include <windows.h>
#include <wingdi.h>
#include <gdiplus.h>
#include <dwmapi.h>
#include <uxtheme.h>
#include "strings.h"
#include "launcher.h"
#include <array>
#pragma comment (lib,"Gdiplus.lib")
#pragma comment (lib,"dwmapi.lib")
#pragma comment (lib,"uxtheme.lib")

HFONT hFont;
HWND hwndbtplay, hwndbtquit;
HWND hwndComboBox;
HWND hwndEn, hwndDe, hwndFr, hwndRu, hwndEs, hwndPt;

void ButtonAction(int id)
{
	switch(id)
	{
	default:
		return;
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		userlang = id - 1;
		break;
	case 10:
		LaunchGame();
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	case 11:
		DeleteObject(hFont);
		PostQuitMessage(0);
		break;
	case 12:
		vidmode = (int)SendMessage(hwndComboBox, CB_GETCURSEL, 0, 0);
		break;
	}
}

typedef UINT(WINAPI* GetDpiForWindow_t)(HWND);
double GetDpiScale(HWND WindowHandle)
{
	static GetDpiForWindow_t pGetDpiForWindow = nullptr;
	static bool done = false;
	if (!done)
	{
		HMODULE hMod = GetModuleHandleA("User32.dll");
		if (hMod != nullptr) pGetDpiForWindow = reinterpret_cast<GetDpiForWindow_t>(GetProcAddress(hMod, "GetDpiForWindow"));
		done = true;
	}

	if (pGetDpiForWindow)
		return pGetDpiForWindow(WindowHandle) / 96.0;
	else
		return 1.0;
}

void ExpandWindow(HWND hwnd)
{
	// Get the dimensions of the window
	RECT rect;
	GetWindowRect(hwnd, &rect);

	AdjustWindowRectEx(&rect, GetWindowLong(hwnd, GWL_STYLE), false, GetWindowLong(hwnd, GWL_EXSTYLE));

	// Set the position of the window
	SetWindowPos(hwnd, HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, 0);
}

void CenterWindow(HWND hwnd)
{
	// Get the dimensions of the window
	RECT rect;
	GetWindowRect(hwnd, &rect);
	int windowWidth = rect.right - rect.left;
	int windowHeight = rect.bottom - rect.top;

	// Get the dimensions of the screen
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Calculate the position to center the window
	int posX = (screenWidth - windowWidth) / 2;
	int posY = (screenHeight - windowHeight) / 2;

	// Set the position of the window
	SetWindowPos(hwnd, HWND_TOP, posX, posY, 0, 0, SWP_NOSIZE);
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	static Gdiplus::Image* backgroundImage = NULL;
	static Gdiplus::Bitmap* bitmap = NULL;

	static INT tx, ty, sx, sy;

	switch(msg)
	{
		case WM_CREATE:
		{
			Gdiplus::GdiplusStartupInput gdiplusStartupInput;
			ULONG_PTR gdiplusToken;
			Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

			// Load the background image
			backgroundImage = new Gdiplus::Image(L"startup.png");
			bitmap = new Gdiplus::Bitmap(L"extras.png");
			break;
		}
		case WM_PAINT:
		{
			PAINTSTRUCT ps;

			HDC hdc = BeginPaint(hwnd, &ps);

			// Create a Graphics object from the device context
			Gdiplus::Graphics graphics(hdc);

			// Get the dimensions of the window
			RECT rect;
			GetClientRect(hwnd, &rect);
			int windowWidth = rect.right - rect.left;
			int windowHeight = rect.bottom - rect.top;

			// Draw the background image
			if (backgroundImage != NULL)
				graphics.DrawImage(backgroundImage, 0, 0, windowWidth, windowHeight);

			if (bitmap != NULL)
			{
				double dpiScale = GetDpiScale(hwnd);
				sx = bitmap->GetWidth();
				sy = bitmap->GetHeight();

				double sizerx = 160.0 / sx;
				double sizery = 120.0 / sy;
				double sizer = (sizerx > sizery) ? sizerx : sizery;

				sx = INT(sizer * sx);
				sy = INT(sizer * sy);

				tx = 630 - sx;
				ty = 470 - sy;

				tx = INT(GetDpiScale(hwnd) * tx);
				ty = INT(GetDpiScale(hwnd) * ty);
				sx = INT(GetDpiScale(hwnd) * sx);
				sy = INT(GetDpiScale(hwnd) * sy);

				graphics.DrawImage(bitmap, tx, ty, sx, sy);
			}
			else if (bitmap == NULL)
			{
				tx = ty = sx = sy = -1; // disable extras field
			}

			EndPaint(hwnd, &ps);

			break;
		}
		case WM_COMMAND:
			ButtonAction(LOWORD(wparam));
			break;
		case WM_CLOSE:
			DeleteObject(hFont);
			PostQuitMessage(0);
			break;
		case WM_CTLCOLORSTATIC:
		{
			HDC hdcStatic = (HDC)wparam;
			SetBkMode(hdcStatic, TRANSPARENT);
			SetTextColor(hdcStatic, RGB(255, 255, 255)); 
			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
		case WM_LBUTTONUP:
		{
			int x = LOWORD(lparam);  // horizontal position of cursor 
			int y = HIWORD(lparam);  // vertical position of cursor
			if (x >= tx && x <= tx+sx && y >= ty && y <= ty+sy)
				LaunchExtras();
		}
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR lpCmdLine, int nCmdShow)
{
	LoadSettings();
	commandline = _strdup(lpCmdLine);
	AdjustSettings();

	if (skiplauncher)
	{
		// short circuit window creation, it's not necessary
		LaunchGame();
		return (INT)0;
	}

	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	const char* className = "myWindowClass";
	WNDCLASSEX wc = { };

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WindowProcedure;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = className;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	// WS_EX_CLIENTEDGE

	// WS_OVERLAPPEDWINDOW
	// WS_POPUP
	// WS_DLGFRAME|WS_POPUP
	HWND hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, className, GAMENAME,
		WS_BORDER|WS_CAPTION|WS_SYSMENU, 0, CW_USEDEFAULT, 1, 1, NULL, NULL, hInst, NULL);
	double dpiScale = GetDpiScale(hwnd);
	SetWindowPos(hwnd, HWND_TOP, 0, 0, UINT(640.0 * dpiScale), UINT(480.0 * dpiScale), SWP_NOMOVE);
	ExpandWindow(hwnd);
	CenterWindow(hwnd);
	//SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

	//Image image(L"Grapes.jpg");
	//graphics.DrawImage(&image, 60, 10);

	if(hwnd == NULL)
	{
		MessageBox(NULL, "Window Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return 0;
	}

	BOOL setv = TRUE;
	DwmSetWindowAttribute(hwnd, 20/*DWMWA_USE_IMMERSIVE_DARK_MODE*/, &setv, sizeof(setv));

	LONG bordercolor = RGB(51, 51, 51);
	LONG captioncolor = RGB(33, 33, 33);
	LONG textcolor = RGB(226, 223, 219);
	DwmSetWindowAttribute(hwnd, 34/*DWMWA_BORDER_COLOR*/, &bordercolor, sizeof(LONG));
	DwmSetWindowAttribute(hwnd, 35/*DWMWA_CAPTION_COLOR*/, &captioncolor, sizeof(LONG));
	DwmSetWindowAttribute(hwnd, 36/*DWMWA_TEXT_COLOR*/, &textcolor, sizeof(LONG));

	// Create buttons

	hwndbtplay = CreateWindowExA(WS_EX_TRANSPARENT, "BUTTON", BTPLAY, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		UINT(398.0 * dpiScale), UINT(10.0 * dpiScale), UINT(100.0 * dpiScale), UINT(24.0 * dpiScale),
		hwnd, (HMENU)10, NULL, NULL);

	hwndbtquit = CreateWindowExA(WS_EX_TRANSPARENT, "BUTTON", BTQUIT, WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		UINT(526.0 * dpiScale), UINT(10.0 * dpiScale), UINT(100.0 * dpiScale), UINT(24.0 * dpiScale),
		hwnd, (HMENU)11, NULL, NULL);

	hwndComboBox = CreateWindowExA(
		WS_EX_TRANSPARENT,
		"COMBOBOX",	// Class name
		NULL,	// Window text
		CBS_DROPDOWNLIST | WS_CHILD | WS_VISIBLE,	// Styles
		UINT(14.0 * dpiScale),	// x position
		UINT(10.0 * dpiScale),	// y position
		UINT(200.0 * dpiScale),	// Width
		UINT(100.0 * dpiScale),	// Height
		hwnd,	// Parent window
		(HMENU)12,	// Menu
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), // Instance handle
		NULL	// Additional application data
	);

	// Add items to the combo box
	SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM)VIDMODE1);
	SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM)VIDMODE2);
	SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM)VIDMODE3);
	SendMessage(hwndComboBox, CB_ADDSTRING, 0, (LPARAM)VIDMODE4);

	SendMessage(hwndComboBox, CB_SETCURSEL, (WPARAM)vidmode, 0);

	// English Radio Button
	hwndEn = CreateWindowExW(
		WS_EX_TRANSPARENT,
		L"BUTTON",	// Predefined class; Unicode assumed 
		LANGEN,	// Button text 
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,	// Styles 
		UINT(10.0 * dpiScale),	// x position 
		UINT(300.0 * dpiScale),	// y position 
		UINT(200.0 * dpiScale),	// Button width
		UINT(30.0 * dpiScale),	// Button height
		hwnd,	// Parent window
		(HMENU)1,	// No menu.
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
		NULL);	// Pointer not needed.

	// French Radio Button
	hwndFr = CreateWindowExW(
		WS_EX_TRANSPARENT,
		L"BUTTON",
		LANGFR,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		UINT(10.0 * dpiScale),
		UINT(330.0 * dpiScale),
		UINT(200.0 * dpiScale),
		UINT(30.0 * dpiScale),
		hwnd,
		(HMENU)2,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
		NULL);

	// German Radio Button
	hwndDe = CreateWindowExW(
		WS_EX_TRANSPARENT,
		L"BUTTON",
		LANGDE,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		UINT(10.0 * dpiScale),
		UINT(360.0 * dpiScale),
		UINT(200.0 * dpiScale),
		UINT(30.0 * dpiScale),
		hwnd,
		(HMENU)3,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
		NULL);

	// Russian Radio Button
	hwndRu = CreateWindowExW(
		WS_EX_TRANSPARENT,
		L"BUTTON",
		LANGRU,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		UINT(10.0 * dpiScale),
		UINT(390.0 * dpiScale),
		UINT(200.0 * dpiScale),
		UINT(30.0 * dpiScale),
		hwnd,
		(HMENU)4,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
		NULL);

	// Spanish Radio Button
	hwndEs = CreateWindowExW(
		WS_EX_TRANSPARENT,
		L"BUTTON",
		LANGES,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		UINT(10.0 * dpiScale),
		UINT(420.0 * dpiScale),
		UINT(200.0 * dpiScale),
		UINT(30.0 * dpiScale),
		hwnd,
		(HMENU)5,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
		NULL);

	// Portuguese Radio Button
	hwndPt = CreateWindowExW(
		WS_EX_TRANSPARENT,
		L"BUTTON",
		LANGPT,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		UINT(10.0 * dpiScale),
		UINT(450.0 * dpiScale),
		UINT(200.0 * dpiScale),
		UINT(30.0 * dpiScale),
		hwnd,
		(HMENU)6,
		(HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), 
		NULL);

	SetWindowTheme(hwndEn, L"", L"");
	SetWindowTheme(hwndFr, L"", L"");
	SetWindowTheme(hwndDe, L"", L"");
	SetWindowTheme(hwndRu, L"", L"");
	SetWindowTheme(hwndEs, L"", L"");
	SetWindowTheme(hwndPt, L"", L"");

	if (userlang == 1)
		SendMessage(hwndFr, BM_SETCHECK, BST_CHECKED, 0);
	else if (userlang == 2)
		SendMessage(hwndDe, BM_SETCHECK, BST_CHECKED, 0);
	else if (userlang == 3)
		SendMessage(hwndRu, BM_SETCHECK, BST_CHECKED, 0);
	else if (userlang == 4)
		SendMessage(hwndEs, BM_SETCHECK, BST_CHECKED, 0);
	else if (userlang == 5)
		SendMessage(hwndPt, BM_SETCHECK, BST_CHECKED, 0);
	else
		SendMessage(hwndEn, BM_SETCHECK, BST_CHECKED, 0);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	// try and correct for the race condition that destroys the rendering of the radio text
	const std::array<HWND, 6> hwndArray = {
		hwndEn, hwndFr, hwndDe, hwndRu, hwndEs, hwndPt
	};
	for (const auto& hwnd : hwndArray) {
		ShowWindow(hwnd, SW_HIDE);
		ShowWindow(hwnd, SW_SHOW);
	}

	hFont = CreateFont(UINT(15.0 * dpiScale), 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
		CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Ariel"));

	if (hFont == NULL)
	{
		MessageBox(NULL, "Font Creation Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
	}

	SendMessage (hwndbtplay, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndComboBox, NULL, TRUE);
	SendMessage (hwndbtquit, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndComboBox, NULL, TRUE);
	SendMessage (hwndComboBox, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndComboBox, NULL, TRUE);
	SendMessage (hwndEn, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndEn, NULL, TRUE);
	SendMessage (hwndFr, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndFr, NULL, TRUE);
	SendMessage (hwndDe, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndDe, NULL, TRUE);
	SendMessage (hwndRu, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndRu, NULL, TRUE);
	SendMessage (hwndEs, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndEs, NULL, TRUE);
	SendMessage (hwndPt, WM_SETFONT, WPARAM (hFont), TRUE);
	InvalidateRect(hwndPt, NULL, TRUE);

	UpdateWindow(hwnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
