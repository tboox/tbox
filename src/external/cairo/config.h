/* confdefs.h */
#ifndef CAIRO_CONFIG_H
#define CAIRO_CONFIG_H

#include "../prefix.h" 

#define PACKAGE_NAME "cairo"
#define PACKAGE_TARNAME "cairo"
#define PACKAGE_VERSION "1.8.10"
#define PACKAGE_STRING "cairo 1.8.10"
#define PACKAGE_BUGREPORT "http://bugs.freedesktop.org/enter_bug.cgi?product=cairo"
#define PACKAGE_URL ""
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

// no mutex
#define CAIRO_NO_MUTEX 1

// no directfb
#define CAIRO_HAS_DIRECTFB_SURFACE 0

// no freetype
#ifndef CAIRO_HAS_FT_FONT
#define CAIRO_HAS_FT_FONT 0
#endif

// endian
#ifdef TPLAT_FLOAT_BIGENDIAN
# 	define FLOAT_WORDS_BIGENDIAN
#else
# 	undef FLOAT_WORDS_BIGENDIAN
#endif
#ifdef TPLAT_WORDS_BIGENDIAN
# 	define WORDS_BIGENDIAN
#else
# 	undef WORDS_BIGENDIAN
#endif

// disable some floating point
#define DISABLE_SOME_FLOATING_POINT 1

#endif
