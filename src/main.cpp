#include <windows.h>
#include <string>
#include <sstream>
#include <vector>
#include "main.h"

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
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 300,   // wider and taller
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

HWND hEditMode, hEditAsset, hEditFrom, hEditTo, hButton;

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

    // Create mutable buffer (copy into vector<char>)
    std::vector<char> cmdline(command.begin(), command.end());
    cmdline.push_back('\0');  // null terminator

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    if (CreateProcessA(
            NULL,
            cmdline.data(),  // now mutable char*
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
        CreateWindowA("STATIC", "Mode:", WS_VISIBLE | WS_CHILD,
            10, 10, 100, 20, hwnd, NULL, NULL, NULL);
        hEditMode = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            120, 10, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "Asset:", WS_VISIBLE | WS_CHILD,
            10, 40, 100, 20, hwnd, NULL, NULL, NULL);
        hEditAsset = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            120, 40, 200, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "From (YYYY-MM-DD):", WS_VISIBLE | WS_CHILD,
            10, 70, 200, 20, hwnd, NULL, NULL, NULL);
        hEditFrom = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            230, 70, 180, 20, hwnd, NULL, NULL, NULL);

        CreateWindowA("STATIC", "To (YYYY-MM-DD):", WS_VISIBLE | WS_CHILD,
            10, 100, 200, 20, hwnd, NULL, NULL, NULL);
        hEditTo = CreateWindowA("EDIT", "", WS_VISIBLE | WS_CHILD | WS_BORDER,
            230, 100, 180, 20, hwnd, NULL, NULL, NULL);

        hButton = CreateWindowA("BUTTON", "Fetch Data", WS_VISIBLE | WS_CHILD,
            120, 140, 150, 30, hwnd, (HMENU)1, NULL, NULL);
        break;
    }
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            char bufMode[2];     // plenty for "0".."99"
            char bufAsset[8];    // up to 7 letters + '\0'
            char bufFrom[11];    // 10 chars date + '\0'
            char bufTo[11];      // same
            GetWindowTextA(hEditMode, bufMode, 64);
            GetWindowTextA(hEditAsset, bufAsset, 64);
            GetWindowTextA(hEditFrom, bufFrom, 64);
            GetWindowTextA(hEditTo, bufTo, 64);

            RunGoProgram(bufMode, bufAsset, bufFrom, bufTo);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProcA(hwnd, uMsg, wParam, lParam);
}
