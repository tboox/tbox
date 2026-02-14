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
 * @file        towlower.c
 * @ingroup     libc
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../libc.h"
#ifdef TB_CONFIG_OS_WINDOWS
#   include <windows.h>
#else
#   include "../../utils/bits.h"
#endif
#ifdef TB_CONFIG_LIBC_HAVE_TOWLOWER
#   include <wctype.h>
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#ifndef TB_CONFIG_OS_WINDOWS
static __tb_inline__ tb_bool_t tb_unicode_tolower_try(tb_uint32_t ch, tb_uint32_t* out)
{
    // builtin, locale-independent case mapping for some common unicode ranges:
    // - Basic Latin (ASCII)
    // - Latin-1 Supplement (partial)
    // - Latin Extended-A (partial)
    // - Greek (partial)
    // - Cyrillic (partial)
    if (sizeof(tb_wchar_t) == 2 && ch >= 0xd800 && ch <= 0xdfff) return tb_false;

    // Basic Latin (ASCII)
    if (ch <= 0x7f)
    {
        *out = tb_tolower(ch);
        return tb_true;
    }

    // Latin-1 Supplement: U+00C0..U+00D6, U+00D8..U+00DE
    if ((ch >= 0x00c0 && ch <= 0x00d6) || (ch >= 0x00d8 && ch <= 0x00de)) { *out = ch + 0x20; return tb_true; }
    // Latin-1 Supplement: U+00E0..U+00F6, U+00F8..U+00FE
    if ((ch >= 0x00e0 && ch <= 0x00f6) || (ch >= 0x00f8 && ch <= 0x00fe)) { *out = ch; return tb_true; }

    // Latin-1 Supplement: U+0178 <-> U+00FF
    if (ch == 0x0178) { *out = 0x00ff; return tb_true; }
    if (ch == 0x00ff) { *out = ch; return tb_true; }

    // Latin Extended Additional: U+1E9E <-> U+00DF
    if (ch == 0x1e9e) { *out = 0x00df; return tb_true; }
    if (ch == 0x00df) { *out = ch; return tb_true; }

    // Latin Extended-A: many letters have alternating upper/lower code points
    if (ch >= 0x0100 && ch <= 0x012f) { *out = (ch & 0x1) ? ch : (ch + 1); return tb_true; }
    if (ch >= 0x0132 && ch <= 0x0137) { *out = (ch & 0x1) ? ch : (ch + 1); return tb_true; }
    if (ch >= 0x0139 && ch <= 0x0148) { *out = (ch & 0x1) ? (ch + 1) : ch; return tb_true; }
    if (ch >= 0x014a && ch <= 0x0177) { *out = (ch & 0x1) ? ch : (ch + 1); return tb_true; }
    if (ch >= 0x0179 && ch <= 0x017e) { *out = (ch & 0x1) ? (ch + 1) : ch; return tb_true; }
    // Latin Extended-A: long s (already lowercase)
    if (ch == 0x017f) { *out = ch; return tb_true; }

    // Greek and Coptic (partial): U+0391..U+03A1, U+03A3..U+03AB
    if ((ch >= 0x0391 && ch <= 0x03a1) || (ch >= 0x03a3 && ch <= 0x03ab)) { *out = ch + 0x20; return tb_true; }
    // Greek and Coptic (partial): U+03B1..U+03C1, U+03C3..U+03CB, and U+03C2
    if ((ch >= 0x03b1 && ch <= 0x03c1) || (ch >= 0x03c3 && ch <= 0x03cb) || ch == 0x03c2) { *out = ch; return tb_true; }

    // Cyrillic (partial): U+0401/U+0451 and U+0410..U+042F
    if (ch == 0x0401) { *out = 0x0451; return tb_true; }
    if (ch == 0x0451) { *out = ch; return tb_true; }
    if (ch >= 0x0402 && ch <= 0x040f) { *out = ch + 0x50; return tb_true; }
    if (ch >= 0x0452 && ch <= 0x045f) { *out = ch; return tb_true; }
    if (ch >= 0x0410 && ch <= 0x042f) { *out = ch + 0x20; return tb_true; }
    if (ch >= 0x0430 && ch <= 0x044f) { *out = ch; return tb_true; }

    return tb_false;
}
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_wchar_t tb_towlower(tb_wchar_t c)
{
#ifdef TB_CONFIG_OS_WINDOWS
    return (tb_wchar_t)(tb_size_t)CharLowerW((LPWSTR)(tb_size_t)c);
#else
    tb_uint32_t ch = tb_bits_wchar_to_u32_le(c);
    tb_uint32_t out;
    if (__tb_likely__(tb_unicode_tolower_try(ch, &out)))
        return tb_bits_u32_le_to_wchar(out);

#if defined(TB_CONFIG_LIBC_HAVE_TOWLOWER)
    // fallback to libc: the result may depend on the current locale
    return (tb_wchar_t)towlower((wint_t)c);
#else
    return tb_tolower(c);
#endif
#endif
}
