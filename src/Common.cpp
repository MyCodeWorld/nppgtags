/**
 *  \file
 *  \brief  Various helper types and functions
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


#include "Common.h"


char    CCharArray::_end;
wchar_t CWcharArray::_end;


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
const TCHAR* CPath::GetFilename() const
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
CTextW::CTextW(unsigned size) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (_size < size)
    {
        _size = (size / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
        _str = new wchar_t[_size];
        _str[0] = 0;
    }
}


/**
 *  \brief
 */
CTextW::CTextW(const wchar_t* str) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (str)
    {
        _len = wcslen(str);
        if (_len >= _size)
        {
            _size = (_len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
            _str = new wchar_t[_size];
        }
        wcscpy_s(_str, _size, str);
    }
}


/**
 *  \brief
 */
CTextW::CTextW(const char* str) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (str)
    {
        _len = strlen(str);
        if (_len >= _size)
        {
            _size = (_len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
            _str = new wchar_t[_size];
        }
        size_t cnt;
        mbstowcs_s(&cnt, _str, _size, str, _TRUNCATE);
    }
}


/**
 *  \brief
 */
CTextW::CTextW(const CTextW& txt) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (_size < txt._size)
    {
        _size = txt._size;
        _str = new wchar_t[_size];
    }
    _len = txt._len;
    wcscpy_s(_str, _size, txt._str);
}


/**
 *  \brief
 */
const CTextW& CTextW::operator=(const wchar_t* str)
{
    if (str)
    {
        resize(wcslen(str));
        wcscpy_s(_str, _size, str);
    }

    return *this;
}


/**
 *  \brief
 */
const CTextW& CTextW::operator=(const char* str)
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
const CTextW& CTextW::operator=(const CTextW& txt)
{
    if (this != &txt)
    {
        resize(txt._len);
        wcscpy_s(_str, _size, txt._str);
    }

    return *this;
}


/**
 *  \brief
 */
const CTextW& CTextW::operator+=(const wchar_t* str)
{
    if (str)
    {
        unsigned len = wcslen(str);
        if (len)
        {
            expand(len);
            wcscat_s(_str, _size, str);
        }
    }

    return *this;
}


/**
 *  \brief
 */
const CTextW& CTextW::operator+=(const char* str)
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
const CTextW& CTextW::operator+=(const CTextW& txt)
{
    unsigned len = txt._len;
    if (len)
    {
        expand(len);
        wcscat_s(_str, _size, txt._str);
    }

    return *this;
}


/**
 *  \brief
 */
const CTextW& CTextW::append(const wchar_t* str, unsigned len)
{
    if (str && len)
    {
        expand(len);
        wcsncat_s(_str, _size, str, len);
    }

    return *this;
}


/**
 *  \brief
 */
void CTextW::resize(unsigned newLen)
{
    unsigned newSize = (newLen / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;

    _len = newLen;
    if (_size != newSize)
    {
        if (_str != _buf)
            delete [] _str;
        if (newSize != ALLOC_CHUNK_SIZE)
            _str = new wchar_t[newSize];
        else
            _str = _buf;
        _size = newSize;
    }
}


/**
 *  \brief
 */
unsigned CTextW::expand(unsigned newLen)
{
    if (newLen == 0)
        return 0;

    unsigned len = _len;
    _len += newLen;

    if (_size <= _len)
    {
        wchar_t *oldStr = _str;
        _size = (_len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
        _str = new wchar_t[_size];
        wcscpy_s(_str, _size, oldStr);
        if (oldStr != _buf)
            delete [] oldStr;
    }

    return len;
}


/**
 *  \brief
 */
CTextA::CTextA(unsigned size) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (_size < size)
    {
        _size = (size / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
        _str = new char[_size];
        _str[0] = 0;
    }
}


/**
 *  \brief
 */
CTextA::CTextA(const char* str) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (str)
    {
        _len = strlen(str);
        if (_len >= _size)
        {
            _size = (_len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
            _str = new char[_size];
        }
        strcpy_s(_str, _size, str);
    }
}


/**
 *  \brief
 */
CTextA::CTextA(const wchar_t* str) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (str)
    {
        _len = wcslen(str);
        if (_len >= _size)
        {
            _size = (_len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
            _str = new char[_size];
        }
        size_t cnt;
        wcstombs_s(&cnt, _str, _size, str, _TRUNCATE);
    }
}


/**
 *  \brief
 */
CTextA::CTextA(const CTextA& txt) : _size(ALLOC_CHUNK_SIZE), _len(0), _str(_buf)
{
    _buf[0] = 0;
    if (_size < txt._size)
    {
        _size = txt._size;
        _str = new char[_size];
    }
    _len = txt._len;
    strcpy_s(_str, _size, txt._str);
}


/**
 *  \brief
 */
const CTextA& CTextA::operator=(const char* str)
{
    if (str)
    {
        resize(strlen(str));
        strcpy_s(_str, _size, str);
    }

    return *this;
}


/**
 *  \brief
 */
const CTextA& CTextA::operator=(const wchar_t* str)
{
    if (str)
    {
        resize(wcslen(str));
        size_t cnt;
        wcstombs_s(&cnt, _str, _size, str, _TRUNCATE);
    }

    return *this;
}


/**
 *  \brief
 */
const CTextA& CTextA::operator=(const CTextA& txt)
{
    if (this != &txt)
    {
        resize(txt._len);
        strcpy_s(_str, _size, txt._str);
    }

    return *this;
}


/**
 *  \brief
 */
const CTextA& CTextA::operator+=(const char* str)
{
    if (str)
    {
        unsigned len = strlen(str);
        if (len)
        {
            expand(len);
            strcat_s(_str, _size, str);
        }
    }

    return *this;
}


/**
 *  \brief
 */
const CTextA& CTextA::operator+=(const wchar_t* str)
{
    if (str)
    {
        unsigned len = wcslen(str);
        if (len)
        {
            len = expand(len);
            size_t cnt;
            wcstombs_s(&cnt, &_str[len], _size - len, str, _TRUNCATE);
        }
    }

    return *this;
}


/**
 *  \brief
 */
const CTextA& CTextA::operator+=(const CTextA& txt)
{
    unsigned len = txt._len;
    if (len)
    {
        expand(len);
        strcat_s(_str, _size, txt._str);
    }

    return *this;
}


/**
 *  \brief
 */
const CTextA& CTextA::append(const char* str, unsigned len)
{
    if (str && len)
    {
        expand(len);
        strncat_s(_str, _size, str, len);
    }

    return *this;
}


/**
 *  \brief
 */
void CTextA::resize(unsigned newLen)
{
    unsigned newSize = (newLen / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;

    _len = newLen;
    if (_size != newSize)
    {
        if (_str != _buf)
            delete [] _str;
        if (newSize != ALLOC_CHUNK_SIZE)
            _str = new char[newSize];
        else
            _str = _buf;
        _size = newSize;
    }
}


/**
 *  \brief
 */
unsigned CTextA::expand(unsigned newLen)
{
    if (newLen == 0)
        return 0;

    unsigned len = _len;
    _len += newLen;

    if (_size <= _len)
    {
        char *oldStr = _str;
        _size = (_len / ALLOC_CHUNK_SIZE + 1) * ALLOC_CHUNK_SIZE;
        _str = new char[_size];
        strcpy_s(_str, _size, oldStr);
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
