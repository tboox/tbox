/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TGraphic; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		format.h
 *
 */

#ifndef TB_FORMAT_H
#define TB_FORMAT_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "stream.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_FORMAT_SCORE_MAX 		(100)

/* /////////////////////////////////////////////////////////
 * types
 */

// the format type
typedef enum __tf_format_type_t
{
	TB_FORMAT_TYPE_NUL = 0

	// video
,	TB_FORMAT_TYPE_FLV = 1
,	TB_FORMAT_TYPE_AVI = 2

	// audio
,	TB_FORMAT_TYPE_WAV = 3
,	TB_FORMAT_TYPE_MP3 = 4

	// image
,	TB_FORMAT_TYPE_JPG = 5
,	TB_FORMAT_TYPE_GIF = 6
,	TB_FORMAT_TYPE_BMP = 7
,	TB_FORMAT_TYPE_PNG = 8

	// flash
,	TB_FORMAT_TYPE_SWF = 9

}tf_format_type_t;

// the format flag
typedef enum __tf_format_flag_t
{
	TB_FORMAT_FLAG_ALL = 0
,	TB_FORMAT_FLAG_VIDEO = 1
,	TB_FORMAT_FLAG_AUDIO = 2
,	TB_FORMAT_FLAG_IMAGE = 4
,	TB_FORMAT_FLAG_FLASH = 8

}tf_format_flag_t;

// the format type
typedef struct __tb_format_t
{
	// the format flag
	tb_size_t 			flag;

	// the format type
	tb_size_t 			type;

	// the format name
	tb_char_t const* 	name;

	// the probeor
	tb_size_t 			(*probe)(tb_stream_t* st);
	
}tb_format_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// probe format
tb_format_t const* 	tb_format_probe(tb_stream_t* st, tb_size_t flag);

// video
tb_size_t 			tb_format_flv_probe(tb_stream_t* st);
tb_size_t 			tb_format_avi_probe(tb_stream_t* st);

// audio
tb_size_t 			tb_format_wav_probe(tb_stream_t* st);
tb_size_t 			tb_format_mp3_probe(tb_stream_t* st);

// image
tb_size_t 			tb_format_bmp_probe(tb_stream_t* st);
tb_size_t 			tb_format_gif_probe(tb_stream_t* st);
tb_size_t 			tb_format_jpg_probe(tb_stream_t* st);
tb_size_t 			tb_format_png_probe(tb_stream_t* st);

// flash
tb_size_t 			tb_format_swf_probe(tb_stream_t* st);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif

