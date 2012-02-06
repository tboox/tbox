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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * \author		ruki
 * \file		waito.c
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * implemention
 */
static tb_long_t tb_aioo_reactor_waito_wait(tb_aioo_t* object, tb_long_t timeout)
{
	tb_assert_and_check_return_val(object && object->handle, -1);

	// type
	tb_size_t otype = object->otype;
	tb_size_t etype = object->etype;
	
	// check
	tb_assert_and_check_return_val(otype == TB_AIOO_OTYPE_FILE, -1);

	// select
	DWORD r = WaitForSingleObject(object->handle, timeout >= 0? timeout : INFINITE);
	tb_assert_and_check_return_val(r != WAIT_FAILED, -1);

	// timeout?
	tb_check_return_val(r != WAIT_TIMEOUT, 0);

	// error?
	tb_check_return_val(r == WAIT_OBJECT_0, -1);

	// ok
	return etype;
}

