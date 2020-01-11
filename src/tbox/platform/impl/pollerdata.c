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
 * Copyright (C) 2009-2020, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        pollerdata.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "pollerdata.h"
#include "../thread_local.h"
#include "../../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef __tb_small__ 
#   define TB_POLLERDATA_GROW     (64)
#else
#   define TB_POLLERDATA_GROW     (256)
#endif

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

#ifdef TB_CONFIG_OS_WINDOWS
    // exit the pipe data
    if (pollerdata->pipedata) tb_hash_map_exit(pollerdata->pipedata);
    pollerdata->pipedata = tb_null;
#endif
}
tb_void_t tb_pollerdata_clear(tb_pollerdata_ref_t pollerdata)
{
    // check
    tb_assert(pollerdata);

    // clear data
    if (pollerdata->data) tb_memset(pollerdata->data, 0, pollerdata->maxn * sizeof(tb_cpointer_t));

#ifdef TB_CONFIG_OS_WINDOWS
    // clear the pipe data
    if (pollerdata->pipedata) tb_hash_map_clear(pollerdata->pipedata);
#endif
}
tb_void_t tb_pollerdata_set(tb_pollerdata_ref_t pollerdata, tb_poller_object_ref_t object, tb_cpointer_t priv)
{
    // check
    tb_assert(pollerdata && object);

#ifdef TB_CONFIG_OS_WINDOWS
    if (object->type == TB_POLLER_OBJECT_PIPE)
    {
        // init the pipe data if not exists
        if (!pollerdata->pipedata)
            pollerdata->pipedata = tb_hash_map_init(TB_HASH_MAP_BUCKET_SIZE_MICRO, tb_element_ptr(tb_null, tb_null), tb_element_ptr(tb_null, tb_null));
        tb_assert(pollerdata->pipedata);

        // save the private data for the pipe data 
        tb_hash_map_insert(pollerdata->pipedata, object->ref.ptr, priv);
        return ;
    }
#endif

    // get fd
    tb_long_t fd = tb_ptr2fd(object->ref.ptr);
    tb_assert(fd > 0 && fd < TB_MAXS32);

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

#ifdef TB_CONFIG_OS_WINDOWS
    // remove the private data for the pipe data 
    if (object->type == TB_POLLER_OBJECT_PIPE)
    {
        if (pollerdata->pipedata) tb_hash_map_remove(pollerdata->pipedata, object->ref.ptr);
        return ;
    }
#endif

    // get fd
    tb_long_t fd = tb_ptr2fd(object->ref.ptr);
    tb_assert(fd > 0 && fd < TB_MAXS32);

    // remove the poller private data
    if (fd < pollerdata->maxn) pollerdata->data[fd] = tb_null;
}
