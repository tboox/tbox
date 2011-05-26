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
#ifndef TPLAT_AUDIO_H
#define TPLAT_AUDIO_H

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
typedef enum __tplat_audio_fmt_t
{
	TPLAT_AUDIO_FMT_U8 		= 1
,	TPLAT_AUDIO_FMT_S8 		= 2
,	TPLAT_AUDIO_FMT_U16_LE 	= 3
,	TPLAT_AUDIO_FMT_U16_BE 	= 4
,	TPLAT_AUDIO_FMT_S16_LE 	= 5
,	TPLAT_AUDIO_FMT_S16_BE 	= 6

}tplat_audio_fmt_t;

// the audio sample rate type
typedef enum __tplat_audio_rate_t
{
	TPLAT_AUDIO_RATE_5512 		= 1
,	TPLAT_AUDIO_RATE_11025 		= 2
,	TPLAT_AUDIO_RATE_22050 		= 3
,	TPLAT_AUDIO_RATE_44100 		= 4
,	TPLAT_AUDIO_RATE_88200 		= 5

}tplat_audio_rate_t;

// the audio sample channel type
typedef enum __tplat_audio_channel_t
{
	TPLAT_AUDIO_CHANNEL_MONO 	= 1
,	TPLAT_AUDIO_CHANNEL_STEREO 	= 2

}tplat_audio_channel_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// get & set volume
tplat_size_t 	tplat_audio_get_volume();
tplat_bool_t 	tplat_audio_set_volume(tplat_size_t volume);

// open & close 
tplat_handle_t 	tplat_audio_open(tplat_audio_fmt_t format, tplat_audio_rate_t sample_rate, tplat_audio_channel_t channel, tplat_bool_t is_block);
void 			tplat_audio_close(tplat_handle_t haudio);

// buffer operations
tplat_int_t 	tplat_audio_write(tplat_handle_t haudio, tplat_byte_t const* data, tplat_size_t size);
tplat_size_t 	tplat_audio_bsize(tplat_handle_t haudio);
void 			tplat_audio_clear(tplat_handle_t haudio);
void 			tplat_audio_pause(tplat_handle_t haudio);
void 			tplat_audio_resume(tplat_handle_t haudio);
	
// c plus plus
#ifdef __cplusplus
}
#endif

#endif
