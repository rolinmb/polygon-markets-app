#include <windows.h>

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

void RunGoProgram(const std::string& mode, const std::string& asset, const std::string& from, const std::string& to);