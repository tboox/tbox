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
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		string.h
 *
 */
#ifndef TB_STRING_H
#define TB_STRING_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
// the string stack max
#define TB_STRING_STACK_MAX 			(1024)

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the string type
typedef struct __tb_string_t
{
	// the string data
	tb_byte_t* 		data;
	tb_uint16_t 	size;

	// the maximum size
	// readonly: maxn ==0 && owner == 0
	tb_uint16_t 	maxn : 15;
	tb_uint16_t 	owner : 1;

}tb_string_t;

// the stack string type
typedef struct __tb_stack_string_t
{
	// the string base
	tb_string_t 	base;

	// the stack
	tb_byte_t 		stack[TB_STRING_STACK_MAX];

}tb_stack_string_t;

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */

#if 1

// create & destroy
tb_string_t* 		tb_string_create();
tb_string_t* 		tb_string_create_from_buffer(tb_byte_t* data, tb_uint16_t size);
void 				tb_string_destroy(tb_string_t* string);

// init
void				tb_string_init(tb_string_t* string);
void				tb_string_init_stack_string(tb_stack_string_t* string);
void 				tb_string_init_from_buffer(tb_string_t* string, tb_byte_t* data, tb_uint16_t size);
void 				tb_string_uninit(tb_string_t* string);

// accessors
tb_char_t const* 	tb_string_c_string(tb_string_t const* string);
tb_uint16_t 		tb_string_size(tb_string_t const* string);
tb_char_t 			tb_string_at(tb_string_t const* string, tb_int_t index);
tb_bool_t 			tb_string_is_null(tb_string_t const* string);
tb_bool_t 			tb_string_subat(tb_string_t const* string, tb_string_t* sub, tb_int_t start, tb_int_t size);

// finders
tb_int_t 			tb_string_find_char(tb_string_t const* string, tb_char_t ch, tb_int_t start);
tb_int_t 			tb_string_find_c_string(tb_string_t const* string, tb_char_t const* sub, tb_int_t start);
tb_int_t 			tb_string_find_string(tb_string_t const* string, tb_string_t const* sub, tb_int_t start);

tb_int_t 			tb_string_find_char_nocase(tb_string_t const* string, tb_char_t ch, tb_int_t start);
tb_int_t 			tb_string_find_c_string_nocase(tb_string_t const* string, tb_char_t const* sub, tb_int_t start);
tb_int_t 			tb_string_find_string_nocase(tb_string_t const* string, tb_string_t const* sub, tb_int_t start);

tb_int_t 			tb_string_reverse_find_char(tb_string_t const* string, tb_char_t ch, tb_int_t start);
tb_int_t 			tb_string_reverse_find_c_string(tb_string_t const* string, tb_char_t const* sub, tb_int_t start);
tb_int_t 			tb_string_reverse_find_string(tb_string_t const* string, tb_string_t const* sub, tb_int_t start);

tb_int_t 			tb_string_reverse_find_char_nocase(tb_string_t const* string, tb_char_t ch, tb_int_t start);
tb_int_t 			tb_string_reverse_find_c_string_nocase(tb_string_t const* string, tb_char_t const* sub, tb_int_t start);
tb_int_t 			tb_string_reverse_find_string_nocase(tb_string_t const* string, tb_string_t const* sub, tb_int_t start);

// modifiors
tb_bool_t 			tb_string_resize(tb_string_t* string, tb_uint16_t size);
tb_byte_t* 			tb_string_data(tb_string_t* string);
void 				tb_string_clear(tb_string_t* string);
void 				tb_string_set(tb_string_t const* string, tb_int_t index, tb_char_t ch);
tb_char_t const* 	tb_string_shift(tb_string_t* string, tb_string_t* s_string);

// assign
tb_char_t const* 	tb_string_assign(tb_string_t* string, tb_string_t const* s_string);
tb_char_t const* 	tb_string_assign_char(tb_string_t* string, tb_char_t ch);
tb_char_t const* 	tb_string_assign_format(tb_string_t* string, tb_char_t const* fmt, ...);
tb_char_t const* 	tb_string_assign_c_string(tb_string_t* string, tb_char_t const* c_string);
tb_char_t const* 	tb_string_assign_c_string_with_size(tb_string_t* string, tb_char_t const* c_string, tb_size_t size);

tb_char_t const* 	tb_string_assign_by_ref(tb_string_t* string, tb_string_t const* s_string);
tb_char_t const* 	tb_string_assign_c_string_by_ref(tb_string_t* string, tb_char_t const* c_string);
tb_char_t const* 	tb_string_assign_c_string_with_size_by_ref(tb_string_t* string, tb_char_t const* c_string, tb_size_t size);

// append
tb_char_t const* 	tb_string_append(tb_string_t* string, tb_string_t const* s_string);
tb_char_t const* 	tb_string_append_c_string(tb_string_t* string, tb_char_t const* c_string);
tb_char_t const* 	tb_string_append_c_string_with_size(tb_string_t* string, tb_char_t const* c_string, tb_size_t size);
tb_char_t const* 	tb_string_append_char(tb_string_t* string, tb_char_t ch);
tb_char_t const* 	tb_string_append_format(tb_string_t* string, tb_char_t const* fmt, ...);

// compare
tb_bool_t 			tb_string_compare(tb_string_t* string, tb_string_t const* s_string);
tb_bool_t 			tb_string_compare_c_string(tb_string_t* string, tb_char_t const* c_string);
tb_bool_t 			tb_string_compare_nocase(tb_string_t* string, tb_string_t const* s_string);
tb_bool_t 			tb_string_compare_c_string_nocase(tb_string_t* string, tb_char_t const* c_string);

#else
#define tb_string_create() 							tb_string_create_trace(__tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_create_from_buffer(a,b) 			tb_string_create_from_buffer_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_resize(a,b) 						tb_string_resize_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_subat(a,b,c,d) 					tb_string_subat_trace(a,b,c,d, __tplat_func__, __tplat_line__, __tplat_file__)

#define tb_string_assign(a,b) 						tb_string_assign_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_assign_char(a,b) 					tb_string_assign_char_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_assign_format(a,b,...) 			tb_string_assign_format_trace(a, __tplat_func__, __tplat_line__, __tplat_file__, #b)
#define tb_string_assign_c_string(a,b) 				tb_string_assign_c_string_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_assign_c_string_with_size(a,b,c) 	tb_string_assign_c_string_with_size_trace(a,b,c, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_append(a,b) 						tb_string_append_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_append_c_string(a,b) 				tb_string_append_c_string_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_append_c_string_with_size(a,b,c) 	tb_string_append_c_string_with_size_trace(a,b,c, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_append_char(a,b) 					tb_string_append_char_trace(a,b, __tplat_func__, __tplat_line__, __tplat_file__)
#define tb_string_append_format(a,b,...)			tb_string_append_format_trace(a, __tplat_func__, __tplat_line__, __tplat_file__, #b)

#endif

#endif

