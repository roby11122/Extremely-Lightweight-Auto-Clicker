#include <windows.h>

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")

// minimal state
volatile LONG g_running = 0;
HANDLE g_timer = nullptr;
HANDLE g_thread = nullptr;
int g_interval = 100;
HWND g_hwnd = nullptr;
HWND g_edit = nullptr;
HWND g_btn = nullptr;
HWND g_hotkeyBtn = nullptr;
HWND g_hotkeyText = nullptr;
UINT g_hotkeyMod = 0;
UINT g_hotkeyVk = 0x77; // VK_F8
BOOL g_recording = FALSE;

// manual string functions logic (no CRT)
__forceinline int my_strlen(const wchar_t* s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

__forceinline void my_strcat(wchar_t* dst, const wchar_t* src) {
    int i = 0;
    while (dst[i]) i++;
    int j = 0;
    while (src[j]) {
        dst[i++] = src[j++];
    }
    dst[i] = 0;
}

__forceinline void my_itoa(UINT val, wchar_t* buf) {
    int i = 0;
    if (val == 0) {
        buf[i++] = L'0';
        buf[i] = 0;
        return;
    }
    wchar_t temp[16];
    int t = 0;
    while (val > 0) {
        temp[t++] = L'0' + (val % 10);
        val /= 10;
    }
    for (int j = t - 1; j >= 0; j--) {
        buf[i++] = temp[j];
    }
    buf[i] = 0;
}

__forceinline DWORD WINAPI ClickThread(LPVOID) {
    LARGE_INTEGER li;
    INPUT input = {};
    input.type = INPUT_MOUSE;
    
    while (g_running) {
        li.QuadPart = -(LONGLONG)(DWORD)g_interval * 10000LL;
        if (SetWaitableTimer(g_timer, &li, 0, nullptr, nullptr, FALSE)) {
            WaitForSingleObject(g_timer, INFINITE);
        }
        if (!g_running) break;
        
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
        SendInput(1, &input, sizeof(INPUT));
        input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
        SendInput(1, &input, sizeof(INPUT));
    }
    return 0;
}

__forceinline void StartClicker() {
    if (InterlockedCompareExchange(&g_running, 1L, 0L)) return;
    g_thread = CreateThread(nullptr, 0, ClickThread, nullptr, 0, nullptr);
    SetWindowTextW(g_btn, L"Stop");
}

__forceinline void SaveConfig() {
    HANDLE hFile = CreateFileW(L"c.ini", GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        DWORD written;
        wchar_t buf[64];
        int pos = 0;
        my_itoa(g_hotkeyMod, buf + pos);
        pos = my_strlen(buf);
        buf[pos++] = L'\n';
        my_itoa(g_hotkeyVk, buf + pos);
        pos = my_strlen(buf);
        buf[pos++] = L'\n';
        buf[pos] = 0;
        WriteFile(hFile, buf, (DWORD)(pos * 2), &written, nullptr);
        CloseHandle(hFile);
    }
}

__forceinline void LoadConfig() {
    HANDLE hFile = CreateFileW(L"c.ini", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile != INVALID_HANDLE_VALUE) {
        wchar_t buf[64];
        DWORD read;
        if (ReadFile(hFile, buf, 128, &read, nullptr)) {
            int chars = read / 2;
            buf[chars] = 0;
            wchar_t* p = buf;
            g_hotkeyMod = 0;
            while (*p >= L'0' && *p <= L'9') {
                g_hotkeyMod = g_hotkeyMod * 10 + (*p - L'0');
                p++;
            }
            p++;
            g_hotkeyVk = 0;
            while (*p >= L'0' && *p <= L'9') {
                g_hotkeyVk = g_hotkeyVk * 10 + (*p - L'0');
                p++;
            }
        }
        CloseHandle(hFile);
    }
}

__forceinline void UpdateHotkeyText() {
    wchar_t buf[32] = L"";
    if (g_hotkeyMod & MOD_CONTROL) my_strcat(buf, L"Ctrl+");
    if (g_hotkeyMod & MOD_SHIFT) my_strcat(buf, L"Shift+");
    if (g_hotkeyMod & MOD_ALT) my_strcat(buf, L"Alt+");
    if (g_hotkeyMod & MOD_WIN) my_strcat(buf, L"Win+");
    
    if (g_hotkeyVk >= '0' && g_hotkeyVk <= '9') {
        int len = my_strlen(buf);
        buf[len] = (wchar_t)g_hotkeyVk;
        buf[len + 1] = 0;
    } else if (g_hotkeyVk >= 'A' && g_hotkeyVk <= 'Z') {
        int len = my_strlen(buf);
        buf[len] = (wchar_t)g_hotkeyVk;
        buf[len + 1] = 0;
    } else if (g_hotkeyVk == 0x70) my_strcat(buf, L"F1");
    else if (g_hotkeyVk == 0x71) my_strcat(buf, L"F2");
    else if (g_hotkeyVk == 0x72) my_strcat(buf, L"F3");
    else if (g_hotkeyVk == 0x73) my_strcat(buf, L"F4");
    else if (g_hotkeyVk == 0x74) my_strcat(buf, L"F5");
    else if (g_hotkeyVk == 0x75) my_strcat(buf, L"F6");
    else if (g_hotkeyVk == 0x76) my_strcat(buf, L"F7");
    else if (g_hotkeyVk == 0x77) my_strcat(buf, L"F8");
    else if (g_hotkeyVk == 0x78) my_strcat(buf, L"F9");
    else if (g_hotkeyVk == 0x79) my_strcat(buf, L"F10");
    else if (g_hotkeyVk == 0x7A) my_strcat(buf, L"F11");
    else if (g_hotkeyVk == 0x7B) my_strcat(buf, L"F12");
    else if (g_hotkeyVk == VK_SPACE) my_strcat(buf, L"Space");
    else if (g_hotkeyVk == VK_RETURN) my_strcat(buf, L"Enter");
    else if (g_hotkeyVk == VK_ESCAPE) my_strcat(buf, L"Esc");
    else my_strcat(buf, L"?");
    
    SetWindowTextW(g_hotkeyText, buf);
}

__forceinline void StopClicker() {
    if (!InterlockedCompareExchange(&g_running, 0L, 1L)) return;
    if (g_timer) {
        LARGE_INTEGER li;
        li.QuadPart = -1LL;
        SetWaitableTimer(g_timer, &li, 0, nullptr, nullptr, FALSE);
    }
    if (g_thread) {
        WaitForSingleObject(g_thread, (DWORD)1000);
        CloseHandle(g_thread);
        g_thread = nullptr;
    }
    SetWindowTextW(g_btn, L"Start");
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wp) == 1) {
                wchar_t buf[16];
                GetWindowTextW(g_edit, buf, 16);
                int val = 0;
                for (int i = 0; buf[i] >= L'0' && buf[i] <= L'9'; i++) {
                    val = val * 10 + (buf[i] - L'0');
                }
                g_interval = val > 0 ? val : 1;
                if (g_running) {
                    StopClicker();
                    StartClicker();
                } else {
                    StartClicker();
                }
            } else if (LOWORD(wp) == 2) {
                g_recording = TRUE;
                SetWindowTextW(g_hotkeyText, L"...");
                SetFocus(hwnd);
            }
            break;
        case WM_KEYDOWN:
            if (g_recording) {
                g_recording = FALSE;
                UINT mod = 0;
                if (GetKeyState(VK_CONTROL) < 0) mod |= MOD_CONTROL;
                if (GetKeyState(VK_SHIFT) < 0) mod |= MOD_SHIFT;
                if (GetKeyState(VK_MENU) < 0) mod |= MOD_ALT;
                if (GetKeyState(VK_LWIN) < 0 || GetKeyState(VK_RWIN) < 0) mod |= MOD_WIN;
                
                UINT vk = (UINT)wp;
                if (vk != VK_CONTROL && vk != VK_SHIFT && vk != VK_MENU && vk != VK_LWIN && vk != VK_RWIN) {
                    UnregisterHotKey(hwnd, 1);
                    g_hotkeyMod = mod;
                    g_hotkeyVk = vk;
                    RegisterHotKey(hwnd, 1, g_hotkeyMod, g_hotkeyVk);
                    UpdateHotkeyText();
                } else {
                    UpdateHotkeyText();
                }
                return 0;
            }
            break;
        case WM_HOTKEY:
            if (g_running) StopClicker();
            else StartClicker();
            break;
        case WM_DESTROY:
            StopClicker();
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, wp, lp);
    }
    return 0;
}

void mainCRTStartup() {
    LoadConfig();
    g_timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
    
    HINSTANCE inst = GetModuleHandleW(nullptr);
    
    WNDCLASSW wc = {};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = inst;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"C";
    RegisterClassW(&wc);
    
    g_hwnd = CreateWindowExW(0, L"C", L"C", WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, 220, 150, nullptr, nullptr, inst, nullptr);
    
    CreateWindowExW(0, L"STATIC", L"ms:", WS_CHILD|WS_VISIBLE, 10, 12, 30, 20, g_hwnd, nullptr, inst, nullptr);
    g_edit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"100", WS_CHILD|WS_VISIBLE|ES_NUMBER,
        45, 10, 60, 24, g_hwnd, nullptr, inst, nullptr);
    g_btn = CreateWindowExW(0, L"BUTTON", L"Start", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        10, 50, 190, 32, g_hwnd, (HMENU)(UINT_PTR)1, inst, nullptr);
    
    CreateWindowExW(0, L"STATIC", L"HK:", WS_CHILD|WS_VISIBLE, 10, 88, 30, 20, g_hwnd, nullptr, inst, nullptr);
    g_hotkeyText = CreateWindowExW(0, L"STATIC", L"F8", WS_CHILD|WS_VISIBLE, 45, 88, 60, 20, g_hwnd, nullptr, inst, nullptr);
    g_hotkeyBtn = CreateWindowExW(0, L"BUTTON", L"Set", WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,
        110, 86, 90, 24, g_hwnd, (HMENU)(UINT_PTR)2, inst, nullptr);
    
    UpdateHotkeyText();
    RegisterHotKey(g_hwnd, 1, g_hotkeyMod, g_hotkeyVk);
    
    ShowWindow(g_hwnd, SW_SHOW);
    UpdateWindow(g_hwnd);
    
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnregisterHotKey(g_hwnd, 1);
    SaveConfig();
    if (g_timer) CloseHandle(g_timer);
    ExitProcess(0);
}
