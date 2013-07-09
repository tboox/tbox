#ifndef TB_FLV_H
#define TB_FLV_H

/* ////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "tbox.h"

/* ////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the sdata string max
#define TB_FLV_SDATA_STRING_MAX 				(4096)

// offsets for packed values 
#define TB_FLV_AUDIO_SAMPLESIZE_OFFSET 			(1)
#define TB_FLV_AUDIO_SAMPLERATE_OFFSET 			(2)
#define TB_FLV_AUDIO_CODEC_OFFSET 				(4)
	
#define TB_FLV_VIDEO_FRAMETYPE_OFFSET 			(4)

// bitmasks to isolate specific values 
#define TB_FLV_AUDIO_CHANNEL_MASK 				(0x01)
#define TB_FLV_AUDIO_SAMPLESIZE_MASK 			(0x02)
#define TB_FLV_AUDIO_SAMPLERATE_MASK 			(0x0c)
#define TB_FLV_AUDIO_CODEC_MASK 				(0xf0)

#define TB_FLV_VIDEO_CODEC_MASK 				(0x0f)
#define TB_FLV_VIDEO_FRAMETYPE_MASK 			(0xf0)

/* ////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the flv tag type
typedef enum __tb_flv_tag_type_t
{
	TB_FLV_TAG_TYPE_AUDIO 						= 0x08
,	TB_FLV_TAG_TYPE_VIDEO 						= 0x09
,	TB_FLV_TAG_TYPE_SDATA 						= 0x12

}tb_flv_tag_type_t;

// the flv script data type
typedef enum __tb_flv_sdata_type_t
{
	TB_FLV_SDATA_TYPE_NUMBER 					= 0x00
,	TB_FLV_SDATA_TYPE_BOOLEAN 					= 0x01
,	TB_FLV_SDATA_TYPE_STRING 					= 0x02
,	TB_FLV_SDATA_TYPE_OBJECT 					= 0x03
,	TB_FLV_SDATA_TYPE_MOVIECLIP 				= 0x04
,	TB_FLV_SDATA_TYPE_NULL 						= 0x05
,	TB_FLV_SDATA_TYPE_UNDEFINED 				= 0x06
,	TB_FLV_SDATA_TYPE_REFERENCE 				= 0x07
,	TB_FLV_SDATA_TYPE_ECMAARRAY 				= 0x08
,	TB_FLV_SDATA_TYPE_STRICTARRAY 				= 0x0a
,	TB_FLV_SDATA_TYPE_DATE 						= 0x0b
,	TB_FLV_SDATA_TYPE_LONGSTRING 				= 0x0c

}tb_flv_sdata_type_t;

// the flv channel type
typedef enum __tb_flv_channel_t
{
 	TB_FLV_AUDIO_CHANNEL_MONO   				= 0
, 	TB_FLV_AUDIO_CHANNEL_STEREO 				= 1

}tb_flv_channel_t;

// the flv sample size type
typedef enum __tb_flv_sample_size_t
{
    TB_FLV_AUDIO_SAMPLESIZE_8BIT  				= 0
,	TB_FLV_AUDIO_SAMPLESIZE_16BIT 				= 1 << TB_FLV_AUDIO_SAMPLESIZE_OFFSET

}tb_flv_sample_size_t;

// the flv sample rate type
typedef enum __tb_flv_sample_rate_t
{
    TB_FLV_AUDIO_SAMPLERATE_SPECIAL 			= 0 //!< signifies 5512Hz and 8000Hz in the case of NELLYMOSER
, 	TB_FLV_AUDIO_SAMPLERATE_11025HZ 			= 1 << TB_FLV_AUDIO_SAMPLERATE_OFFSET
, 	TB_FLV_AUDIO_SAMPLERATE_22050HZ 			= 2 << TB_FLV_AUDIO_SAMPLERATE_OFFSET
, 	TB_FLV_AUDIO_SAMPLERATE_44100HZ 			= 3 << TB_FLV_AUDIO_SAMPLERATE_OFFSET

}tb_flv_sample_rate_t;

// the flv audio codec type
typedef enum __tb_flv_audio_codec_t
{
	TB_FLV_AUDIO_CODEC_PCM                  	= 0
,	TB_FLV_AUDIO_CODEC_ADPCM               		= 1 	<< TB_FLV_AUDIO_CODEC_OFFSET
,	TB_FLV_AUDIO_CODEC_MP3                 		= 2 	<< TB_FLV_AUDIO_CODEC_OFFSET
,	TB_FLV_AUDIO_CODEC_PCM_LE              		= 3 	<< TB_FLV_AUDIO_CODEC_OFFSET
,	TB_FLV_AUDIO_CODEC_NELLYMOSER_16KHZ_MONO 	= 4 	<< TB_FLV_AUDIO_CODEC_OFFSET
,	TB_FLV_AUDIO_CODEC_NELLYMOSER_8KHZ_MONO 	= 5 	<< TB_FLV_AUDIO_CODEC_OFFSET
,	TB_FLV_AUDIO_CODEC_NELLYMOSER           	= 6 	<< TB_FLV_AUDIO_CODEC_OFFSET
,	TB_FLV_AUDIO_CODEC_AAC                  	= 10 	<< TB_FLV_AUDIO_CODEC_OFFSET
,	TB_FLV_AUDIO_CODEC_SPEEX                	= 11 	<< TB_FLV_AUDIO_CODEC_OFFSET

}tb_flv_audio_codec_t;

// the flv video codec type
typedef enum __tb_flv_video_codec_t
{
	TB_FLV_VIDEO_CODEC_H263    					= 2
,	TB_FLV_VIDEO_CODEC_SCREEN  					= 3
,	TB_FLV_VIDEO_CODEC_VP6         				= 4
,	TB_FLV_VIDEO_CODEC_VP6A        				= 5
,	TB_FLV_VIDEO_CODEC_SCREEN2     				= 6
,	TB_FLV_VIDEO_CODEC_H264        				= 7

}tb_flv_video_codec_t;

// the flv frame type
typedef enum __tb_flv_frame_type_t
{
 	TB_FLV_FRAME_TYPE_KEY       				= 1 << TB_FLV_VIDEO_FRAMETYPE_OFFSET
,	TB_FLV_FRAME_TYPE_INTER      				= 2 << TB_FLV_VIDEO_FRAMETYPE_OFFSET
, 	TB_FLV_FRAME_TYPE_DISP_INTER 				= 3 << TB_FLV_VIDEO_FRAMETYPE_OFFSET

}tb_flv_frame_type_t;

// the flv script data string type
typedef struct __tb_flv_sdata_string_t
{
	tb_size_t 		size;
	tb_char_t 		data[TB_FLV_SDATA_STRING_MAX];

}tb_flv_sdata_string_t;

// the flv script data value type
struct __tb_flv_sdata_array_t;
struct __tb_flv_sdata_object_t;
typedef struct __tb_flv_sdata_value_t
{
	tb_size_t 							type;

	union 
	{
		tb_double_t 						number;
		tb_bool_t 						boolean;
		tb_flv_sdata_string_t 			string;
		struct __tb_flv_sdata_value_t* 	array;

	}u;

}tb_flv_sdata_value_t;

// the flv spank type
typedef enum __tb_flv_spank_type_t
{
	TB_FLV_SPANK_TYPE_HDATA 			= 1
,	TB_FLV_SPANK_TYPE_SDATA 			= 2
,	TB_FLV_SPANK_TYPE_AUDIO_CONFIG 		= 4
,	TB_FLV_SPANK_TYPE_VIDEO_CONFIG 		= 8
,	TB_FLV_SPANK_TYPE_AUDIO_DATA 		= 16
,	TB_FLV_SPANK_TYPE_VIDEO_DATA 		= 32

}tb_flv_spank_type_t;

// the ioctl command type
typedef enum __tb_flv_ioctl_cmd_t
{
	// callback type
	TB_FLV_IOCTL_CMD_CB_SDATA
,	TB_FLV_IOCTL_CMD_CB_SDATA_DATA
,	TB_FLV_IOCTL_CMD_CB_HDATA
,	TB_FLV_IOCTL_CMD_CB_AUDIO_CONFIG
,	TB_FLV_IOCTL_CMD_CB_VIDEO_CONFIG
,	TB_FLV_IOCTL_CMD_CB_AUDIO_DATA
,	TB_FLV_IOCTL_CMD_CB_VIDEO_DATA

	// stream
,	TB_FLV_IOCTL_CMD_SET_STREAM
,	TB_FLV_IOCTL_CMD_GET_STREAM

	// spank type
,	TB_FLV_IOCTL_CMD_SPANK_TYPE

}tb_flv_ioctl_cmd_t;

// the callback type
typedef tb_void_t (*tb_flv_hdata_cb_func_t)(tb_byte_t const* data, tb_size_t size, tb_pointer_t cb_data);
typedef tb_void_t (*tb_flv_sdata_cb_func_t)(tb_char_t const* spath, tb_flv_sdata_value_t const* value, tb_pointer_t cb_data);
typedef tb_void_t (*tb_flv_sdata_data_cb_func_t)(tb_byte_t const* head_data, tb_size_t head_size, tb_byte_t const* body_data, tb_size_t body_size, tb_pointer_t cb_data);
typedef tb_void_t (*tb_flv_audio_config_cb_func_t)(tb_byte_t const* head_data, tb_size_t head_size, tb_byte_t const* body_data, tb_size_t body_size, tb_pointer_t cb_data);
typedef tb_void_t (*tb_flv_video_config_cb_func_t)(tb_byte_t const* head_data, tb_size_t head_size, tb_byte_t const* body_data, tb_size_t body_size, tb_pointer_t cb_data);
typedef tb_bool_t (*tb_flv_audio_data_cb_func_t)(tb_byte_t const* head_data, tb_size_t head_size, tb_byte_t const* body_data, tb_size_t body_size, tb_uint32_t dts, tb_pointer_t cb_data);
typedef tb_bool_t (*tb_flv_video_data_cb_func_t)(tb_byte_t const* head_data, tb_size_t head_size, tb_byte_t const* body_data, tb_size_t body_size, tb_uint32_t dts, tb_sint32_t cts, tb_pointer_t cb_data);

/* ////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// init & exit
tb_handle_t tb_flv_init(tb_gstream_t* gst);
tb_void_t 	tb_flv_exit(tb_handle_t hflv);

// ioctl
tb_bool_t 	tb_flv_ioctl(tb_handle_t hflv, tb_size_t cmd, ...);

// spank
tb_bool_t 	tb_flv_spank(tb_handle_t hflv);

/* ////////////////////////////////////////////////////////////////////////////////
 * helper
 */

tb_size_t 	tb_flv_video_h264_sps_analyze_remove_emulation(tb_byte_t* sps_data, tb_size_t sps_size);
tb_double_t 	tb_flv_video_h264_sps_analyze_framerate(tb_byte_t* data, tb_size_t size);

#endif
