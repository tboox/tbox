/* confdefs.h */
#ifndef PIXMAN_CONFDEFS_H
#define PIXMAN_CONFDEFS_H

#define PACKAGE_NAME "pixman"
#define PACKAGE_TARNAME "pixman"
#define PACKAGE_VERSION "0.18.0"
#define PACKAGE_STRING "pixman 0.18.0"
#define PACKAGE_BUGREPORT ""pixman@lists.freedesktop.org""
#define PACKAGE_URL ""
#define PACKAGE "pixman"
#define VERSION "0.18.0"
#define STDC_HEADERS 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_SYS_STAT_H 1
#define HAVE_STDLIB_H 1
#define HAVE_STRING_H 1
#define HAVE_MEMORY_H 1
#define HAVE_STRINGS_H 1
#define HAVE_INTTYPES_H 1
#define HAVE_STDINT_H 1
#define HAVE_UNISTD_H 1
#define HAVE_DLFCN_H 1
#define LT_OBJDIR ".libs/"
#define SIZEOF_LONG 4

// malloc
#include "../../../malloc.h"


// flasher config
#include "../../../../config.h" 

// endian
#ifdef CONFIG_WORDS_BIGENDIAN
# 	define WORDS_BIGENDIAN
#else
# 	undef WORDS_BIGENDIAN
#endif

/* use ARM NEON assembly optimizations */
/* #undef USE_ARM_NEON */

/* use ARM SIMD assembly optimizations */
/* #undef USE_ARM_SIMD */

/* use GNU-style inline assembler */
//#define USE_GCC_INLINE_ASM 1

/* use MMX compiler intrinsics */
//#define USE_MMX 1

/* use SSE2 compiler intrinsics */
//#define USE_SSE2 1

/* use VMX compiler intrinsics */
/* #undef USE_VMX */


#endif
