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
 * @author		ruki
 * @file		tbox.h
 *
 */
#ifndef TB_TBOX_H
#define TB_TBOX_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "algorithm/algorithm.h"
#include "container/container.h"
#include "platform/platform.h"
#include "network/network.h"
#include "charset/charset.h"
#include "memory/memory.h"
#include "filter/filter.h"
#include "stream/stream.h"
#include "string/string.h"
#include "object/object.h"
#include "utils/utils.h"
#include "math/math.h"
#include "libc/libc.h"
#include "libm/libm.h"
#include "asio/asio.h"
#include "xml/xml.h"
#include "zip/zip.h"
#include "sql/sql.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the compile mode
#define TB_MODE_DEBUG 			(1)
#define TB_MODE_SMALL 			(2)

#ifdef __tb_debug__
# 	define __tb_mode_debug__ 	TB_MODE_DEBUG
#else
# 	define __tb_mode_debug__ 	(0)
#endif

#ifdef __tb_small__
# 	define __tb_mode_small__ 	TB_MODE_SMALL
#else
# 	define __tb_mode_small__ 	(0)
#endif

// init tbox
#define tb_init(data, size) 	tb_init_and_check(data, size, (tb_size_t)(__tb_mode_debug__ | __tb_mode_small__), TB_VERSION_BUILD)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the tbox library
 *
 * @param data 		the memory data, using the native memory if be tb_null
 * @param size 		the memory size, using the native memory if be zero
 * @param mode 		the compile mode for check __tb_small__ and __tb_debug__
 * @param build 	the build version
 *
 * @return 			tb_true or tb_false
 */
tb_bool_t 			tb_init_and_check(tb_byte_t* data, tb_size_t size, tb_size_t mode, tb_hize_t build);

/// exit the tbox library
tb_void_t 			tb_exit(tb_noarg_t);

/// the tbox version string
tb_version_t const*	tb_version(tb_noarg_t);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
