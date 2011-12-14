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
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <linux/soundcard.h>
/* /////////////////////////////////////////////////////////
 * macros
 */
// the audio device name 
#define TB_AUDIO_DEVNAME 			"/dev/dsp"

/* /////////////////////////////////////////////////////////
 * implemention
 */
tb_size_t tb_audio_get_volume()
{
	return 0;
}
tb_bool_t tb_audio_set_volume(tb_size_t volume)
{
	return TB_TRUE;
}
tb_handle_t tb_audio_open(tb_audio_fmt_t format, tb_audio_rate_t sample_rate, tb_audio_channel_t channel, tb_bool_t is_block)
{
	// open audio device 
	tb_int_t fd = open(TB_AUDIO_DEVNAME, O_WRONLY);
	if (fd < 0) 
	{
		tb_trace("cannot open audio device:%s", TB_AUDIO_DEVNAME);
		return TB_NULL;
	}

#if 0
	tb_int_t quantify_bits = 16;
	if (ioctl(fd, SOUND_PCM_WRITE_RATE, &sample_rate) == -1) return TB_NULL;
	if (ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &channels) == -1) return TB_NULL;
	if (ioctl(fd, SOUND_PCM_WRITE_BITS, &quantify_bits) == -1) return TB_NULL;
#else

	tb_int_t fmt = AFMT_U8;
	switch (format)
	{
	case TB_AUDIO_FMT_U8:
		fmt = AFMT_U8;
		break;
	case TB_AUDIO_FMT_S8:
		fmt = AFMT_S8;
		break;
	case TB_AUDIO_FMT_S16_LE:
		fmt = AFMT_S16_LE;
		break;
	case TB_AUDIO_FMT_S16_BE:
		fmt = AFMT_S16_BE;
		break;
	case TB_AUDIO_FMT_U16_LE:
		fmt = AFMT_U16_LE;
		break;
	case TB_AUDIO_FMT_U16_BE:
		fmt = AFMT_U16_BE;
		break;
	default:
		goto fail;
	}

	tb_int_t channels_n = 1;
	if (channel == TB_AUDIO_CHANNEL_MONO) channels_n = 1;
	else if (channel == TB_AUDIO_CHANNEL_STEREO) channels_n = 2;
	else goto fail;

	tb_int_t rate = 0;
	switch (sample_rate)
	{
	case TB_AUDIO_RATE_5512:
		rate = 5512;
		break;
	case TB_AUDIO_RATE_11025:
		rate = 11025;
		break;
	case TB_AUDIO_RATE_22050:
		rate = 22050;
		break;
	case TB_AUDIO_RATE_44100:
		rate = 44100;
		break;
	case TB_AUDIO_RATE_88200:
		rate = 88200;
		break;
	default:
		goto fail;
	}

	if (ioctl(fd, SNDCTL_DSP_SETFMT, &fmt) == -1) goto fail;
	if (ioctl(fd, SNDCTL_DSP_CHANNELS, &channels_n) == -1) goto fail;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &rate) == -1) goto fail;

	if (is_block == TB_TRUE) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
	else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

#endif

	return ((tb_handle_t)fd);

fail:
	if (fd >= 0) close(fd);
	return TB_NULL;
}
tb_int_t tb_audio_writ(tb_handle_t haudio, tb_byte_t const* data, tb_size_t size)
{
	tb_int_t fd = (tb_int_t)haudio;
	if (fd >= 0) return writ(fd, data, size);
	else return -1;
}
tb_void_t tb_audio_close(tb_handle_t haudio)
{
	tb_int_t fd = (tb_int_t)haudio;
	if (fd >= 0) close(fd);
}
tb_size_t tb_audio_bsize(tb_handle_t haudio)
{
	tb_int_t fd = (tb_int_t)haudio;
	if (fd >= 0)
	{
		tb_int_t sample_rate = 0;
		tb_int_t channels = 0;
		tb_int_t quantify_bits = 0;
		if (ioctl(fd, SOUND_PCM_READ_RATE, &sample_rate) == -1) return 0;
		if (ioctl(fd, SOUND_PCM_READ_CHANNELS, &channels) == -1) return 0;
		if (ioctl(fd, SOUND_PCM_READ_BITS, &quantify_bits) == -1) return 0;
		tb_trace("bsize: %d x %d x %d / 8 = %d kb", sample_rate, channels, quantify_bits, ((sample_rate * channels * quantify_bits) >> 3) / 1024);
		return ((sample_rate * channels * quantify_bits) >> 3);
	}
	return 0;
}
tb_void_t tb_audio_clear(tb_handle_t haudio)
{
}
tb_void_t tb_audio_pause(tb_handle_t haudio)
{
}
tb_void_t tb_audio_resume(tb_handle_t haudio)
{
}
