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
 * @ingroup     scheduler
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "scheduler.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_scheduler_ref_t tb_scheduler_init_with_fifo()
{
    return tb_null;
}
tb_scheduler_ref_t tb_scheduler_init_with_poll()
{
    return tb_null;
}
tb_void_t tb_scheduler_exit(tb_scheduler_ref_t scheduler)
{
}
tb_void_t tb_scheduler_loop(tb_scheduler_ref_t scheduler)
{
}
tb_size_t tb_scheduler_type(tb_scheduler_ref_t scheduler)
{
    return 0;
}
tb_size_t tb_scheduler_self()
{
    return 0;
}
tb_bool_t tb_scheduler_ctrl(tb_scheduler_ref_t scheduler, tb_size_t ctrl, ...)
{
    return tb_false;
}
