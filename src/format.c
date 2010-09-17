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
 * \file		format.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "format.h"

/* /////////////////////////////////////////////////////////
 * types
 */


/* /////////////////////////////////////////////////////////
 * globals
 */
static tb_format_t g_formats[] = 
{
	// video
	{TB_FORMAT_FLAG_VIDEO, TB_FORMAT_TYPE_FLV, "flv", tb_format_flv_detect}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_BMP, "bmp", tb_format_bmp_detect}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_GIF, "gif", tb_format_gif_detect}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_JPG, "jpg", tb_format_jpg_detect}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_PNG, "png", tb_format_png_detect}
,	{TB_FORMAT_FLAG_FLASH, TB_FORMAT_TYPE_SWF, "swf", tb_format_swf_detect}
,	{TB_FORMAT_FLAG_AUDIO, TB_FORMAT_TYPE_WAV, "wav", tb_format_wav_detect}
,	{TB_FORMAT_FLAG_AUDIO, TB_FORMAT_TYPE_MP3, "mp3", tb_format_mp3_detect}
,	{TB_FORMAT_FLAG_VIDEO, TB_FORMAT_TYPE_AVI, "avi", tb_format_avi_detect}

};

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_format_t const* tb_format_detect(tb_stream_t* st, tb_size_t flag)
{
	TB_ASSERT(st);
	if (!st) return TB_NULL;

	tb_size_t score_max = 0;
	tb_format_t const* format = TB_NULL;

	// detect format
	tb_int_t i = 0;
	tb_int_t n = TB_STATIC_ARRAY_SIZE(g_formats);
	for (i = 0; i < n; i++)
	{
		// filter 
		if (flag == TB_FORMAT_FLAG_ALL || g_formats[i].flag & flag)
		{
			// detect score
			tb_size_t score = 0;
			if (g_formats[i].detect) score = g_formats[i].detect(st);

			// save the best format
			if (score > score_max)
			{
				score_max = score;
				format = &g_formats[i];
			}

			// is the best?
			if (score == TB_FORMAT_SCORE_MAX) break;
		}
	}

	return format;
}
