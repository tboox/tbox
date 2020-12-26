/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        pollerdata.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_IMPL_POLLERDATA_H
#define TB_PLATFORM_IMPL_POLLERDATA_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "poller.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// fd to pointer
#define tb_fd2ptr(fd)             ((fd) >= 0? (tb_pointer_t)((tb_long_t)(fd) + 1) : tb_null)

// pointer to fd
#define tb_ptr2fd(ptr)            (tb_int_t)((ptr)? (((tb_long_t)(ptr)) - 1) : -1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the poller data type
typedef struct __tb_pollerdata_t
{
    // the poller data (fd => priv)
    tb_cpointer_t*          data;

    // the poller data maximum count
    tb_size_t               maxn;

}tb_pollerdata_t, *tb_pollerdata_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* init poller data
 *
 * @param pollerdata    the pollerdata
 */
tb_void_t               tb_pollerdata_init(tb_pollerdata_ref_t pollerdata);

/* exit poller data
 *
 * @param pollerdata    the pollerdata
 */
tb_void_t               tb_pollerdata_exit(tb_pollerdata_ref_t pollerdata);

/* clear poller data
 *
 * @param pollerdata    the pollerdata
 */
tb_void_t               tb_pollerdata_clear(tb_pollerdata_ref_t pollerdata);

/* set poller data
 *
 * @param pollerdata    the pollerdata
 * @param object        the poller object
 * @param priv          the poller private data
 */
tb_void_t               tb_pollerdata_set(tb_pollerdata_ref_t pollerdata, tb_poller_object_ref_t object, tb_cpointer_t priv);

/* get poller data
 *
 * @param pollerdata    the pollerdata
 * @param object        the poller object
 *
 * @return              the poller private data
 */
tb_cpointer_t           tb_pollerdata_get(tb_pollerdata_ref_t pollerdata, tb_poller_object_ref_t object);

/* reset poller data
 *
 * @param pollerdata    the pollerdata
 * @param object        the poller object
 */
tb_void_t               tb_pollerdata_reset(tb_pollerdata_ref_t pollerdata, tb_poller_object_ref_t object);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
