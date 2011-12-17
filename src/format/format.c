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
	{TB_FORMAT_FLAG_VIDEO, TB_FORMAT_TYPE_FLV, "flv", tb_format_flv_probe}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_BMP, "bmp", tb_format_bmp_probe}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_GIF, "gif", tb_format_gif_probe}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_JPG, "jpg", tb_format_jpg_probe}
,	{TB_FORMAT_FLAG_IMAGE, TB_FORMAT_TYPE_PNG, "png", tb_format_png_probe}
,	{TB_FORMAT_FLAG_FLASH, TB_FORMAT_TYPE_SWF, "swf", tb_format_swf_probe}
,	{TB_FORMAT_FLAG_AUDIO, TB_FORMAT_TYPE_WAV, "wav", tb_format_wav_probe}
,	{TB_FORMAT_FLAG_AUDIO, TB_FORMAT_TYPE_MP3, "mp3", tb_format_mp3_probe}
,	{TB_FORMAT_FLAG_VIDEO, TB_FORMAT_TYPE_AVI, "avi", tb_format_avi_probe}
,	{TB_FORMAT_FLAG_STORE, TB_FORMAT_TYPE_ZIP, "zip", tb_format_zip_probe}
,	{TB_FORMAT_FLAG_STORE, TB_FORMAT_TYPE_7Z, "7z", tb_format_7z_probe}
,	{TB_FORMAT_FLAG_STORE, TB_FORMAT_TYPE_GZ, "gz", tb_format_gz_probe}
, 	{TB_FORMAT_FLAG_STORE, TB_FORMAT_TYPE_BZ2, "bz2", tb_format_bz2_probe}
,	{TB_FORMAT_FLAG_TEXTT, TB_FORMAT_TYPE_PDF, "pdf", tb_format_pdf_probe}
,	{TB_FORMAT_FLAG_TEXTT, TB_FORMAT_TYPE_XML, "xml", tb_format_xml_probe}
,	{TB_FORMAT_FLAG_TEXTT, TB_FORMAT_TYPE_HTM, "htm", tb_format_htm_probe}
,	{TB_FORMAT_FLAG_TEXTT, TB_FORMAT_TYPE_TXT, "txt", tb_format_txt_probe}

};

/* /////////////////////////////////////////////////////////
 * interfaces
 */
tb_format_t const* tb_format_probe(tb_gstream_t* gst, tb_size_t flag)
{
	tb_assert_and_check_return_val(gst, TB_NULL);

	tb_size_t score_max = 0;
	tb_size_t score_total = 0;
	tb_format_t const* format = TB_NULL;

	// probe format
	tb_int_t i = 0;
	tb_int_t n = tb_arrayn(g_formats);
	for (i = 0; i < n; i++)
	{
		// filter 
		if (flag == TB_FORMAT_FLAG_ALL || g_formats[i].flag & flag)
		{
			// probe score
			tb_size_t score = 0;
			if (g_formats[i].probe) score = g_formats[i].probe(gst);

			// compute the total score
			score_total += score;

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

	// no match format
	if (!score_total) return TB_NULL;

	tb_trace("match: %d%%", score_max * 100 / score_total);
	return format;
}
