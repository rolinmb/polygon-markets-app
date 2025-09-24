#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "main.h"

// Link with modern common controls
#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND hComboMode, hEditAsset, hEditFrom, hEditTo, hButton;
HFONT hFont; // global font

// Forward declare
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    const char CLASS_NAME[] = "polygon-markets-app";

    WNDCLASSA wc = {};
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "polygon-markets-app",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 650, 280,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

    // Create modern Segoe UI font
    hFont = CreateFontA(
        -16, 0, 0, 0, FW_NORMAL,
        FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        "Segoe UI"
    );

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {};
    while (GetMessageA(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return 0;
}

void RunGoProgram(const std::string& mode,
                  const std::string& asset,
                  const std::string& from,
                  const std::string& to)
{
    std::ostringstream cmd;
    cmd << "go run main.go "
        << mode << " "
        << asset << " "
        << from << " "
        << to;

    std::string command = cmd.str();

    std::vector<char> cmdline(command.begin(), command.end());
    cmdline.push_back('\0');

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(
            NULL,
            cmdline.data(),
            NULL, NULL, TRUE,
            0, NULL, NULL, &si, &pi))
    {
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
    }
    else {
        MessageBoxA(NULL, "Failed to run Go program", "Error", MB_OK | MB_ICONERROR);
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_CREATE: {
        // Mode label
        HWND hStaticMode = CreateWindowA("STATIC", "Mode:", WS_VISIBLE | WS_CHILD,
            10, 10, 100, 20, hwnd, NULL, NULL, NULL);
        SendMessageA(hStaticMode, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Combo box
        hComboMode = CreateWindowA("COMBOBOX", "",
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST | WS_VSCROLL,
            120, 10, 200, 200, hwnd, NULL, NULL, NULL);
        SendMessageA(hComboMode, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Add items
        SendMessageA(hComboMode, CB_ADDSTRING, 0, (LPARAM)"Equities");
        SendMessageA(hComboMode, CB_ADDSTRING, 0, (LPARAM)"Foreign Exchange");
        SendMessageA(hComboMode, CB_ADDSTRING, 0, (LPARAM)"Cryptocurrency");
        SendMessageA(hComboMode, CB_ADDSTRING, 0, (LPARAM)"Options");
        SendMessageA(hComboMode, CB_SETCURSEL, 0, 0);

        // Asset
        HWND hStaticAsset = CreateWindowA("STATIC", "Asset:", WS_VISIBLE | WS_CHILD,
            10, 50, 100, 20, hwnd, NULL, NULL, NULL);
        SendMessageA(hStaticAsset, WM_SETFONT, (WPARAM)hFont, TRUE);

        hEditAsset = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            120, 50, 200, 22, hwnd, NULL, NULL, NULL);
        SendMessageA(hEditAsset, WM_SETFONT, (WPARAM)hFont, TRUE);

        // From date
        HWND hStaticFrom = CreateWindowA("STATIC", "From (YYYY-MM-DD):", WS_VISIBLE | WS_CHILD,
            10, 90, 200, 20, hwnd, NULL, NULL, NULL);
        SendMessageA(hStaticFrom, WM_SETFONT, (WPARAM)hFont, TRUE);

        hEditFrom = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            230, 90, 180, 22, hwnd, NULL, NULL, NULL);
        SendMessageA(hEditFrom, WM_SETFONT, (WPARAM)hFont, TRUE);

        // To date
        HWND hStaticTo = CreateWindowA("STATIC", "To (YYYY-MM-DD):", WS_VISIBLE | WS_CHILD,
            10, 130, 200, 20, hwnd, NULL, NULL, NULL);
        SendMessageA(hStaticTo, WM_SETFONT, (WPARAM)hFont, TRUE);

        hEditTo = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            230, 130, 180, 22, hwnd, NULL, NULL, NULL);
        SendMessageA(hEditTo, WM_SETFONT, (WPARAM)hFont, TRUE);

        // Button
        hButton = CreateWindowA("BUTTON", "Fetch Data", WS_VISIBLE | WS_CHILD,
            220, 180, 150, 35, hwnd, (HMENU)1, NULL, NULL);
        SendMessageA(hButton, WM_SETFONT, (WPARAM)hFont, TRUE);

        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            int sel = (int)SendMessageA(hComboMode, CB_GETCURSEL, 0, 0);

            std::string mode;
            if (sel == 0) mode = "0";
            else if (sel == 1) mode = "1";
            else if (sel == 2) mode = "2";
            else if (sel == 3) mode = "3";
            else mode = "0";

            char bufAsset[64];
            char bufFrom[64];
            char bufTo[64];

            GetWindowTextA(hEditAsset, bufAsset, sizeof(bufAsset));
            GetWindowTextA(hEditFrom, bufFrom, sizeof(bufFrom));
            GetWindowTextA(hEditTo, bufTo, sizeof(bufTo));

            RunGoProgram(mode, bufAsset, bufFrom, bufTo);
        }
        break;
    case WM_DESTROY:
        DeleteObject(hFont); // cleanup
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}
