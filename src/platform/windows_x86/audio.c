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
 * \file		audio.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * macros
 */

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_size_t tb_audio_get_volume()
{
	tb_trace_noimpl();
	return 0;
}
tb_bool_t tb_audio_set_volume(tb_size_t volume)
{
	tb_trace_noimpl();
	return TB_FALSE;
}
tb_handle_t tb_audio_open(tb_audio_fmt_t format, tb_audio_rate_t sample_rate, tb_audio_channel_t channel, tb_bool_t is_block)
{
	tb_trace_noimpl();
	return TB_NULL;
}
tb_int_t tb_audio_write(tb_handle_t haudio, tb_byte_t const* data, tb_size_t size)
{
	tb_trace_noimpl();
	return -1;
}
tb_void_t tb_audio_close(tb_handle_t haudio)
{
	tb_trace_noimpl();
}
tb_size_t tb_audio_bsize(tb_handle_t haudio)
{
	tb_trace_noimpl();
	return 0;
}
tb_void_t tb_audio_clear(tb_handle_t haudio)
{
	tb_trace_noimpl();
}
tb_void_t tb_audio_pause(tb_handle_t haudio)
{
	tb_trace_noimpl();
}
tb_void_t tb_audio_resume(tb_handle_t haudio)
{
	tb_trace_noimpl();
}
