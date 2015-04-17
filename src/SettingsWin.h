/**
 *  \file
 *  \brief  GTags settings window
 *
 *  \author  Pavel Nedev <pg.nedev@gmail.com>
 *
 *  \section COPYRIGHT
 *  Copyright(C) 2015 Pavel Nedev
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


#pragma once


#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>


namespace GTags
{

struct Settings;


/**
 *  \class  SettingsWin
 *  \brief
 */
class SettingsWin
{
public:
    static void Show(HWND hOwner, Settings* _settings);

private:
    static const TCHAR cClassName[];
    static const TCHAR cHeader[];
    static const int cBackgroundColor;
    static const TCHAR cFont[];
    static const int cFontSize;

    static LRESULT APIENTRY wndProc(HWND hwnd, UINT umsg,
            WPARAM wparam, LPARAM lparam);
    static RECT adjustSizeAndPos(HWND hOwner, DWORD styleEx, DWORD style,
            int width, int height);

    SettingsWin(Settings* settings) : _settings(settings) {}
    SettingsWin(const SettingsWin&);
    ~SettingsWin();

    HWND composeWindow(HWND hOwner);

    void onOK(HWND hWnd);

    Settings* _settings;
    HWND _hParser;
    HWND _hAutoUpdate;
    HWND _hLibraryDBs;
    HWND _hOK;
    HWND _hCancel;
    HFONT _hFont;
};

} // namespace GTags
