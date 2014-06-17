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
 * @ingroup     platform
 */
#ifndef TB_PLATFORM_ASIO_PREFIX_H
#define TB_PLATFORM_ASIO_PREFIX_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../prefix.h"
#include "../../asio/asio.h"
#include "../../math/math.h"
#include "../../memory/memory.h"
#include "../../platform/platform.h"
#include "../../container/container.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */
static __tb_inline__ tb_bool_t tb_aico_is_killed(tb_handle_t aico)
{
    // check
    tb_assert_and_check_return_val(aico, tb_false);

    // the state
    tb_size_t state = tb_atomic_get(&((tb_aico_t*)aico)->state);

    // killing or exiting or killed?
    return (state == TB_STATE_KILLING) || (state == TB_STATE_EXITING) || (state == TB_STATE_KILLED);
}
static __tb_inline__ tb_size_t tb_aice_priority(tb_aice_t const* aice)
{
    // the priorities
    static tb_size_t s_priorities[] =
    {
        1

    ,   1   
    ,   0   // acpt
    ,   0   // conn
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1
    ,   1

    ,   1
    ,   1
    ,   1
    ,   1
    ,   1

    ,   0   // task
    };
    tb_assert_and_check_return_val(aice->code && aice->code < tb_arrayn(s_priorities), 1);
    
    // the priority
    return s_priorities[aice->code];
}

#endif
