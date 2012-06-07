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
 * @file		arch.h
 *
 */
#ifndef TB_PREFIX_ARCH_H
#define TB_PREFIX_ARCH_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "config.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#if defined(TB_CONFIG_ARCH_x86)
# 	define TB_ARCH_x86
#elif defined(TB_CONFIG_ARCH_x64)
# 	define TB_ARCH_x64
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

#endif


