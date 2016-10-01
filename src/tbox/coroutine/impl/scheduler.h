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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        scheduler.h
 * @ingroup     coroutine
 *
 */
#ifndef TB_COROUTINE_IMPL_SCHEDULER_H
#define TB_COROUTINE_IMPL_SCHEDULER_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "coroutine.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// get running coroutine
#define tb_scheduler_running(scheduler)           ((scheduler)->running)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the scheduler type
typedef struct __tb_scheduler_t
{   
    // the scheduler type
    tb_uint8_t              type;

    // the running coroutine
    tb_coroutine_t*         running;

    /* ready the given coroutine
     *
     * @param scheduler     the scheduler
     * @param coroutine     the coroutine
     *
     * @return              tb_true or tb_false
     */
    tb_bool_t               (*ready)(struct __tb_scheduler_t* scheduler, tb_coroutine_t* coroutine);

    /* yield the given coroutine
     *
     * @param scheduler     the scheduler
     * @param coroutine     the coroutine
     */
    tb_void_t               (*yield)(struct __tb_scheduler_t* scheduler, tb_coroutine_t* coroutine);

    /* sleep the given coroutine
     *
     * @param scheduler     the scheduler
     * @param coroutine     the coroutine
     * @param interval      the interval (ms)
     */
    tb_void_t               (*sleep)(struct __tb_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_size_t interval);

    /* control scheduler 
     *
     * @param scheduler     the scheduler
     * @param ctrl          the control code
     * @param args          the arguments
     *
     * @return              tb_true or tb_false
     */
    tb_bool_t               (*ctrl)(struct __tb_scheduler_t* scheduler, tb_size_t ctrl, tb_va_list_t args);

    /* run scheduler loop
     *
     * @param scheduler     the scheduler
     */
    tb_void_t               (*loop)(struct __tb_scheduler_t* scheduler);

    /* exit scheduler
     *
     * @param scheduler     the scheduler
     */
    tb_void_t               (*exit)(struct __tb_scheduler_t* scheduler);

}tb_scheduler_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/* ready the coroutine 
 *
 * @param scheduler         the scheduler
 * @param coroutine         the coroutine
 *
 * @return                  tb_true or tb_false
 */
tb_bool_t                   tb_scheduler_ready(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine);

/* yield the given coroutine
 *
 * @param scheduler         the scheduler
 * @param coroutine         the coroutine
 */
tb_void_t                   tb_scheduler_yield(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine);

/* sleep the given coroutine
 *
 * @param scheduler         the scheduler
 * @param coroutine         the coroutine
 * @param interval          the interval (ms)
 */
tb_void_t                   tb_scheduler_sleep(tb_scheduler_t* scheduler, tb_coroutine_t* coroutine, tb_size_t interval);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
