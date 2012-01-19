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
 * \file		event.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../event.h"
#include <stdio.h>
#include <errno.h>
#include <unistd.h>

/* /////////////////////////////////////////////////////////
 * implemention
 */

#if defined(TB_CONFIG_EVENT_HAVE_POSIX)
# 	include "event/posix.c"
#elif defined(TB_CONFIG_EVENT_HAVE_PTHREAD)
# 	include "event/pthread.c"
#elif defined(TB_CONFIG_EVENT_HAVE_SYSTEMV)
# 	include "event/systemv.c"
#else
# 	include "../generic/event.c"
#endif

