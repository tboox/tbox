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
 * \file		reactor.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../event.h"
#include "../../../math/math.h"
#include "../../../memory/memory.h"
#include "../../../container/container.h"
#include <sys/types.h>
#include <sys/socket.h>

/* /////////////////////////////////////////////////////////
 * reactor
 */
#if defined(TB_CONFIG_EVENT_HAVE_EPOLL)
# 	include "reactor/epoll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_POLL)
# 	include "reactor/poll.c"
#elif defined(TB_CONFIG_EVENT_HAVE_PORT)
# 	include "reactor/port.c"
#elif defined(TB_CONFIG_EVENT_HAVE_SELECT)
# 	include "reactor/select.c"
#elif defined(TB_CONFIG_EVENT_HAVE_KQUEUE)
# 	include "reactor/kqueue.c"
#else
# 	error have not available event mode
#endif

