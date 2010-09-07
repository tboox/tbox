/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		regex.h
 *
 */
#ifndef TB_REGEX_H
#define TB_REGEX_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* Options. 
 * Some are compile-time only, some are run-time only, 
 * and some are both, so we keep them all distinct. 
 */
#define TB_REGEX_NULL 				(0x00000000)
#define TB_REGEX_CASELESS 			(0x00000001)
#define TB_REGEX_MULTILINE 			(0x00000002)
#define TB_REGEX_DOTALL 			(0x00000004)
#define TB_REGEX_EXTENDED 			(0x00000008)
#define TB_REGEX_ANCHORED 			(0x00000010)
#define TB_REGEX_DOLLAR_ENDONLY 	(0x00000020)
#define TB_REGEX_EXTRA 				(0x00000040)
#define TB_REGEX_NOTBOL 			(0x00000080)
#define TB_REGEX_NOTEOL 			(0x00000100)
#define TB_REGEX_UNGREEDY 			(0x00000200)
#define TB_REGEX_NOTEMPTY 			(0x00000400)
#define TB_REGEX_UTF8 				(0x00000800)
#define TB_REGEX_NO_AUTO_CAPTURE 	(0x00001000)
#define TB_REGEX_NO_UTF8_CHECK 		(0x00002000)
#define TB_REGEX_AUTO_CALLOUT 		(0x00004000)
#define TB_REGEX_PARTIAL_SOFT 		(0x00008000)
#define TB_REGEX_PARTIAL 			(0x00008000)  //!< Backwards compatible synonym
#define TB_REGEX_DFA_SHORTEST 		(0x00010000)
#define TB_REGEX_DFA_RESTART 		(0x00020000)
#define TB_REGEX_FIRSTLINE 			(0x00040000)
#define TB_REGEX_DUPNAMES 			(0x00080000)
#define TB_REGEX_NEWLINE_CR 		(0x00100000)
#define TB_REGEX_NEWLINE_LF 		(0x00200000)
#define TB_REGEX_NEWLINE_CRLF 		(0x00300000)
#define TB_REGEX_NEWLINE_ANY 		(0x00400000)
#define TB_REGEX_NEWLINE_ANYCRLF 	(0x00500000)
#define TB_REGEX_BSR_ANYCRLF 		(0x00800000)
#define TB_REGEX_BSR_UNICODE 		(0x01000000)
#define TB_REGEX_JAVASCRIPT_COMPAT 	(0x02000000)
#define TB_REGEX_NO_START_OPTIMIZE 	(0x04000000)
#define TB_REGEX_NO_START_OPTIMISE 	(0x04000000)
#define TB_REGEX_PARTIAL_HARD 		(0x08000000)
#define TB_REGEX_NOTEMPTY_ATSTART 	(0x10000000)

/* /////////////////////////////////////////////////////////
 * the interfaces
 */
tb_handle_t 		tb_regex_create(tb_char_t const* regex, tb_int_t opts);
void 				tb_regex_destroy(tb_handle_t hregex);

tb_bool_t 			tb_regex_exec(tb_handle_t hregex, tb_char_t const* s, tb_int_t opts);
tb_char_t const* 	tb_regex_get(tb_handle_t hregex, tb_int_t index, tb_char_t* sub_s, tb_int_t* sub_sn);
tb_int_t 		 	tb_regex_count(tb_handle_t hregex);
tb_char_t const*	tb_regex_error(tb_handle_t hregex, tb_int_t* error_at);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
