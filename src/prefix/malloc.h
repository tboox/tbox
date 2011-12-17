/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		malloc.h
 *
 */
#ifndef TB_PREFIX_MALLOC_H
#define TB_PREFIX_MALLOC_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// malloc & free
#ifdef TB_CONFIG_MEMORY_POOL_ENABLE

# 	ifdef TB_DEBUG
tb_pointer_t 		tb_memory_allocate(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_pointer_t  	tb_memory_callocate(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
tb_pointer_t 		tb_memory_reallocate(tb_pointer_t data, tb_size_t size,tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
tb_void_t 		tb_memory_deallocate(tb_pointer_t data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
# 	else
tb_pointer_t 		tb_memory_allocate(tb_size_t size);
tb_pointer_t  	tb_memory_callocate(tb_size_t item, tb_size_t size);
tb_pointer_t 		tb_memory_reallocate(tb_pointer_t data, tb_size_t size);
tb_void_t 		tb_memory_deallocate(tb_pointer_t data);
# 	endif

# 	ifdef TB_DEBUG
# 		define tb_malloc(size) 					tb_memory_allocate(size, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_calloc(item, size) 			tb_memory_callocate(item, size, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_realloc(data, size) 			tb_memory_reallocate(data, size, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_free(data) 					tb_memory_deallocate(data, __tb_func__, __tb_line__, __tb_file__)
# 	else
# 		define tb_malloc(size) 					tb_memory_allocate(size)
# 		define tb_calloc(item, size) 			tb_memory_callocate(item, size)
# 		define tb_realloc(data, size) 			tb_memory_reallocate(data, size)
# 		define tb_free(data) 					tb_memory_deallocate(data)
# 	endif

#else

tb_pointer_t 	tb_malloc(tb_size_t size);
tb_pointer_t 	tb_realloc(tb_pointer_t data, tb_size_t size);
tb_pointer_t 	tb_calloc(tb_size_t item, tb_size_t size);
tb_void_t 	tb_free(tb_pointer_t data);

#endif

#if 0
// new & delete
#ifdef __cplusplus

# 	ifdef TB_DEBUG
__tb_inline__ tb_pointer_t 	operator new(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file) throw () 	{ return tb_memory_allocate(size, func, line, file); 	}
__tb_inline__ tb_void_t 	operator delete(tb_pointer_t p) throw() 																	{ tb_free(p); 											}
__tb_inline__ tb_pointer_t 	operator new[](tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file) throw () 	{ return tb_memory_allocate(size, func, line, file); 	}
__tb_inline__ tb_void_t 	operator delete[](tb_pointer_t p) throw() 																{ tb_free(p); 											}
# 	else
__tb_inline__ tb_pointer_t 	operator new(tb_size_t size) throw () 	{ return tb_malloc(size); 	}
__tb_inline__ tb_void_t 	operator delete(tb_pointer_t p) throw() 	{ tb_free(p); 				}
__tb_inline__ tb_pointer_t 	operator new[](tb_size_t size) throw () { return tb_malloc(size); 	}
__tb_inline__ tb_void_t 	operator delete[](tb_pointer_t p) throw() { tb_free(p); 				}
# 	endif

# 	ifdef TB_DEBUG
# 		define new 			new(__tb_func__, __tb_line__, __tb_file__)
# 	endif

#endif
#endif


// c plus plus
#ifdef __cplusplus
}
#endif

#endif


