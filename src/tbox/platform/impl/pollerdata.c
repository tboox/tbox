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
 * @file        pollerdata.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pollerdata.h"
#include "../pipe.h"
#include "../thread_local.h"
#include "../../libc/libc.h"
#ifdef TB_CONFIG_OS_WINDOWS
#    include "../windows/windows.h"
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__
#   define TB_POLLERDATA_GROW     (64)
#else
#   define TB_POLLERDATA_GROW     (256)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
#ifdef TB_CONFIG_OS_WINDOWS
__tb_extern_c_enter__
HANDLE tb_pipe_file_handle(tb_pipe_file_ref_t file);
__tb_extern_c_leave__
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static tb_long_t tb_pollerdata_object2fd(tb_poller_object_ref_t object)
{
    // check
    tb_assert(object && object->ref.ptr);

    // get the fd
    tb_long_t fd;
#ifdef TB_CONFIG_OS_WINDOWS
    if (object->type == TB_POLLER_OBJECT_PIPE)
        fd = (tb_long_t)tb_pipe_file_handle(object->ref.pipe);
    else fd = tb_ptr2fd(object->ref.ptr);
#else
    fd = tb_ptr2fd(object->ref.ptr);
#endif
    tb_assert(fd > 0 && fd < TB_MAXS16);

    return fd;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_void_t tb_pollerdata_init(tb_pollerdata_ref_t pollerdata)
{
    // check
    tb_assert(pollerdata);

    // init it
    pollerdata->data = tb_null;
    pollerdata->maxn = 0;
}
tb_void_t tb_pollerdata_exit(tb_pollerdata_ref_t pollerdata)
{
    // check
    tb_assert(pollerdata);

    // exit poller data
    if (pollerdata->data) tb_free(pollerdata->data);
    pollerdata->data = tb_null;
    pollerdata->maxn = 0;
}
tb_void_t tb_pollerdata_clear(tb_pollerdata_ref_t pollerdata)
{
    // check
    tb_assert(pollerdata);

    // clear data
    if (pollerdata->data) tb_memset(pollerdata->data, 0, pollerdata->maxn * sizeof(tb_cpointer_t));
}
tb_cpointer_t tb_pollerdata_get(tb_pollerdata_ref_t pollerdata, tb_poller_object_ref_t object)
{
    // check
    tb_assert(pollerdata);

    // get the poller private data
    tb_long_t fd = tb_pollerdata_object2fd(object);
    return (pollerdata->data && fd < pollerdata->maxn)? pollerdata->data[fd] : tb_null;
}
tb_void_t tb_pollerdata_set(tb_pollerdata_ref_t pollerdata, tb_poller_object_ref_t object, tb_cpointer_t priv)
{
    // check
    tb_assert(pollerdata && object);

    // get fd
    tb_long_t fd = tb_pollerdata_object2fd(object);

    // no data? init it first
    tb_size_t need = fd + 1;
    if (!pollerdata->data)
    {
        // init data
        need += TB_POLLERDATA_GROW;
        pollerdata->data = tb_nalloc0_type(need, tb_cpointer_t);
        tb_assert_and_check_return(pollerdata->data);

        // init data size
        pollerdata->maxn = need;
    }
    else if (need > pollerdata->maxn)
    {
        // grow data
        need += TB_POLLERDATA_GROW;
        pollerdata->data = (tb_cpointer_t*)tb_ralloc(pollerdata->data, need * sizeof(tb_cpointer_t));
        tb_assert_and_check_return(pollerdata->data);

        // init growed space
        tb_memset(pollerdata->data + pollerdata->maxn, 0, (need - pollerdata->maxn) * sizeof(tb_cpointer_t));

        // grow data size
        pollerdata->maxn = need;
    }

    // save the poller private data
    pollerdata->data[fd] = priv;
}
tb_void_t tb_pollerdata_reset(tb_pollerdata_ref_t pollerdata, tb_poller_object_ref_t object)
{
    // check
    tb_assert(pollerdata && object);

    // get fd
    tb_long_t fd = tb_pollerdata_object2fd(object);

    // remove the poller private data
    if (fd < pollerdata->maxn) pollerdata->data[fd] = tb_null;
}
