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
 * @file		exception.h
 *
 */
#ifndef TB_PLATFORM_EXCEPTION_H
#define TB_PLATFORM_EXCEPTION_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#if defined(TB_CONFIG_OS_WINDOWS)
# 	include "windows/exception.h"
#endif

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifndef __tb_try
# 	define __tb_try 				if (1)
#endif

#ifndef __tb_except
# 	define __tb_except 				if (0)
#endif

#ifndef __tb_end
# 	define __tb_end 				
#endif

#endif


