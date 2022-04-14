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
 * @file        stdfile.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_STDFILE_H
#define TB_PLATFORM_STDFILE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the stdfile ref type
typedef __tb_typeref__(stdfile);

/// the stdfile type
typedef enum __tb_stdfile_type_e
{
    TB_STDFILE_TYPE_NONE    = 0
,   TB_STDFILE_TYPE_STDIN   = 1
,   TB_STDFILE_TYPE_STDOUT  = 2
,   TB_STDFILE_TYPE_STDERR  = 3

}tb_stdfile_type_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get stdin file singleton
 *
 * @return              the stdin file singleton
 */
tb_stdfile_ref_t        tb_stdfile_input(tb_noarg_t);

/*! get stdout file singleton
 *
 * @return              the stdout file singleton
 */
tb_stdfile_ref_t        tb_stdfile_output(tb_noarg_t);

/*! get stderr file singleton
 *
 * @return              the stdin file singleton
 */
tb_stdfile_ref_t        tb_stdfile_error(tb_noarg_t);

/*! init the standard input/ouput device file
 *
 * @param type          the file type, e.g. stdin, stdout, stderr
 *
 * @return              the stdfile
 */
tb_stdfile_ref_t        tb_stdfile_init(tb_size_t type);

/*! exit the stdfile
 *
 * @param stdfile       the stdfile
 */
tb_void_t               tb_stdfile_exit(tb_stdfile_ref_t stdfile);

/*! get the stdfile type
 *
 * @param stdfile       the stdfile
 *
 * @return              the file type
 */
tb_size_t               tb_stdfile_type(tb_stdfile_ref_t stdfile);

/*! forces a write of all buffered data for the given output
 *
 * @note does not support to flush stdin
 *
 * @param file          the file
 */
tb_bool_t               tb_stdfile_flush(tb_stdfile_ref_t stdfile);

/*! read the stdfile data (block mode)
 *
 * @param file          the stdfile
 * @param data          the data
 * @param size          the size
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_read(tb_stdfile_ref_t file, tb_byte_t* data, tb_size_t size);

/*! write the stdfile data (block mode)
 *
 * @param file          the stdfile
 * @param data          the data
 * @param size          the size
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_writ(tb_stdfile_ref_t file, tb_byte_t const* data, tb_size_t size);

/*! has readable data for stdfile
 *
 * @param file          the stdfile
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_readable(tb_stdfile_ref_t file);

/*! peek an character from stdfile
 *
 * @param file          the stdfile
 * @param ch            the character pointer
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_peek(tb_stdfile_ref_t file, tb_char_t* pch);

/*! read an character from stdfile
 *
 * @param file          the stdfile
 * @param ch            the character pointer
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_getc(tb_stdfile_ref_t file, tb_char_t* pch);

/*! write an character to stdfile
 *
 * @param file          the stdfile
 * @param ch            the character
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_putc(tb_stdfile_ref_t file, tb_char_t ch);

/*! reads characters from stdfile and stores them as a C string into str until (num-1) characters have been read
 * or either a newline or the end-of-file is reached, whichever happens first.
 *
 * a newline character makes fgets stop reading, but it is considered a valid character by the function and
 * included in the string copied to str.
 *
 * a terminating null character is automatically appended after the characters copied to str.
 *
 * @param file          the stdfile
 * @param str           pointer to an array of chars where the string read is copied.
 * @param num           maximum number of characters to be copied into str (including the terminating null-character).
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_gets(tb_stdfile_ref_t file, tb_char_t* str, tb_size_t num);

/*! writes the C string pointed by str to the stdfile.
 *
 * the function begins copying from the address specified (str) until it reaches the terminating null character ('\0').
 * this terminating null-character is not copied to the stdfile.
 *
 * @param file          the stdfile
 * @param s             C string with the content to be written to stdfile.
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_stdfile_puts(tb_stdfile_ref_t file, tb_char_t const* str);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
