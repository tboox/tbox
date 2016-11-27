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
 * @file        coroutine.h
 * @ingroup     coroutine
 *
 */
#ifndef TB_COROUTINE_STACKLESS_H
#define TB_COROUTINE_STACKLESS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "core.h"
#include "scheduler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/*!
 *
 * @code
 *
    // before
    tb_lo_coroutine_enter(co);
    for (i = 0; i < 100; i++)
    {
        tb_lo_coroutine_yield();
    }
    tb_lo_coroutine_leave();


    // after expanding
    { tb_lo_coroutine_ref_t co__ = co; tb_int_t lo_yield_flag__ = 1; tb_lo_core_resume(co);
    for (i = 0; i < 100; i++)
    {
        lo_yield_flag__ = 0;
        tb_lo_core_record(co__);
        if (lo_yield_flag__ == 0)
            return ; 
    }
    tb_lo_core_exit(co__); lo_yield_flag__ = 0; return ; }


    // after expanding again (init: branch = 0, state = TB_STATE_READY)
    { 
        tb_lo_coroutine_ref_t co__ = co;
        tb_int_t lo_yield_flag__ = 1; 
        switch (tb_lo_core(co__)->branch) 
        {
        case 0:;
            for (i = 0; i < 100; i++)
            {
                lo_yield_flag__ = 0;
                tb_lo_core(co__)->branch = __tb_line__; case __tb_line__:;
                if (lo_yield_flag__ == 0)
                    return ; 
            }
        } 
        tb_lo_core(co__)->branch = 0;
        tb_lo_core(co__)->state = TB_STATE_END; 
        lo_yield_flag__ = 0;  
        return ; 
    }

 * @endcode
 */
#define tb_lo_coroutine_enter(co)   { tb_lo_coroutine_ref_t co__ = (co); tb_int_t lo_yield_flag__ = 1; tb_lo_core_resume(co__)
#define tb_lo_coroutine_yield() \
do \
{ \
    lo_yield_flag__ = 0; \
    tb_lo_core_record(co__); \
    if (lo_yield_flag__ == 0) \
        return ; \
    \
} while(0)
#define tb_lo_coroutine_leave()     tb_lo_core_exit(co__); lo_yield_flag__ = 0; return ; }

/*! suspend coroutine
 *
 * the scheduler will move this coroutine to the suspended coroutines after the function be returned
 *
 * @code
 *
    // before
    tb_lo_coroutine_enter(co);
    for (i = 0; i < 100; i++)
    {
        tb_lo_coroutine_yield();
        tb_lo_coroutine_suspend();
    }
    tb_lo_coroutine_leave();

    // after expanding again (init: branch = 0, state = TB_STATE_READY)
    { 
        tb_lo_coroutine_ref_t co__ = co;
        tb_int_t lo_yield_flag__ = 1; 
        switch (tb_lo_core(co__)->branch) 
        {
        case 0:;
            for (i = 0; i < 100; i++)
            {
                lo_yield_flag__ = 0;
                tb_lo_core(co__)->branch = __tb_line__; case __tb_line__:;
                if (lo_yield_flag__ == 0)
                    return ; 

                // suspend coroutine
                tb_lo_core(co__)->state = TB_STATE_SUSPEND;
                tb_lo_core(co__)->branch = __tb_line__; case __tb_line__:;
                if (tb_lo_core(co__)->state == TB_STATE_SUSPEND)
                    return ; 
            }
        } 
        lo_yield_flag__ = 0;  
        tb_lo_core(co__)->branch = 0;
        tb_lo_core_state_set(co__, TB_STATE_END); 
        return ; 
    }

 * @endcode
 */
#define tb_lo_coroutine_suspend() \
do \
{ \
    tb_lo_core_state_set(co__, TB_STATE_SUSPEND); \
    tb_lo_core_record(co__); \
    if (tb_lo_core_state(co__) == TB_STATE_SUSPEND) \
        return ; \
    \
} while(0)

/// resume coroutine
#define tb_lo_coroutine_resume()        tb_lo_core_state_set(co__, TB_STATE_READY)

/// the self coroutine
#define tb_lo_coroutine_self()          (co__)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the scheduler of coroutine
 *
 * @param coroutine     the coroutine 
 *
 * @return              the scheduler
 */
tb_lo_scheduler_ref_t   tb_lo_coroutine_scheduler(tb_lo_coroutine_ref_t coroutine);

/*! start coroutine 
 *
 * @param scheduler     the scheduler (can not be null)
 * @param func          the coroutine function
 * @param priv          the passed user private data as the argument of function
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_lo_coroutine_start(tb_lo_scheduler_ref_t scheduler, tb_lo_coroutine_func_t func, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
