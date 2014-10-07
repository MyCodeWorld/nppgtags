/**
 *  \file
 *  \brief  Various helper types and functions
 *
 *  \author  Pavel Nedev <pg.nedev@gmail.com>
 *
 *  \section COPYRIGHT
 *  Copyright(C) 2014 Pavel Nedev
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


#define WIN32_LEAN_AND_MEAN
#include "Common.h"
#include <stdlib.h>
#include <string.h>


/**
 *  \brief
 */
unsigned CPath::StripFilename()
{
    int len;

    for (len = _tcslen(_str) - 1; len >= 0; len--)
        if (_str[len] == _T('\\') || _str[len] == _T('/'))
            break;
    _str[++len] = 0;

    return (unsigned)len;
}


/**
 *  \brief
 */
const TCHAR* CPath::GetFilename_C_str() const
{
    int len;

    for (len = _tcslen(_str) - 1; len >= 0; len--)
        if (_str[len] == _T('\\') || _str[len] == _T('/'))
            break;

    return &_str[++len];
}


/**
 *  \brief
 */
unsigned CPath::Up()
{
    int len = _tcslen(_str) - 1;
    if (_str[len] == _T('\\') || _str[len] == _T('/'))
        len--;

    for (; len >= 0; len--)
        if (_str[len] == _T('\\') || _str[len] == _T('/'))
            break;
    _str[++len] = 0;

    return (unsigned)len;
}


/**
 *  \brief
 */
bool CPath::Contains(const TCHAR* pathStr) const
{
    unsigned len = _tcslen(_str);
    if (len >= _tcslen(pathStr))
        return false;

    return !_tcsncmp(_str, pathStr, len);
}


/**
 *  \brief
 */
bool CPath::Contains(const CPath& path) const
{
    unsigned len = _tcslen(_str);
    if (len >= path.Len())
        return false;

    return !_tcsncmp(_str, path._str, len);
}


/**
 *  \brief
 */
bool CPath::IsContainedIn(const TCHAR* pathStr) const
{
    unsigned len = _tcslen(pathStr);
    if (len >= _tcslen(_str))
        return false;

    return !_tcsncmp(_str, pathStr, len);
}


/**
 *  \brief
 */
bool CPath::IsContainedIn(const CPath& path) const
{
    unsigned len = path.Len();
    if (len >= _tcslen(_str))
        return false;

    return !_tcsncmp(_str, path._str, len);
}


/**
 *  \brief
 */
CText::CText(const TCHAR* str) : _size(ALLOC_CHUNK_SIZE), _str(_buf)
{
    _buf[0] = 0;
    if (str)
    {
        unsigned len = _tcslen(str);
        if (len >= _size)
        {
            _size = (len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
            _str = new TCHAR[_size];
        }
        _tcscpy_s(_str, _size, str);
    }
}


/**
 *  \brief
 */
CText::CText(const char* str) : _size(ALLOC_CHUNK_SIZE), _str(_buf)
{
    _buf[0] = 0;
    if (str)
    {
        unsigned len = strlen(str);
        if (len >= _size)
        {
            _size = (len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
            _str = new TCHAR[_size];
        }
        size_t cnt;
        mbstowcs_s(&cnt, _str, _size, str, _TRUNCATE);
    }
}


/**
 *  \brief
 */
CText::CText(const CText& txt) : _size(ALLOC_CHUNK_SIZE), _str(_buf)
{
    _buf[0] = 0;
    if (_size < txt._size)
    {
        _size = txt._size;
        _str = new TCHAR[_size];
    }
    _tcscpy_s(_str, _size, txt._str);
}


/**
 *  \brief
 */
const CText& CText::operator=(const TCHAR* str)
{
    if (str)
    {
        resize(_tcslen(str));
        _tcscpy_s(_str, _size, str);
    }

    return *this;
}


/**
 *  \brief
 */
const CText& CText::operator=(const char* str)
{
    if (str)
    {
        resize(strlen(str));
        size_t cnt;
        mbstowcs_s(&cnt, _str, _size, str, _TRUNCATE);
    }

    return *this;
}


/**
 *  \brief
 */
const CText& CText::operator=(const CText& txt)
{
    if (this != &txt)
    {
        resize(txt.Len());
        _tcscpy_s(_str, _size, txt._str);
    }

    return *this;
}


/**
 *  \brief
 */
const CText& CText::operator+=(const TCHAR* str)
{
    if (str)
    {
        unsigned len = _tcslen(str);
        if (len)
        {
            expand(len);
            _tcscat_s(_str, _size, str);
        }
    }

    return *this;
}


/**
 *  \brief
 */
const CText& CText::operator+=(const char* str)
{
    if (str)
    {
        unsigned len = strlen(str);
        if (len)
        {
            len = expand(len);
            size_t cnt;
            mbstowcs_s(&cnt, &_str[len], _size - len, str, _TRUNCATE);
        }
    }

    return *this;
}


/**
 *  \brief
 */
const CText& CText::operator+=(const CText& txt)
{
    unsigned len = txt.Len();
    if (len)
    {
        expand(len);
        _tcscat_s(_str, _size, txt._str);
    }

    return *this;
}


/**
 *  \brief
 */
void CText::ToUpper()
{
    for (int i = _tcslen(_str) - 1; i >= 0; i--)
        _str[i] = _totupper(_str[i]);
}


/**
 *  \brief
 */
void CText::resize(unsigned newLen)
{
    unsigned newSize = (newLen / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;

    if (_size != newSize)
    {
        if (_str != _buf)
            delete [] _str;
        if (newSize != ALLOC_CHUNK_SIZE)
            _str = new TCHAR[newSize];
        else
            _str = _buf;
        _size = newSize;
    }
}


/**
 *  \brief
 */
unsigned CText::expand(unsigned newLen)
{
    if (newLen == 0)
        return 0;

    unsigned len = _tcslen(_str);
    newLen += len;

    if (_size <= newLen)
    {
        TCHAR *oldStr = _str;
        _size = (newLen / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
        _str = new TCHAR[_size];
        _tcscpy_s(_str, _size, oldStr);
        if (oldStr != _buf)
            delete [] oldStr;
    }

    return len;
}


namespace Tools
{

/**
 *  \brief
 */
void ReleaseKey(WORD virtKey)
{
    if (GetKeyState(virtKey))
    {
        INPUT input          = {0};
        input.type           = INPUT_KEYBOARD;
        input.ki.wVk         = virtKey;
        input.ki.dwFlags     = KEYEVENTF_KEYUP;
        input.ki.dwExtraInfo = GetMessageExtraInfo();

        SendInput(1, &input, sizeof(input));
    }
}

} // namespace Tools
