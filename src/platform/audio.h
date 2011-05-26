/*!The Tiny Platform Library
 * 
 * TPlat is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TPlat is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TPlat; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		audio.h
 *
 */
#ifndef TB_PLATFORM_AUDIO_H
#define TB_PLATFORM_AUDIO_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif


/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */
// the audio format type
typedef enum __tb_audio_fmt_t
{
	TB_AUDIO_FMT_U8 		= 1
,	TB_AUDIO_FMT_S8 		= 2
,	TB_AUDIO_FMT_U16_LE 	= 3
,	TB_AUDIO_FMT_U16_BE 	= 4
,	TB_AUDIO_FMT_S16_LE 	= 5
,	TB_AUDIO_FMT_S16_BE 	= 6

}tb_audio_fmt_t;

// the audio sample rate type
typedef enum __tb_audio_rate_t
{
	TB_AUDIO_RATE_5512 		= 1
,	TB_AUDIO_RATE_11025 		= 2
,	TB_AUDIO_RATE_22050 		= 3
,	TB_AUDIO_RATE_44100 		= 4
,	TB_AUDIO_RATE_88200 		= 5

}tb_audio_rate_t;

// the audio sample channel type
typedef enum __tb_audio_channel_t
{
	TB_AUDIO_CHANNEL_MONO 	= 1
,	TB_AUDIO_CHANNEL_STEREO 	= 2

}tb_audio_channel_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// get & set volume
tb_size_t 	tb_audio_get_volume();
tb_bool_t 	tb_audio_set_volume(tb_size_t volume);

// open & close 
tb_handle_t 	tb_audio_open(tb_audio_fmt_t format, tb_audio_rate_t sample_rate, tb_audio_channel_t channel, tb_bool_t is_block);
void 			tb_audio_close(tb_handle_t haudio);

// buffer operations
tb_int_t 	tb_audio_write(tb_handle_t haudio, tb_byte_t const* data, tb_size_t size);
tb_size_t 	tb_audio_bsize(tb_handle_t haudio);
void 			tb_audio_clear(tb_handle_t haudio);
void 			tb_audio_pause(tb_handle_t haudio);
void 			tb_audio_resume(tb_handle_t haudio);
	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
