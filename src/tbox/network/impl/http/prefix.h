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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        prefix.h
 *
 */
#ifndef TB_NETWORK_IMPL_HTTP_PREFIX_H
#define TB_NETWORK_IMPL_HTTP_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../url.h"
#include "../../http.h"
#include "../../cookies.h"
#include "../../../libc/libc.h"
#include "../../../string/string.h"
#include "../../../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the http default timeout, 10s
#define TB_HTTP_DEFAULT_TIMEOUT                 (10000)

/// the http default redirect maxn
#define TB_HTTP_DEFAULT_REDIRECT                (10)

/// the http default port
#define TB_HTTP_DEFAULT_PORT                    (80)

/// the http default port for ssl
#define TB_HTTP_DEFAULT_PORT_SSL                (443)

#endif
