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
 * @file        fwatcher.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_FWATCHER_H
#define TB_PLATFORM_FWATCHER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "path.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the fwatcher ref type
typedef __tb_typeref__(fwatcher);

/// the fwatcher event enum
typedef enum __tb_fwatcher_event_e
{
    TB_FWATCHER_EVENT_NONE   = 0
,   TB_FWATCHER_EVENT_MODIFY = 1
,   TB_FWATCHER_EVENT_CREATE = 2
,   TB_FWATCHER_EVENT_DELETE = 4

}tb_fwatcher_event_e;

/// the fwatcher event type
typedef struct __tb_fwatcher_event_t
{
    tb_size_t           event;
    tb_char_t           filepath[TB_PATH_MAXN];

}tb_fwatcher_event_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the fwatcher directory watcher
 *
 * @return              the fwatcher
 */
tb_fwatcher_ref_t       tb_fwatcher_init(tb_noarg_t);

/*! exit the fwatcher
 *
 * @param fwatcher      the fwatcher
 */
tb_void_t               tb_fwatcher_exit(tb_fwatcher_ref_t fwatcher);

/*! add the watched directory, we can just watch single-level directory
 *
 * @param fwatcher      the fwatcher
 * @param watchdir      the watched directory
 * @param recursion     is recursion?
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_fwatcher_add(tb_fwatcher_ref_t fwatcher, tb_char_t const* watchdir, tb_bool_t recursion);

/*! remove the watched directory, we can just watch single-level directory
 *
 * @param fwatcher      the fwatcher
 * @param watchdir      the watched directory
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_fwatcher_remove(tb_fwatcher_ref_t fwatcher, tb_char_t const* watchdir);

/*! spank the fwatcher, break the tb_fwatcher_wait() and return all events
 *
 * @param fwatcher      the fwatcher
 */
tb_void_t               tb_fwatcher_spak(tb_fwatcher_ref_t fwatcher);

/*! wait the fwatcher event
 *
 * @param fwatcher      the fwatcher
 * @param event         the event
 * @param timeout       the timeout, infinity: -1
 *
 * @return              > 0: has event, 0: timeout, -1: failed
 */
tb_long_t               tb_fwatcher_wait(tb_fwatcher_ref_t fwatcher, tb_fwatcher_event_t* event, tb_long_t timeout);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__


#endif
