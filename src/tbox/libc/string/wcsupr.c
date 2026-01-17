/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        wcsupr.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "string.h"
#include "../misc/wctype.h"
#include "../stdlib/setlocale.h"
#ifdef TB_CONFIG_LIBC_HAVE_WCSUPR
#   include <wchar.h>
#endif
#if defined(TB_CONFIG_OS_MACOSX) || defined(TB_CONFIG_OS_IOS)
#   include <CoreFoundation/CoreFoundation.h>
#endif
#ifdef TB_CONFIG_OS_WINDOWS
#   include <windows.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_wchar_t* tb_wcsupr(tb_wchar_t* s)
{
    // check
    tb_assert_and_check_return_val(s, tb_null);

#ifdef TB_CONFIG_OS_WINDOWS
    // use system api
    CharUpperW((LPWSTR)s);
    return s;
#endif

#if defined(TB_CONFIG_OS_MACOSX) || defined(TB_CONFIG_OS_IOS)
    // use framework
    tb_bool_t ok = tb_false;
    tb_size_t n = tb_wcslen(s);
    CFStringEncoding encoding = (sizeof(tb_wchar_t) == 4) ? kCFStringEncodingUTF32LE : kCFStringEncodingUTF16LE;
    CFStringRef str = CFStringCreateWithBytes(kCFAllocatorDefault, (UInt8 const*)s, n * sizeof(tb_wchar_t), encoding, false);
    if (str)
    {
        CFMutableStringRef mstr = CFStringCreateMutableCopy(kCFAllocatorDefault, 0, str);
        if (mstr)
        {
            CFStringUppercase(mstr, NULL);
            CFIndex len = CFStringGetLength(mstr);

            // strictly check length to avoid buffer overflow and truncation
            if (len == (CFIndex)n)
            {
                CFIndex converted = CFStringGetBytes(mstr, CFRangeMake(0, len), encoding, 0, false, (UInt8*)s, n * sizeof(tb_wchar_t), NULL);
                if (converted == len) ok = tb_true;
            }
            CFRelease(mstr);
        }
        CFRelease(str);
    }
    if (ok) return s;
#endif

    // set local locale
    tb_setlocale();

#ifdef TB_CONFIG_LIBC_HAVE_WCSUPR
    wcsupr(s);
#else
    tb_wchar_t* p = s;
    while (*p)
    {
        *p = tb_towupper(*p);
        p++;
    }
#endif

    // set default locale
    tb_resetlocale();

    return s;
}
