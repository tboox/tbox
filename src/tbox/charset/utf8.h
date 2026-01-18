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
 * @file        utf8.h
 * @ingroup     charset
 *
 */
#ifndef TB_CHARSET_UTF8_H
#define TB_CHARSET_UTF8_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! to lower
 *
 * @param s     the utf8 string
 * @param n     the utf8 string length
 *
 * @return      the new string size
 */
tb_long_t       tb_charset_utf8_tolower(tb_char_t* s, tb_size_t n);

/*! to upper
 *
 * @param s     the utf8 string
 * @param n     the utf8 string length
 *
 * @return      the new string size
 */
tb_long_t       tb_charset_utf8_toupper(tb_char_t* s, tb_size_t n);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
