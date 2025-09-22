#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "main.h"

HWND hComboMode, hEditAsset, hEditFrom, hEditTo, hButton;

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
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 300,
        NULL, NULL, hInstance, NULL
    );

    if (!hwnd) return 0;

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

    // Mutable buffer
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
        // Mode label + combo box
        CreateWindowA("STATIC", "Mode:", WS_VISIBLE | WS_CHILD,
            10, 10, 100, 20, hwnd, NULL, NULL, NULL);

        hComboMode = CreateWindowA("COMBOBOX", "",
            WS_VISIBLE | WS_CHILD | CBS_DROPDOWNLIST,
            120, 10, 200, 100, hwnd, NULL, NULL, NULL);

        // Add items
        SendMessageA(hComboMode, CB_ADDSTRING, 0, (LPARAM)"Equities");
        SendMessageA(hComboMode, CB_ADDSTRING, 0, (LPARAM)"Foreign Exchange");
        SendMessageA(hComboMode, CB_ADDSTRING, 0, (LPARAM)"Cryptocurrency");
        SendMessageA(hComboMode, CB_SETCURSEL, 0, 0); // default = Equities

        // Asset
        CreateWindowA("STATIC", "Asset:", WS_VISIBLE | WS_CHILD,
            10, 40, 100, 20, hwnd, NULL, NULL, NULL);
        hEditAsset = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            120, 40, 200, 20, hwnd, NULL, NULL, NULL);

        // From date
        CreateWindowA("STATIC", "From (YYYY-MM-DD):", WS_VISIBLE | WS_CHILD,
            10, 70, 200, 20, hwnd, NULL, NULL, NULL);
        hEditFrom = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            230, 70, 180, 20, hwnd, NULL, NULL, NULL);

        // To date
        CreateWindowA("STATIC", "To (YYYY-MM-DD):", WS_VISIBLE | WS_CHILD,
            10, 100, 200, 20, hwnd, NULL, NULL, NULL);
        hEditTo = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            230, 100, 180, 20, hwnd, NULL, NULL, NULL);

        // Button
        hButton = CreateWindowA("BUTTON", "Fetch Data", WS_VISIBLE | WS_CHILD,
            200, 150, 150, 30, hwnd, (HMENU)1, NULL, NULL);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            int sel = (int)SendMessageA(hComboMode, CB_GETCURSEL, 0, 0);

            // Map selection index -> mode number string
            std::string mode;
            if (sel == 0) mode = "0";
            else if (sel == 1) mode = "1";
            else if (sel == 2) mode = "2";
            else mode = "0"; // default fallback

            char bufAsset[8];
            char bufFrom[11];
            char bufTo[11];

            GetWindowTextA(hEditAsset, bufAsset, sizeof(bufAsset));
            GetWindowTextA(hEditFrom, bufFrom, sizeof(bufFrom));
            GetWindowTextA(hEditTo, bufTo, sizeof(bufTo));

            RunGoProgram(mode, bufAsset, bufFrom, bufTo);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}
