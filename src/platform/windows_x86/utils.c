/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		utils.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../../libc/libc.h"
#include "../../math/math.h"

/* /////////////////////////////////////////////////////////
 * implemention
 */

// usleep
tb_void_t tb_usleep(tb_size_t us)
{
	TB_NOT_IMPLEMENT();
}

// msleep
tb_void_t tb_msleep(tb_size_t ms)
{
	TB_NOT_IMPLEMENT();
}

// sleep
tb_void_t tb_sleep(tb_size_t s)
{
	TB_NOT_IMPLEMENT();
}

// printf
tb_void_t tb_printf(tb_char_t const* fmt, ...)
{
	TB_NOT_IMPLEMENT();
}

// mclock
tb_int64_t tb_mclock()
{
	TB_NOT_IMPLEMENT();
	return TB_INT64_ZERO;
}

// uclock
tb_int64_t tb_uclock()
{
	TB_NOT_IMPLEMENT();
	return TB_INT64_ZERO;
}
tb_int64_t tb_time()
{
	TB_NOT_IMPLEMENT();
	return TB_INT64_ZERO;
}


