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
 * \file		audio.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "../tplat.h"
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
#define TPLAT_AUDIO_DEVNAME 			"/dev/dsp"

/* /////////////////////////////////////////////////////////
 * implemention
 */
tplat_size_t tplat_audio_get_volume()
{
	return 0;
}
tplat_bool_t tplat_audio_set_volume(tplat_size_t volume)
{
	return TPLAT_TRUE;
}
tplat_handle_t tplat_audio_open(tplat_audio_fmt_t format, tplat_audio_rate_t sample_rate, tplat_audio_channel_t channel, tplat_bool_t is_block)
{
	// open audio device 
	tplat_int_t fd = open(TPLAT_AUDIO_DEVNAME, O_WRONLY);
	if (fd < 0) 
	{
		TPLAT_DBG("cannot open audio device:%s", TPLAT_AUDIO_DEVNAME);
		return TPLAT_INVALID_HANDLE;
	}

#if 0
	tplat_int_t quantify_bits = 16;
	if (ioctl(fd, SOUND_PCM_WRITE_RATE, &sample_rate) == -1) return TPLAT_INVALID_HANDLE;
	if (ioctl(fd, SOUND_PCM_WRITE_CHANNELS, &channels) == -1) return TPLAT_INVALID_HANDLE;
	if (ioctl(fd, SOUND_PCM_WRITE_BITS, &quantify_bits) == -1) return TPLAT_INVALID_HANDLE;
#else

	tplat_int_t fmt = AFMT_U8;
	switch (format)
	{
	case TPLAT_AUDIO_FMT_U8:
		fmt = AFMT_U8;
		break;
	case TPLAT_AUDIO_FMT_S8:
		fmt = AFMT_S8;
		break;
	case TPLAT_AUDIO_FMT_S16_LE:
		fmt = AFMT_S16_LE;
		break;
	case TPLAT_AUDIO_FMT_S16_BE:
		fmt = AFMT_S16_BE;
		break;
	case TPLAT_AUDIO_FMT_U16_LE:
		fmt = AFMT_U16_LE;
		break;
	case TPLAT_AUDIO_FMT_U16_BE:
		fmt = AFMT_U16_BE;
		break;
	default:
		goto fail;
	}

	tplat_int_t channels_n = 1;
	if (channel == TPLAT_AUDIO_CHANNEL_MONO) channels_n = 1;
	else if (channel == TPLAT_AUDIO_CHANNEL_STEREO) channels_n = 2;
	else goto fail;

	tplat_int_t rate = 0;
	switch (sample_rate)
	{
	case TPLAT_AUDIO_RATE_5512:
		rate = 5512;
		break;
	case TPLAT_AUDIO_RATE_11025:
		rate = 11025;
		break;
	case TPLAT_AUDIO_RATE_22050:
		rate = 22050;
		break;
	case TPLAT_AUDIO_RATE_44100:
		rate = 44100;
		break;
	case TPLAT_AUDIO_RATE_88200:
		rate = 88200;
		break;
	default:
		goto fail;
	}

	if (ioctl(fd, SNDCTL_DSP_SETFMT, &fmt) == -1) goto fail;
	if (ioctl(fd, SNDCTL_DSP_CHANNELS, &channels_n) == -1) goto fail;
	if (ioctl(fd, SNDCTL_DSP_SPEED, &rate) == -1) goto fail;

	if (is_block == TPLAT_TRUE) fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);
	else fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);

#endif

	return ((tplat_handle_t)fd);

fail:
	if (fd >= 0) close(fd);
	return TPLAT_INVALID_HANDLE;
}
tplat_int_t tplat_audio_write(tplat_handle_t haudio, tplat_byte_t const* data, tplat_size_t size)
{
	tplat_int_t fd = (tplat_int_t)haudio;
	if (fd >= 0) return write(fd, data, size);
	else return -1;
}
void tplat_audio_close(tplat_handle_t haudio)
{
	tplat_int_t fd = (tplat_int_t)haudio;
	if (fd >= 0) close(fd);
}
tplat_size_t tplat_audio_bsize(tplat_handle_t haudio)
{
	tplat_int_t fd = (tplat_int_t)haudio;
	if (fd >= 0)
	{
		tplat_int_t sample_rate = 0;
		tplat_int_t channels = 0;
		tplat_int_t quantify_bits = 0;
		if (ioctl(fd, SOUND_PCM_READ_RATE, &sample_rate) == -1) return 0;
		if (ioctl(fd, SOUND_PCM_READ_CHANNELS, &channels) == -1) return 0;
		if (ioctl(fd, SOUND_PCM_READ_BITS, &quantify_bits) == -1) return 0;
		TPLAT_DBG("bsize: %d x %d x %d / 8 = %d kb", sample_rate, channels, quantify_bits, ((sample_rate * channels * quantify_bits) >> 3) / 1024);
		return ((sample_rate * channels * quantify_bits) >> 3);
	}
	return 0;
}
void tplat_audio_clear(tplat_handle_t haudio)
{
}
void tplat_audio_pause(tplat_handle_t haudio)
{
}
void tplat_audio_resume(tplat_handle_t haudio)
{
}
