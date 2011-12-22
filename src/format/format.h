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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		format.h
 *
 */

#ifndef TB_FORMAT_H
#define TB_FORMAT_H

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../stream/stream.h"

/* /////////////////////////////////////////////////////////
 * macros
 */
#define TB_FORMAT_SCORE_MAX 		(100)

/* /////////////////////////////////////////////////////////
 * types
 */

// the format type
typedef enum __tb_format_type_t
{
	TB_FORMAT_TYPE_NUL 	= 0

	// video
,	TB_FORMAT_TYPE_FLV 	= 1
,	TB_FORMAT_TYPE_AVI 	= 2

	// audio
,	TB_FORMAT_TYPE_WAV 	= 3
,	TB_FORMAT_TYPE_MP3 	= 4

	// image
,	TB_FORMAT_TYPE_JPG 	= 5
,	TB_FORMAT_TYPE_GIF 	= 6
,	TB_FORMAT_TYPE_BMP 	= 7
,	TB_FORMAT_TYPE_PNG 	= 8

	// flash
,	TB_FORMAT_TYPE_SWF 	= 9

	// text
,	TB_FORMAT_TYPE_TXT 	= 10
,	TB_FORMAT_TYPE_XML 	= 11
,	TB_FORMAT_TYPE_HTM 	= 12
,	TB_FORMAT_TYPE_PDF 	= 13

	// zix
,	TB_FORMAT_TYPE_ZIP 	= 14
,	TB_FORMAT_TYPE_BZ2 	= 15
,	TB_FORMAT_TYPE_GZ 	= 16
,	TB_FORMAT_TYPE_7Z 	= 17


}tb_format_type_t;

// the format flag
typedef enum __tb_format_flag_t
{
	TB_FORMAT_FLAG_ALL = 0
,	TB_FORMAT_FLAG_TEXTT = 1
,	TB_FORMAT_FLAG_VIDEO = 2
,	TB_FORMAT_FLAG_AUDIO = 4
,	TB_FORMAT_FLAG_IMAGE = 8
,	TB_FORMAT_FLAG_FLASH = 16
,	TB_FORMAT_FLAG_STORE = 32

}tb_format_flag_t;

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
	tb_size_t 			(*probe)(tb_gstream_t* gst);
	
}tb_format_t;

/* /////////////////////////////////////////////////////////
 * interfaces
 */

// probe format
tb_format_t const* 	tb_format_probe(tb_gstream_t* gst, tb_size_t flag);

// video
tb_size_t 			tb_format_flv_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_avi_probe(tb_gstream_t* gst);

// audio
tb_size_t 			tb_format_wav_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_mp3_probe(tb_gstream_t* gst);

// image
tb_size_t 			tb_format_bmp_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_gif_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_jpg_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_png_probe(tb_gstream_t* gst);

// flash
tb_size_t 			tb_format_swf_probe(tb_gstream_t* gst);

// text
tb_size_t 			tb_format_xml_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_htm_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_pdf_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_txt_probe(tb_gstream_t* gst);

// store
tb_size_t 			tb_format_zip_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_bz2_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_gz_probe(tb_gstream_t* gst);
tb_size_t 			tb_format_7z_probe(tb_gstream_t* gst);

#endif

