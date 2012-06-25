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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		compiler.h
 *
 */
#ifndef TB_PREFIX_COMPILER_H
#define TB_PREFIX_COMPILER_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

// intel c++
#if defined(__INTEL_COMPILER)
#	define TB_COMPILER_IS_INTEL
# 	define TB_COMPILER_VERSION_BT(major, minor) 	(__INTEL_COMPILER > ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_BE(major, minor) 	(__INTEL_COMPILER >= ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_EQ(major, minor) 	(__INTEL_COMPILER == ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LT(major, minor) 	(__INTEL_COMPILER < ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LE(major, minor) 	(__INTEL_COMPILER <= ((major) * 100 + (minor)))
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
# 	define TB_COMPILER_VERSION_BT(major, minor) 	(__BORLANDC__ > ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_BE(major, minor) 	(__BORLANDC__ >= ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_EQ(major, minor) 	(__BORLANDC__ == ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LT(major, minor) 	(__BORLANDC__ < ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LE(major, minor) 	(__BORLANDC__ <= ((major) * 100 + (minor)))
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
# 	define TB_COMPILER_VERSION_BT(major, minor) 	((__GNUC__ * 100 + __GNUC_MINOR__) > ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_BE(major, minor) 	((__GNUC__ * 100 + __GNUC_MINOR__) >= ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_EQ(major, minor) 	((__GNUC__ * 100 + __GNUC_MINOR__) == ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LT(major, minor) 	((__GNUC__ * 100 + __GNUC_MINOR__) < ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LE(major, minor) 	((__GNUC__ * 100 + __GNUC_MINOR__) <= ((major) * 100 + (minor)))
#	define TB_COMPILER_STRING						"GNU C/C++"
#	if  __GNUC__ == 2
#		if __GNUC_MINOR__ < 95
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ <2.95"
#		elif __GNUC_MINOR__ == 95
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 2.95"
#		elif __GNUC_MINOR__ == 96
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 2.96"
#		else
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ > 2.9 6&& < 3.0"
#		endif
#	elif __GNUC__ == 3
#		if __GNUC_MINOR__ == 2
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 3.2"
#		elif __GNUC_MINOR__ == 3
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 3.3"
#		elif __GNUC_MINOR__ == 4
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ 3.4"
#		else
#			define TB_COMPILER_VERSION_STRING		"GNU C/C++ > 3.4 && < 4.0"
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
# 	define TB_COMPILER_VERSION_BT(major, minor) 	(__WATCOMC__ > ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_BE(major, minor) 	(__WATCOMC__ >= ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_EQ(major, minor) 	(__WATCOMC__ == ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LT(major, minor) 	(__WATCOMC__ < ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LE(major, minor) 	(__WATCOMC__ <= ((major) * 100 + (minor)))
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
# 	define TB_COMPILER_VERSION_BT(major, minor) 	(__DMC__ > ((major) * 256 + (minor)))
# 	define TB_COMPILER_VERSION_BE(major, minor) 	(__DMC__ >= ((major) * 256 + (minor)))
# 	define TB_COMPILER_VERSION_EQ(major, minor) 	(__DMC__ == ((major) * 256 + (minor)))
# 	define TB_COMPILER_VERSION_LT(major, minor) 	(__DMC__ < ((major) * 256 + (minor)))
# 	define TB_COMPILER_VERSION_LE(major, minor) 	(__DMC__ <= ((major) * 256 + (minor)))
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
# 	define TB_COMPILER_VERSION_BT(major, minor) 	(__VECTORC > (major))
# 	define TB_COMPILER_VERSION_BE(major, minor) 	(__VECTORC >= (major))
# 	define TB_COMPILER_VERSION_EQ(major, minor) 	(__VECTORC == (major))
# 	define TB_COMPILER_VERSION_LT(major, minor) 	(__VECTORC < (major))
# 	define TB_COMPILER_VERSION_LE(major, minor) 	(__VECTORC <= (major))
#	define TB_COMPILER_VERSION_STRING 				"CodePlay VectorC C/C++"
#	if (__VECTORC == 1)
#		define TB_COMPILER_VERSION_STRING 			"CodePlay VectorC C/C++"
#	else
#		error Unknown CodePlay VectorC C++ Compiler Version
#	endif

// visual c++
#elif defined(_MSC_VER)
#	define TB_COMPILER_IS_MSVC
# 	define TB_COMPILER_VERSION_BT(major, minor) 	(_MSC_VER > ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_BE(major, minor) 	(_MSC_VER >= ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_EQ(major, minor) 	(_MSC_VER == ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LT(major, minor) 	(_MSC_VER < ((major) * 100 + (minor)))
# 	define TB_COMPILER_VERSION_LE(major, minor) 	(_MSC_VER <= ((major) * 100 + (minor)))
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


#endif


