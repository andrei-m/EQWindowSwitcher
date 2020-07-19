#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <winuser.h>

//TODO: store this state in CreateWindowEx instead: https://docs.microsoft.com/en-us/windows/win32/learnwin32/managing-application-state-
HWND* windowHandles;
//TODO: DRY for maximum hot key/window count
const int hotKeyIDs[4] = { 0, 1, 2, 3 };
// see https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes for key codes
const UINT hotKeys[4] = { 0x31, 0x32, 0x33, 0x34 };


HWND* getEQWindowHandles()
{
    int i = 0;
    static HWND result[10];
    HWND previous = NULL;
    while (true) {
        //TODO: make window finding criteria configurable
        previous = FindWindowEx(NULL, previous, NULL, L"EverQuest");
        if (previous == NULL) {
            break;
        }
        result[i] = previous;
        i++;
    }
    return result;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"eqWindowSwitcher";

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0,
        L"eqWindowSwitcher",
        L"EQWindowSwitcher",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (hwnd == NULL)
    {
        return 0;
    }

    windowHandles = getEQWindowHandles();
    for (int i = 0; i < 4; i++) {
        if (*(windowHandles + i) == NULL) {
            break;
        }

        if (RegisterHotKey(
            hwnd,
            hotKeyIDs[i],
            MOD_ALT | MOD_SHIFT | MOD_NOREPEAT,
            hotKeys[i]))
        {
            OutputDebugStringW(L"Hotkey registered\n");
        }
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_HOTKEY:
        for (int i = 0; i < 4; i++) {
            if (wParam == hotKeyIDs[i]) {
                SetForegroundWindow(*(windowHandles + i));
                break;
            }
        }

        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);
    }
    return 0;

    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

