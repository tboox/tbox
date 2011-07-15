/*!The Tiny Box Library
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
 * \file		prefix.h
 *
 */
#ifndef TB_PREFIX_H
#define TB_PREFIX_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"
#include <stdarg.h>

/* /////////////////////////////////////////////////////////
 * macros
 */

// is debug?
#if tbox_CONFIG_DEBUG
# 	define TB_DEBUG
#endif

// debug
#ifndef TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO
# 	ifdef TB_DEBUG
#		define TB_DBG(fmt, arg...)					do { tb_printf(fmt "\n" , ## arg); } while (0)
#		define TB_ABORT()							do { tb_printf("[abort]: line: %d, func: %s, file: %s\n", __tb_line__, __tb_func__, __tb_file__); __tb_volatile__ tb_int_t* a = 0; *a = 1; } while(0)
#		define TB_ASSERT(x)							do { if (!(x)) {tb_printf("[assert]: line: %d, func: %s, x: %s, file: %s\n", __tb_line__, __tb_func__, #x, __tb_file__); } } while(0)
#		define TB_ASSERTA(x)						do { if (!(x)) {tb_printf("[assert]: line: %d, func: %s, x: %s, file: %s\n", __tb_line__, __tb_func__, #x, __tb_file__); __tb_volatile__ tb_int_t* a = 0; *a = 1; } } while(0)
#		define TB_ASSERTM(x, fmt, arg...)			do { if (!(x)) {tb_printf("[assert]: line: %d, func: %s, x: %s, msg: " fmt ", file: %s\n", __tb_line__, __tb_func__, #x, ## arg, __tb_file__); }} while(0)
# 	else
#		define TB_DBG(fmt, arg...)
# 		define TB_ABORT()
#		define TB_ASSERT(x)
#		define TB_ASSERTA(x)
#		define TB_ASSERTM(x, fmt, arg...)
# 	endif
#else
# 	ifdef TB_DEBUG
#		define TB_DBG 								
#		define TB_ABORT()							do { tb_printf("[abort]: line :%d, func: %s, file: %s\n", __tb_line__, __tb_func__, __tb_file__); __tb_volatile__ tb_int_t* a = 0; *a = 1; } while(0)
#		define TB_ASSERT(x)							do { if (!(x)) {tb_printf("[assert]: line :%d, func: %s, x: %s, file: %s\n", __tb_line__, __tb_func__, #x, __tb_file__); } } while(0)
#		define TB_ASSERTA(x)						do { if (!(x)) {tb_printf("[assert]: line :%d, func: %s, x: %s, file: %s\n", __tb_line__, __tb_func__, #x, __tb_file__); __tb_volatile__ tb_int_t* a = 0; *a = 1; } } while(0)
#		define TB_ASSERTM
# 	else
#		define TB_DBG
# 		define TB_ABORT()
#		define TB_ASSERT(x)
#		define TB_ASSERTA(x)
#		define TB_ASSERTM
# 	endif
#endif /* TB_CONFIG_COMPILER_NOT_SUPPORT_VARARG_MACRO */

// check
#define TB_IF_FAIL_RETURN(x) 						do { if (!(x)) return ; } while (0)
#define TB_IF_FAIL_RETURN_VAL(x, v) 				do { if (!(x)) return (v); } while (0)
#define TB_IF_FAIL_GOTO(x, b) 						do { if (!(x)) goto b; } while (0)

#define TB_IF_REACHED_RETURN(x) 					do { return ; } while (0)
#define TB_IF_REACHED_RETURN_VAL(x, v) 				do { return (v); } while (0)
#define TB_IF_REACHED_GOTO(x, b) 					do { goto b; } while (0)

#define TB_ASSERT_RETURN(x) 						do { TB_ASSERT(x); if (!(x)) return ; } while (0)
#define TB_ASSERT_ABORT(x) 							do { TB_ASSERT(x); if (!(x)) { __tb_volatile__ tb_int_t* a = 0; *a = 1; } ; } while (0)
#define TB_ASSERT_RETURN_VAL(x, v) 					do { TB_ASSERT(x); if (!(x)) return (v); } while (0)
#define TB_ASSERT_GOTO(x, b) 						do { TB_ASSERT(x); if (!(x)) goto b; } while (0)

#define TB_STATIC_ASSERT(x) 						do { typedef int __static_assert__[(x)? 1 : -1]; } while(0)

// not implement
#define TB_NOT_IMPLEMENT() 							do { TB_DBG("[not_impl]: func: %s, line: %d, file: %s", __tb_func__, __tb_line__, __tb_file__); } while (0)

// cpu
#ifdef TB_CONFIG_CPU_BIGENDIAN
# 	define TB_WORDS_BIGENDIAN
# 	define TB_FLOAT_BIGENDIAN
#endif

// arch
#if defined(TB_CONFIG_ARCH_x86)
# 	define TB_ARCH_x86
#elif defined(TB_CONFIG_ARCH_ARM)
# 	define TB_ARCH_ARM
#elif defined(TB_CONFIG_ARCH_SH4)
# 	define TB_ARCH_SH4
#elif defined(TB_CONFIG_ARCH_MIPS)
# 	define TB_ARCH_MIPS
#elif defined(TB_CONFIG_ARCH_SPARC)
# 	define TB_ARCH_SPARC
#elif defined(TB_CONFIG_ARCH_PPC)
# 	define TB_ARCH_PPC
#else
# 	error unknown arch
#endif

/* /////////////////////////////////////////////////////////
 * compiler
 */
// intel c++
#if defined(__INTEL_COMPILER)
#	define TB_COMPILER_IS_INTEL
#	define TB_COMPILER_STRING 						"Intel C/C++"
#	if (__INTEL_COMPILER == 600)
#		define TB_COMPILER_VERSION_STRING			"Intel C/C++ 6.0"
#	elif (__INTEL_COMPILER == 700)
#		define TB_COMPILER_VERSION_STRING			"Intel C/C++ 7.0"
#	elif (__INTEL_COMPILER == 800)
#		define TB_COMPILER_VERSION_STRING			"Intel C/C++ 8.0"
#	elif (__INTEL_COMPILER == 900)
#		define TB_COMPILER_VERSION_STRING			"Intel C/C++ 9.0"
#	elif (__INTEL_COMPILER == 1000)
#		define TB_COMPILER_VERSION_STRING			"Intel C/C++ 10.0"
#	elif (__INTEL_COMPILER == 1100)
#		define TB_COMPILER_VERSION_STRING			"Intel C/C++ 11.0"
#	elif (__INTEL_COMPILER == 1110)
#		define TB_COMPILER_VERSION_STRING			"Intel C/C++ 11.1"
#	else
#		error Unknown Intel C++ Compiler Version
#	endif

// borland c++
#elif defined(__BORLANDC__)
#	define TB_COMPILER_IS_BORLAND
#	define TB_COMPILER_STRING						"Borland C/C++"
#	if 0
#		define TB_COMPILER_VERSION_STRING			"Borland C++ 4.52"
#	elif 0
#		define TB_COMPILER_VERSION_STRING			"Borland C++ 5.5"
#	elif (__BORLANDC__ == 0x0551)
#		define TB_COMPILER_VERSION_STRING			"Borland C++ 5.51"
#	elif (__BORLANDC__ == 0x0560)
#		define TB_COMPILER_VERSION_STRING			"Borland C++ 5.6"
# elif (__BORLANDC__ == 0x0564)
#		define TB_COMPILER_VERSION_STRING			"Borland C++ 5.6.4 (C++ BuilderX)"
#	elif (__BORLANDC__ == 0x0582)
#		define TB_COMPILER_VERSION_STRING			"Borland C++ 5.82 (Turbo C++)"
#	else
#		error Unknown Borland C++ Compiler Version
#	endif

// gnu c/c++ 
#elif defined(__GNUC__)
#	define TB_COMPILER_IS_GCC
#	define TB_COMPILER_STRING						"GNU C/C++"
#	if  __GNUC__ == 2
#		if __GNUC_MINOR__ < 95
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ <2.95"
#		elif __GNUC_MINOR__ == 95
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 2.95"
#		elif __GNUC_MINOR__ == 96
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 2.96"
#		else
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ >2.96&&<3.0"
#		endif
#	elif __GNUC__ == 3
#		if __GNUC_MINOR__ == 2
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 3.2"
#		elif __GNUC_MINOR__ == 3
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 3.3"
#		elif __GNUC_MINOR__ == 4
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 3.4"
#		else
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ >3.4&&<4.0"
#		endif
#	elif __GNUC__ == 4
#		if __GNUC_MINOR__ == 1
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 4.1"
#		elif __GNUC_MINOR__ == 2
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 4.2"
#		elif __GNUC_MINOR__ == 3
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 4.3"
#		elif __GNUC_MINOR__ == 4
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 4.4"
#		endif
#	else
#		error Unknown GNU C/C++ Compiler Version
#	endif

// watcom c/c++ 
#elif defined(__WATCOMC__)
#	define TB_COMPILER_IS_WATCOM
#	define TB_COMPILER_STRING 						"Watcom C/C++"
#	if (__WATCOMC__ == 1100)
#		define TB_COMPILER_VERSION_STRING 			"Watcom C/C++ 11.0"
#	elif (__WATCOMC__ == 1200)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.0 (Watcom 12.0)"
#	elif (__WATCOMC__ == 1210)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.1 (Watcom 12.1)"
#	elif (__WATCOMC__ == 1220)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.2 (Watcom 12.2)"
#	elif (__WATCOMC__ == 1230)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.3 (Watcom 12.3)"
#	elif (__WATCOMC__ == 1240)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.4 (Watcom 12.4)"
#	elif (__WATCOMC__ == 1250)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.5"
#	elif (__WATCOMC__ == 1260)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.6"
#	elif (__WATCOMC__ == 1270)
#		define TB_COMPILER_VERSION_STRING 			"Open Watcom C/C++ 1.7"
#	else
#		error Unknown Watcom C/C++ Compiler Version
#	endif

// digital mars c/c++
#elif defined(__DMC__)
#	define TB_COMPILER_IS_DMC
#	define TB_COMPILER_STRING 						"Digital Mars C/C++"
#	if (__DMC__ < 0x0826)
#		error Only versions 8.26 and later of the Digital Mars C/C++ compilers are supported by the EXTL libraries
#	else
#		if __DMC__ >= 0x0832
#			define TB_COMPILER_VERSION_STRING 		__DMC_VERSION_STRING__
#		elif (__DMC__ == 0x0826)
#			define TB_COMPILER_VERSION_STRING 		"Digital Mars C/C++ 8.26"
#		elif (__DMC__ == 0x0827)
#			define TB_COMPILER_VERSION_STRING 		"Digital Mars C/C++ 8.27"
#		elif (__DMC__ == 0x0828)
#			define TB_COMPILER_VERSION_STRING 		"Digital Mars C/C++ 8.28"
#		elif (__DMC__ == 0x0829)
#			define TB_COMPILER_VERSION_STRING 		"Digital Mars C/C++ 8.29"
#		elif (__DMC__ == 0x0830)
#			define TB_COMPILER_VERSION_STRING 		"Digital Mars C/C++ 8.30"
#		elif (__DMC__ == 0x0831)
#			define TB_COMPILER_VERSION_STRING 		"Digital Mars C/C++ 8.31"
#		else
#			error Unknown Digital Mars C/C++ Compiler Version
#		endif
#	endif

// codeplay vectorc c/c++
#elif defined(__VECTORC)
#	define TB_COMPILER_IS_VECTORC
#	define TB_COMPILER_VERSION_STRING 				"CodePlay VectorC C/C++"
#	if (__VECTORC == 1)
#		define TB_COMPILER_VERSION_STRING 			"CodePlay VectorC C/C++"
#	else
#		error Unknown CodePlay VectorC C++ Compiler Version
#	endif

// visual c++
#elif defined(_MSC_VER)
#	define TB_COMPILER_IS_MSVC
#		define TB_COMPILER_STRING					"Visual C++"
#	if defined(TB_FORCE_MSVC_4_2) && (_MSC_VER == 1020)
#		define TB_COMPILER_VERSION_STRING			"Visual C++ 4.2"
#	elif (_MSC_VER == 1100)
#		define TB_COMPILER_VERSION_STRING 			"Visual C++ 5.0"
#	elif (_MSC_VER == 1200)
#		define TB_COMPILER_VERSION_STRING 			"Visual C++ 6.0"
#	elif (_MSC_VER == 1300)
#		define TB_COMPILER_VERSION_STRING 			"Visual C++ .NET (7.0)"
#	elif (_MSC_VER == 1310)
#		define TB_COMPILER_VERSION_STRING 			"Visual C++ .NET (7.1)"	/* .NET 2003 */
#	elif (_MSC_VER == 1400)
#		define TB_COMPILER_VERSION_STRING 			"Visual C++ .NET (8.0)"	/* .NET 2005 */
#	elif (_MSC_VER == 1500)
#		define TB_COMPILER_VERSION_STRING 			"Visual C++ .NET (9.0)"	/* .NET 2008 */
#	else
#		error Unknown Visual C++ Compiler Version
#	endif

#else
#	define TB_COMPILER_STRING 						"Unknown Compiler"
#	define TB_COMPILER_VERSION_STRING 				"Unknown Compiler Version"
#	define TB_COMPILER_IS_UNKNOWN
#endif

// keyword
#define __tb_inline__ 				TB_CONFIG_KEYWORD_INLINE
#define __tb_asm__ 					TB_CONFIG_KEYWORD_ASM
#define __tb_func__ 				TB_CONFIG_KEYWORD_FUNC
#define __tb_file__ 				TB_CONFIG_KEYWORD_FILE
#define __tb_line__ 				TB_CONFIG_KEYWORD_LINE
#define __tb_volatile__ 			volatile

#if defined(TB_COMPILER_IS_GCC) && __GNUC__ > 2
# 	define __tb_likely__(x) 		__builtin_expect((x), 1)
# 	define __tb_unlikely__(x) 		__builtin_expect((x), 0)
#else
# 	define __tb_likely__(x) 		(x)
# 	define __tb_unlikely__(x) 		(x)
#endif

// bool values
#define TB_TRUE						((tb_bool_t)1)
#define	TB_FALSE					((tb_bool_t)0)

// null
#ifdef __cplusplus
# 	define TB_NULL 					(0)
#else
# 	define TB_NULL 					((void*)0)
#endif


// varg
#define TB_VARG_FORMAT(s, n, fmt, r) \
do \
{ \
	tb_int_t __tb_ret = 0; \
	va_list __tb_varg_list; \
    va_start(__tb_varg_list, fmt); \
    __tb_ret = vsnprintf(s, (n) - 1, fmt, __tb_varg_list); \
    va_end(__tb_varg_list); \
	if (__tb_ret >= 0) s[__tb_ret] = '\0'; \
	if (r) *r = __tb_ret > 0? __tb_ret : 0; \
 \
} while (0) 


/* /////////////////////////////////////////////////////////
 * types
 */

// basic
typedef signed int				tb_int_t;
typedef unsigned int			tb_uint_t;
typedef tb_int_t				tb_bool_t;
typedef tb_uint_t				tb_size_t;
typedef signed char				tb_int8_t;
typedef tb_int8_t				tb_sint8_t;
typedef unsigned char			tb_uint8_t;
typedef signed short			tb_int16_t;
typedef tb_int16_t				tb_sint16_t;
typedef unsigned short			tb_uint16_t;
typedef tb_int_t				tb_int32_t;
typedef tb_int32_t				tb_sint32_t;
typedef tb_uint_t				tb_uint32_t;
typedef char 					tb_char_t;
typedef tb_int32_t 				tb_uchar_t;
typedef tb_uint8_t				tb_byte_t;
typedef void 					tb_void_t;

// int64
#ifdef TB_CONFIG_TYPE_INT64
typedef signed long long 		tb_int64_t;
typedef unsigned long long 		tb_uint64_t;
typedef tb_int64_t				tb_sint64_t;
#else
# 	error int64 is not supported.
#endif

// handle
typedef tb_void_t* 				tb_handle_t;

// float
#ifdef TB_CONFIG_TYPE_FLOAT
typedef double 					tb_float_t;
#endif

// fixed
typedef tb_int32_t 				tb_fixed6_t;
typedef tb_int32_t 				tb_fixed16_t;
typedef tb_int32_t 				tb_fixed30_t;
typedef tb_fixed16_t 			tb_fixed_t;

// scalar
#ifdef TB_CONFIG_TYPE_SCALAR_IS_FIXED
typedef tb_fixed_t 				tb_scalar_t;
#else
typedef tb_float_t 				tb_scalar_t;
#endif

// limits
#define TB_MAXS16 				(32767)
#define TB_MINS16 				(-32767)
#define TB_MAXU16 				(0xffff)
#define TB_MINU16 				(0)
#define TB_MAXS32 				(0x7fffffff)
#define TB_MINS32 				(0x80000001)
#define TB_MAXU32 				(0xffffffff)
#define TB_MINU32 				(0)
#define TB_NAN32 				(0x80000000)

// abs
#define tb_abs(x) 				((x) > 0? (x) : -(x))

// min & max
#define tb_max(x, y) 			(((x) > (y))? (x) : (y))
#define tb_min(x, y) 			(((x) < (y))? (x) : (y))

// swap
#ifdef __cplusplus
template <typename T> 
static __tb_inline__ void tb_swap(T& x, T& y) 
{
	T c(x);
	x = y;
	y = c;
}
#else
# 	define tb_swap(x, y) 		((x) ^= (y) ^= (x) ^= (y))
#endif

// the number of entries in the array
#define tb_arrayn(x) 			(sizeof((x)) / sizeof((x)[0]))

// align
#define tb_align2(x) 			(((x) + 1) >> 1 << 1)
#define tb_align4(x) 			(((x) + 3) >> 2 << 2)
#define tb_align(x, b) 			(((x) + ((b) - 1)) & ~((b) - 1))

// offsetof
#define tb_offsetof(t, x) 		((tb_byte_t*)&(((t*)1)->x) - (tb_byte_t*)1)

/* /////////////////////////////////////////////////////////
 * memory
 */
#ifdef TB_CONFIG_MEMORY_POOL_ENABLE

# 	ifdef TB_DEBUG
void* 		tb_mpool_allocate(tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
void*  		tb_mpool_callocate(tb_size_t item, tb_size_t size, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
void* 		tb_mpool_reallocate(void* data, tb_size_t size,tb_char_t const* func,  tb_size_t line, tb_char_t const* file);
void 		tb_mpool_deallocate(void* data, tb_char_t const* func, tb_size_t line, tb_char_t const* file);
# 	else
void* 		tb_mpool_allocate(tb_size_t size);
void*  		tb_mpool_callocate(tb_size_t item, tb_size_t size);
void* 		tb_mpool_reallocate(void* data, tb_size_t size);
void 		tb_mpool_deallocate(void* data);
# 	endif

# 	ifdef TB_DEBUG
# 		define tb_malloc(size) 					tb_mpool_allocate(size, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_calloc(item, size) 			tb_mpool_callocate(item, size, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_realloc(data, size) 			tb_mpool_reallocate(data, size, __tb_func__, __tb_line__, __tb_file__)
# 		define tb_free(data) 					tb_mpool_deallocate(data, __tb_func__, __tb_line__, __tb_file__)
# 	else
# 		define tb_malloc(size) 					tb_mpool_allocate(size)
# 		define tb_calloc(item, size) 			tb_mpool_callocate(item, size)
# 		define tb_realloc(data, size) 			tb_mpool_reallocate(data, size)
# 		define tb_free(data) 					tb_mpool_deallocate(data)
# 	endif

#else

void* 	tb_malloc(tb_size_t size);
void* 	tb_realloc(void* data, tb_size_t size);
void* 	tb_calloc(tb_size_t item, tb_size_t size);
void 	tb_free(void* data);

#endif

#ifdef __cplusplus
__tb_inline__ void* operator new(tb_size_t size) throw () 	{ return tb_malloc(size); 	}
__tb_inline__ void 	operator delete(void* p) throw() 		{ tb_free(p); 				}
__tb_inline__ void* operator new[](tb_size_t size) throw () { return tb_malloc(size); 	}
__tb_inline__ void 	operator delete[](void* p) throw() 		{ tb_free(p); 				}
#endif

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
