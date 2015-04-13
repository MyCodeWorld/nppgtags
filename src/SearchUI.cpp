/**
 *  \file
 *  \brief  Search input window
 *
 *  \author  Pavel Nedev <pg.nedev@gmail.com>
 *
 *  \section COPYRIGHT
 *  Copyright(C) 2014-2015 Pavel Nedev
 *
 *  \section LICENSE
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#pragma comment (lib, "comctl32")


#define WIN32_LEAN_AND_MEAN
#include "SearchUI.h"
#include <windowsx.h>
#include <commctrl.h>
#include <richedit.h>
#include "Cmd.h"


namespace GTags
{

const TCHAR SearchUI::cClassName[]      = _T("SearchUI");
const int SearchUI::cBackgroundColor    = COLOR_INFOBK;
const TCHAR SearchUI::cBtnFont[]        = _T("Tahoma");


HINSTANCE SearchUI::HMod = NULL;


/**
 *  \brief
 */
void SearchUI::Register(HINSTANCE hMod)
{
    HMod = hMod;
    if (hMod == NULL)
        GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_PIN, cClassName, &HMod);

    WNDCLASS wc         = {0};
    wc.style            = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc      = wndProc;
    wc.hInstance        = HMod;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = GetSysColorBrush(cBackgroundColor);
    wc.lpszClassName    = cClassName;

    RegisterClass(&wc);

    INITCOMMONCONTROLSEX icex   = {0};
    icex.dwSize                 = sizeof(icex);
    icex.dwICC                  = ICC_STANDARD_CLASSES;

    InitCommonControlsEx(&icex);
    LoadLibrary(_T("Riched20.dll"));
}


/**
 *  \brief
 */
void SearchUI::Unregister()
{
    UnregisterClass(cClassName, HMod);
    FreeLibrary(GetModuleHandle(_T("Riched20.dll")));
}


/**
 *  \brief
 */
bool SearchUI::Show(HWND hOwner,
        const TCHAR* font, unsigned fontSize, int width,
        const TCHAR* header, SearchData* searchData)
{
    if (!searchData)
        return false;

    SearchUI iow(hOwner, width, searchData);
    if (iow.composeWindow(font, fontSize, width, header, searchData) == NULL)
        return false;

    BOOL r;
    MSG msg;
    while ((r = GetMessage(&msg, NULL, 0, 0)) != 0 && r != -1)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return iow._success;
}


/**
 *  \brief
 */
RECT SearchUI::adjustSizeAndPos(DWORD styleEx, DWORD style,
        int width, int height)
{
    if (width <= 0) width = 200;

    RECT maxWin;
    GetWindowRect(GetDesktopWindow(), &maxWin);

    POINT center;
    if (_hOwner)
    {
        RECT biasWin;
        GetWindowRect(_hOwner, &biasWin);
        center.x = (biasWin.right + biasWin.left) / 2;
        center.y = (biasWin.bottom + biasWin.top) / 2;
    }
    else
    {
        center.x = (maxWin.right + maxWin.left) / 2;
        center.y = (maxWin.bottom + maxWin.top) / 2;
    }

    RECT win = {0};
    win.right = width;
    win.bottom = height;

    AdjustWindowRectEx(&win, style, FALSE, styleEx);

    width = win.right - win.left;
    height = win.bottom - win.top;

    if (width < maxWin.right - maxWin.left)
    {
        win.left = center.x - width / 2;
        if (win.left < maxWin.left) win.left = maxWin.left;
        win.right = win.left + width;
    }
    else
    {
        win.left = maxWin.left;
        win.right = maxWin.right;
    }

    if (height < maxWin.bottom - maxWin.top)
    {
        win.top = center.y - height / 2;
        if (win.top < maxWin.top) win.top = maxWin.top;
        win.bottom = win.top + height;
    }
    else
    {
        win.top = maxWin.top;
        win.bottom = maxWin.bottom;
    }

    return win;
}


/**
 *  \brief
 */
SearchUI::~SearchUI()
{
    if (_hBtnFont)
        DeleteObject(_hBtnFont);
    if (_hTxtFont)
        DeleteObject(_hTxtFont);
}


/**
 *  \brief
 */
HWND SearchUI::composeWindow(const TCHAR* font, unsigned fontSize,
        int width, const TCHAR* header, const SearchData* searchData)
{
    TEXTMETRIC tm;
    HDC hdc = GetWindowDC(_hOwner);
    GetTextMetrics(hdc, &tm);
    int txtHeight = MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72) +
            tm.tmInternalLeading;
    int btnHeight = MulDiv(fontSize - 1, GetDeviceCaps(hdc, LOGPIXELSY), 72) +
            tm.tmInternalLeading;
    _hTxtFont = CreateFont(
            -MulDiv(fontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72),
            0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            FF_DONTCARE | DEFAULT_PITCH, font);
    _hBtnFont = CreateFont(
            -MulDiv(fontSize - 1, GetDeviceCaps(hdc, LOGPIXELSY), 72),
            0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET,
            OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
            FF_DONTCARE | DEFAULT_PITCH, cBtnFont);
    ReleaseDC(_hOwner, hdc);

    DWORD styleEx = WS_EX_OVERLAPPEDWINDOW | WS_EX_TOOLWINDOW;
    DWORD style = WS_POPUP | WS_CAPTION;
    RECT win = adjustSizeAndPos(styleEx, style, width,
            txtHeight + btnHeight + 6);
    width = win.right - win.left;
    _hWnd = CreateWindowEx(styleEx, cClassName, header,
            style, win.left, win.top, width, win.bottom - win.top,
            _hOwner, NULL, HMod, (LPVOID) this);
    if (_hWnd == NULL)
        return NULL;

    GetClientRect(_hWnd, &win);
    width = win.right - win.left;

    style = WS_CHILD | WS_VISIBLE | WS_HSCROLL;
    _hEditWnd = CreateWindowEx(0, RICHEDIT_CLASS, NULL, style,
            0, 0, width, txtHeight,
            _hWnd, NULL, HMod, NULL);

    width = (width - 12) / 3;

    _hRegExp = CreateWindowEx(0, _T("BUTTON"), _T("RegExp"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            3, txtHeight + 3, width, btnHeight,
            _hWnd, NULL, HMod, NULL);

    _hMatchCase = CreateWindowEx(0, _T("BUTTON"), _T("MatchCase"),
            WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
            width + 6, txtHeight + 3, width, btnHeight,
            _hWnd, NULL, HMod, NULL);

    _hOK = CreateWindowEx(0, _T("BUTTON"), _T("OK"),
            WS_CHILD | WS_VISIBLE | BS_TEXT | BS_DEFPUSHBUTTON,
            2 * width + 9, txtHeight + 3, width, btnHeight,
            _hWnd, NULL, HMod, NULL);

    SendMessage(_hEditWnd, EM_SETBKGNDCOLOR, 0,
            (LPARAM)GetSysColor(cBackgroundColor));

    CHARFORMAT fmt  = {0};
    fmt.cbSize      = sizeof(fmt);
    fmt.dwMask      = CFM_FACE | CFM_BOLD | CFM_ITALIC | CFM_SIZE;
    fmt.dwEffects   = CFE_AUTOCOLOR;
    fmt.yHeight     = fontSize * 20;
    if (font)
        _tcscpy_s(fmt.szFaceName, _countof(fmt.szFaceName), font);
    SendMessage(_hEditWnd, EM_SETCHARFORMAT, (WPARAM)SCF_ALL, (LPARAM)&fmt);

    if (_hTxtFont)
        SendMessage(_hEditWnd, WM_SETFONT, (WPARAM)_hTxtFont, (LPARAM)TRUE);

    DWORD events = ENM_KEYEVENTS;

    SendMessage(_hEditWnd, EM_EXLIMITTEXT, 0, (LPARAM)(cMaxTagLen - 1));
    SendMessage(_hEditWnd, EM_SETEVENTMASK, 0, (LPARAM)events);

    if (_tcslen(searchData->_str))
    {
        Edit_SetText(_hEditWnd, searchData->_str);
        Edit_SetSel(_hEditWnd, 0, -1);
    }

    if (_hBtnFont)
    {
        SendMessage(_hRegExp, WM_SETFONT, (WPARAM)_hBtnFont, (LPARAM)TRUE);
        SendMessage(_hMatchCase, WM_SETFONT, (WPARAM)_hBtnFont, (LPARAM)TRUE);
    }

    Button_SetCheck(_hRegExp, searchData->_regExp ?
            BST_CHECKED : BST_UNCHECKED);
    Button_SetCheck(_hMatchCase, searchData->_matchCase ?
            BST_CHECKED : BST_UNCHECKED);

    ShowWindow(_hWnd, SW_SHOWNORMAL);
    UpdateWindow(_hWnd);

    return _hWnd;
}


/**
 *  \brief
 */
void SearchUI::onOK()
{
    int len = Edit_GetTextLength(_hEditWnd) + 1;
    if (len > 1)
    {
        Edit_GetText(_hEditWnd, _searchData->_str, len);
        _searchData->_regExp =
                (Button_GetCheck(_hRegExp) == BST_CHECKED) ?
                true : false;
        _searchData->_matchCase =
                (Button_GetCheck(_hMatchCase) == BST_CHECKED) ?
                true : false;
        _success = true;
    }
    SendMessage(_hWnd, WM_CLOSE, 0, 0);
}


/**
 *  \brief
 */
int SearchUI::onKeyDown(DWORD key)
{
    switch (key)
    {
        case VK_ESCAPE:
        case VK_TAB:
            SendMessage(_hWnd, WM_CLOSE, 0, 0);
            return 1;

        case VK_RETURN:
            onOK();
            return 1;
    }

    return 0;
}


/**
 *  \brief
 */
LRESULT APIENTRY SearchUI::wndProc(HWND hwnd, UINT umsg,
        WPARAM wparam, LPARAM lparam)
{
    switch (umsg)
    {
        case WM_CREATE:
        {
            SearchUI* iow =
                    (SearchUI*)((LPCREATESTRUCT)lparam)->lpCreateParams;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, PtrToUlong(iow));
            return 0;
        }

        case WM_SETFOCUS:
        {
            SearchUI* iow =
                    reinterpret_cast<SearchUI*>(static_cast<LONG_PTR>
                            (GetWindowLongPtr(hwnd, GWLP_USERDATA)));
            SetFocus(iow->_hEditWnd);
            return 0;
        }

        case WM_COMMAND:
            if (HIWORD(wparam) == EN_KILLFOCUS)
            {
                DestroyCaret();
                return 0;
            }
            else if (HIWORD(wparam) == BN_CLICKED)
            {
                SearchUI* iow =
                        reinterpret_cast<SearchUI*>(static_cast<LONG_PTR>
                                (GetWindowLongPtr(hwnd, GWLP_USERDATA)));
                if ((HWND)lparam == iow->_hOK)
                {
                    iow->onOK();
                    return 0;
                }
            }
        break;

        case WM_NOTIFY:
            switch (((LPNMHDR)lparam)->code)
            {
                case EN_MSGFILTER:
                {
                    SearchUI* iow =
                            reinterpret_cast<SearchUI*>(static_cast<LONG_PTR>
                                    (GetWindowLongPtr(hwnd, GWLP_USERDATA)));
                    MSGFILTER* pMsgFilter = (MSGFILTER*)lparam;
                    if (pMsgFilter->msg == WM_KEYDOWN)
                        return iow->onKeyDown(pMsgFilter->wParam);
                    return 0;
                }
            }
        break;

        case WM_DESTROY:
        {
            SearchUI* iow =
                    reinterpret_cast<SearchUI*>(static_cast<LONG_PTR>
                            (GetWindowLongPtr(hwnd, GWLP_USERDATA)));
            DestroyCaret();
            PostQuitMessage(0);
            return 0;
        }
    }

    return DefWindowProc(hwnd, umsg, wparam, lparam);
}

} // namespace GTags