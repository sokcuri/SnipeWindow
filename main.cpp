#include "main.h"
#include "SnipeWindow.h"
#include "resource.h"

char		szMainWindowClassName[] = "SnipeWindowMain";
HINSTANCE	g_hInst = NULL;
HWND		g_hwndMainWnd = NULL;
HANDLE		g_hApplicationMutex = NULL;
DWORD		g_dwLastError = 0;
BOOL		g_bStartSearchWindow = FALSE;
HCURSOR		g_hCursorSearchWindow = NULL;
HCURSOR		g_hCursorPrevious = NULL;
HBITMAP		g_hBitmapFinderToolFilled;
HBITMAP		g_hBitmapFinderToolEmpty;
HWND		g_hwndFoundWindow = NULL;
HPEN		g_hRectanglePen = NULL;

BOOL InitializeApplication(HINSTANCE hThisInst, HINSTANCE hPrevInst, LPTSTR lpszArgs, int nWinMode)
{
	WNDCLASS wc;
	BOOL bRetTemp = FALSE;
	long lRetTemp = 0;
	BOOL bRet = FALSE;
	g_hApplicationMutex = CreateMutex(NULL, TRUE, WINDOW_FINDER_APP_MUTEX_NAME);
	g_dwLastError = GetLastError();

	if (g_hApplicationMutex == NULL)
	{
		bRet = FALSE;
		goto InitializeApplication_0;
	}

	if (g_dwLastError == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(g_hApplicationMutex);
		g_hApplicationMutex = NULL;
		bRet = FALSE;
		goto InitializeApplication_0;
	}

	memset(&wc, 0, sizeof(WNDCLASS));

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)MainWndProc;
	wc.hInstance = hThisInst;
	wc.hIcon = LoadIcon((HINSTANCE)NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor((HINSTANCE)NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = (LPCTSTR)MAKEINTRESOURCE(IDR_MENU_MAIN);
	wc.lpszClassName = szMainWindowClassName;

	if (!RegisterClass(&wc))
	{
		bRet = FALSE;
		goto InitializeApplication_0;
	}

	// All went well, return a TRUE.
	bRet = TRUE;

InitializeApplication_0:

	return bRet;
}

BOOL UninitializeApplication()
{
	BOOL bRet = FALSE;

	bRet = UnregisterClass(szMainWindowClassName, g_hInst);

	if (g_hApplicationMutex)
	{
		ReleaseMutex(g_hApplicationMutex);
		CloseHandle(g_hApplicationMutex);
		g_hApplicationMutex = NULL;
	}
	return bRet;
}

BOOL InitialiseResources()
{
	BOOL bRet = FALSE;

	g_hCursorSearchWindow = LoadCursor(g_hInst, MAKEINTRESOURCE(IDC_CURSOR_SEARCH_WINDOW));
	if (g_hCursorSearchWindow == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hRectanglePen = CreatePen(PS_SOLID, 3, RGB(256, 0, 0));
	if (g_hRectanglePen == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hBitmapFinderToolFilled = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP_FINDER_FILLED));
	if (g_hBitmapFinderToolFilled == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	g_hBitmapFinderToolEmpty = LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP_FINDER_EMPTY));
	if (g_hBitmapFinderToolEmpty == NULL)
	{
		bRet = FALSE;
		goto InitialiseResources_0;
	}

	bRet = TRUE;

InitialiseResources_0:

	return bRet;
}





BOOL UninitialiseResources()
{
	BOOL bRet = TRUE;

	if (g_hRectanglePen)
	{
		bRet = DeleteObject(g_hRectanglePen);
		g_hRectanglePen = NULL;
	}

	if (g_hBitmapFinderToolFilled)
	{
		DeleteObject(g_hBitmapFinderToolFilled);
		g_hBitmapFinderToolFilled = NULL;
	}

	if (g_hBitmapFinderToolEmpty)
	{
		DeleteObject(g_hBitmapFinderToolEmpty);
		g_hBitmapFinderToolEmpty = NULL;
	}

	return bRet;
}





int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MSG msg;
	BOOL bRet = FALSE;
	int iRet = 0;
	DWORD dwStyle = 0;

	g_hInst = hInstance;

	bRet = InitializeApplication(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
	if (bRet == FALSE)
	{
		iRet = 0;
		goto WinMain_0;
	}

	bRet = InitialiseResources();
	if (bRet == FALSE)
	{
		iRet = 0;
		goto WinMain_0;
	}


	dwStyle = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	int width = 500, height = 200, x, y;
	RECT rect;
	GetClientRect(GetDesktopWindow(), &rect);
	x = (rect.right - width) / 2;
	y = (rect.bottom - height) / 2;

	while (1)
	{
		StartSearchWindowDialog(0);
	}
	return 0;

WinMain_0:
	UninitializeApplication();
	UninitialiseResources();

	return iRet;
}

// The window procedure.
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	BOOL bCallDefWndProc = FALSE;
	long lRet = 0;

	switch (message)
	{
	case WM_COMMAND:
	{
		WORD wNotifyCode = HIWORD(wParam);
		WORD wID = LOWORD(wParam);
		HWND hwndCtl = (HWND)lParam;

		if (wNotifyCode == 0)
		{
			// Message is from a menu.
			if (wID == IDM_ABOUT)
			{
				MessageBox(NULL, ABOUT_WINDOW_FINDER, "Window Sniper", MB_OK);
			}

			if (wID == IDM_FIND_WINDOW)
			{
				PostMessage(hwnd, WM_START_SEARCH_WINDOW, 0, 0);
			}

			if (wID == IDM_EXIT)
			{
				PostMessage(hwnd, WM_CLOSE, 0, 0);
			}

			lRet = 0;
			bCallDefWndProc = FALSE;
		}
		else
		{
			bCallDefWndProc = TRUE;
		}

		break;
	}

	case WM_DESTROY: // terminate the program 
	{
		lRet = 0;
		bCallDefWndProc = FALSE;
		PostQuitMessage(0);
		break;
	}

	case WM_START_SEARCH_WINDOW:
	{
		lRet = 0;
		bCallDefWndProc = FALSE;

		StartSearchWindowDialog(hwnd);

		break;
	}

	default:
	{
		// Let Windows process any messages not specified in the preceding switch statement.
		bCallDefWndProc = TRUE;
		break;
	}
	}

	if (bCallDefWndProc)
	{
		return DefWindowProc(hwnd, message, wParam, lParam);
	}
	else
	{
		return lRet;
	}
}

