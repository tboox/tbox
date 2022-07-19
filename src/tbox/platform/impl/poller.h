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
 * @file        poller.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_IMPL_POLLER_H
#define TB_PLATFORM_IMPL_POLLER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../poller.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// get the poller object type from the private pointer
#define tb_poller_priv_get_object_type(ptr) (((tb_size_t)(ptr) & ((tb_size_t)0x1 << (TB_CPU_BITSIZE - 1)))? TB_POLLER_OBJECT_PIPE : TB_POLLER_OBJECT_SOCK)

// get the original private pointer
#define tb_poller_priv_get_original(ptr)    ((tb_cpointer_t)((tb_size_t)(ptr) & ~((tb_size_t)0x1 << (TB_CPU_BITSIZE - 1))))

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the process poller ref type
typedef __tb_typeref__(poller_process);

/// the fwatcher poller ref type
typedef __tb_typeref__(poller_fwatcher);

// the poller type
typedef struct __tb_poller_t
{
    // the user private data
    tb_cpointer_t            priv;

    // the poller type
    tb_uint16_t              type;

    // the supported events
    tb_uint16_t              supported_events;

#ifndef TB_CONFIG_MICRO_ENABLE
    // the process poller
    tb_poller_process_ref_t  process_poller;

    // the fwatcher poller
    tb_poller_fwatcher_ref_t fwatcher_poller;
#endif

    /* exit poller
     *
     * @param poller         the poller
     */
    tb_void_t                (*exit)(struct __tb_poller_t* poller);

    /* kill all waited events, tb_poller_wait() will return -1
     *
     * @param poller         the poller
     */
    tb_void_t                (*kill)(struct __tb_poller_t* poller);

    /* spak the poller, break the tb_poller_wait() and return all events
     *
     * @param poller         the poller
     */
    tb_void_t                (*spak)(struct __tb_poller_t* poller);

    /* wait events for all objects
     *
     * @param poller         the poller
     * @param func           the events function
     * @param timeout        the timeout, infinity: -1
     *
     * @return               > 0: the events number, 0: timeout, -1: failed
     */
    tb_long_t                (*wait)(struct __tb_poller_t* poller, tb_poller_event_func_t func, tb_long_t timeout);

    /* insert socket to poller
     *
     * @param poller         the poller
     * @param object         the poller object
     * @param events         the poller events
     * @param priv           the private data
     *
     * @return               tb_true or tb_false
     */
    tb_bool_t                (*insert)(struct __tb_poller_t* poller, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv);

    /* remove socket from poller
     *
     * @param poller         the poller
     * @param object         the poller object
     *
     * @return               tb_true or tb_false
     */
    tb_bool_t                (*remove)(struct __tb_poller_t* poller, tb_poller_object_ref_t object);

    /* modify events for the given socket
     *
     * @param poller         the poller
     * @param object         the poller object
     * @param events         the poller events
     * @param priv           the private data
     *
     * @return               tb_true or tb_false
     */
    tb_bool_t                (*modify)(struct __tb_poller_t* poller, tb_poller_object_ref_t object, tb_size_t events, tb_cpointer_t priv);

    /* attach the poller to the current thread (only for windows/iocp now)
     *
     * @param poller         the poller
     */
    tb_void_t                (*attach)(struct __tb_poller_t* poller);

}tb_poller_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * inline implementation
 */

// set the object type to the private pointer
static __tb_inline__ tb_cpointer_t tb_poller_priv_set_object_type(tb_poller_object_ref_t object, tb_cpointer_t ptr)
{
    // must be a valid pointer address
    tb_assert(!((tb_size_t)ptr & ((tb_size_t)0x1 << (TB_CPU_BITSIZE - 1))));
    return object->type == TB_POLLER_OBJECT_PIPE? (tb_cpointer_t)((tb_size_t)ptr | ((tb_size_t)0x1 << (TB_CPU_BITSIZE - 1))) : ptr;
}

#endif
