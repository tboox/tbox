/****************************************************************************
 ****************************************************************************
 ***
 ***   This header was automatically generated from a Linux kernel header
 ***   of the same name, to make information necessary for userspace to
 ***   call into the kernel available to libc.  It contains only constants,
 ***   structures, and macros generated from the original header, and thus,
 ***   contains no copyrightable information.
 ***
 ****************************************************************************
 ****************************************************************************/
#ifndef __LINUX_VIDEODEV2_H
#define __LINUX_VIDEODEV2_H
#define __user
#include <linux/types.h>

#define VIDEO_MAX_FRAME 32

#define VID_TYPE_CAPTURE 1  
#define VID_TYPE_TUNER 2  
#define VID_TYPE_TELETEXT 4  
#define VID_TYPE_OVERLAY 8  
#define VID_TYPE_CHROMAKEY 16  
#define VID_TYPE_CLIPPING 32  
#define VID_TYPE_FRAMERAM 64  
#define VID_TYPE_SCALES 128  
#define VID_TYPE_MONOCHROME 256  
#define VID_TYPE_SUBCAPTURE 512  
#define VID_TYPE_MPEG_DECODER 1024  
#define VID_TYPE_MPEG_ENCODER 2048  
#define VID_TYPE_MJPEG_DECODER 4096  
#define VID_TYPE_MJPEG_ENCODER 8192  

#define v4l2_fourcc(a,b,c,d)  (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

enum v4l2_field {
 V4L2_FIELD_ANY = 0,
 V4L2_FIELD_NONE = 1,
 V4L2_FIELD_TOP = 2,
 V4L2_FIELD_BOTTOM = 3,
 V4L2_FIELD_INTERLACED = 4,
 V4L2_FIELD_SEQ_TB = 5,
 V4L2_FIELD_SEQ_BT = 6,
 V4L2_FIELD_ALTERNATE = 7,
};
#define V4L2_FIELD_HAS_TOP(field)   ((field) == V4L2_FIELD_TOP ||  (field) == V4L2_FIELD_INTERLACED ||  (field) == V4L2_FIELD_SEQ_TB ||  (field) == V4L2_FIELD_SEQ_BT)
#define V4L2_FIELD_HAS_BOTTOM(field)   ((field) == V4L2_FIELD_BOTTOM ||  (field) == V4L2_FIELD_INTERLACED ||  (field) == V4L2_FIELD_SEQ_TB ||  (field) == V4L2_FIELD_SEQ_BT)
#define V4L2_FIELD_HAS_BOTH(field)   ((field) == V4L2_FIELD_INTERLACED ||  (field) == V4L2_FIELD_SEQ_TB ||  (field) == V4L2_FIELD_SEQ_BT)

enum v4l2_buf_type {
 V4L2_BUF_TYPE_VIDEO_CAPTURE = 1,
 V4L2_BUF_TYPE_VIDEO_OUTPUT = 2,
 V4L2_BUF_TYPE_VIDEO_OVERLAY = 3,
 V4L2_BUF_TYPE_VBI_CAPTURE = 4,
 V4L2_BUF_TYPE_VBI_OUTPUT = 5,

 V4L2_BUF_TYPE_SLICED_VBI_CAPTURE = 6,
 V4L2_BUF_TYPE_SLICED_VBI_OUTPUT = 7,
 V4L2_BUF_TYPE_PRIVATE = 0x80,
};

enum v4l2_ctrl_type {
 V4L2_CTRL_TYPE_INTEGER = 1,
 V4L2_CTRL_TYPE_BOOLEAN = 2,
 V4L2_CTRL_TYPE_MENU = 3,
 V4L2_CTRL_TYPE_BUTTON = 4,
 V4L2_CTRL_TYPE_INTEGER64 = 5,
 V4L2_CTRL_TYPE_CTRL_CLASS = 6,
};

enum v4l2_tuner_type {
 V4L2_TUNER_RADIO = 1,
 V4L2_TUNER_ANALOG_TV = 2,
 V4L2_TUNER_DIGITAL_TV = 3,
};

enum v4l2_memory {
 V4L2_MEMORY_MMAP = 1,
 V4L2_MEMORY_USERPTR = 2,
 V4L2_MEMORY_OVERLAY = 3,
};

enum v4l2_colorspace {

 V4L2_COLORSPACE_SMPTE170M = 1,

 V4L2_COLORSPACE_SMPTE240M = 2,

 V4L2_COLORSPACE_REC709 = 3,

 V4L2_COLORSPACE_BT878 = 4,

 V4L2_COLORSPACE_470_SYSTEM_M = 5,
 V4L2_COLORSPACE_470_SYSTEM_BG = 6,

 V4L2_COLORSPACE_JPEG = 7,

 V4L2_COLORSPACE_SRGB = 8,
};

enum v4l2_priority {
 V4L2_PRIORITY_UNSET = 0,
 V4L2_PRIORITY_BACKGROUND = 1,
 V4L2_PRIORITY_INTERACTIVE = 2,
 V4L2_PRIORITY_RECORD = 3,
 V4L2_PRIORITY_DEFAULT = V4L2_PRIORITY_INTERACTIVE,
};

struct v4l2_rect {
 __s32 left;
 __s32 top;
 __s32 width;
 __s32 height;
};

struct v4l2_fract {
 __u32 numerator;
 __u32 denominator;
};

struct v4l2_capability
{
 __u8 driver[16];
 __u8 card[32];
 __u8 bus_info[32];
 __u32 version;
 __u32 capabilities;
 __u32 reserved[4];
};

#define V4L2_CAP_VIDEO_CAPTURE 0x00000001  
#define V4L2_CAP_VIDEO_OUTPUT 0x00000002  
#define V4L2_CAP_VIDEO_OVERLAY 0x00000004  
#define V4L2_CAP_VBI_CAPTURE 0x00000010  
#define V4L2_CAP_VBI_OUTPUT 0x00000020  
#define V4L2_CAP_SLICED_VBI_CAPTURE 0x00000040  
#define V4L2_CAP_SLICED_VBI_OUTPUT 0x00000080  
#define V4L2_CAP_RDS_CAPTURE 0x00000100  

#define V4L2_CAP_TUNER 0x00010000  
#define V4L2_CAP_AUDIO 0x00020000  
#define V4L2_CAP_RADIO 0x00040000  

#define V4L2_CAP_READWRITE 0x01000000  
#define V4L2_CAP_ASYNCIO 0x02000000  
#define V4L2_CAP_STREAMING 0x04000000  

struct v4l2_pix_format
{
 __u32 width;
 __u32 height;
 __u32 pixelformat;
 enum v4l2_field field;
 __u32 bytesperline;
 __u32 sizeimage;
 enum v4l2_colorspace colorspace;
 __u32 priv;
};

#define V4L2_PIX_FMT_RGB332 v4l2_fourcc('R','G','B','1')  
#define V4L2_PIX_FMT_RGB555 v4l2_fourcc('R','G','B','O')  
#define V4L2_PIX_FMT_RGB565 v4l2_fourcc('R','G','B','P')  
#define V4L2_PIX_FMT_RGB555X v4l2_fourcc('R','G','B','Q')  
#define V4L2_PIX_FMT_RGB565X v4l2_fourcc('R','G','B','R')  
#define V4L2_PIX_FMT_BGR24 v4l2_fourcc('B','G','R','3')  
#define V4L2_PIX_FMT_RGB24 v4l2_fourcc('R','G','B','3')  
#define V4L2_PIX_FMT_BGR32 v4l2_fourcc('B','G','R','4')  
#define V4L2_PIX_FMT_RGB32 v4l2_fourcc('R','G','B','4')  
#define V4L2_PIX_FMT_GREY v4l2_fourcc('G','R','E','Y')  
#define V4L2_PIX_FMT_YVU410 v4l2_fourcc('Y','V','U','9')  
#define V4L2_PIX_FMT_YVU420 v4l2_fourcc('Y','V','1','2')  
#define V4L2_PIX_FMT_YUYV v4l2_fourcc('Y','U','Y','V')  
#define V4L2_PIX_FMT_UYVY v4l2_fourcc('U','Y','V','Y')  
#define V4L2_PIX_FMT_YUV422P v4l2_fourcc('4','2','2','P')  
#define V4L2_PIX_FMT_YUV411P v4l2_fourcc('4','1','1','P')  
#define V4L2_PIX_FMT_Y41P v4l2_fourcc('Y','4','1','P')  

#define V4L2_PIX_FMT_NV12 v4l2_fourcc('N','V','1','2')  
#define V4L2_PIX_FMT_NV21 v4l2_fourcc('N','V','2','1')  

#define V4L2_PIX_FMT_YUV410 v4l2_fourcc('Y','U','V','9')  
#define V4L2_PIX_FMT_YUV420 v4l2_fourcc('Y','U','1','2')  
#define V4L2_PIX_FMT_YYUV v4l2_fourcc('Y','Y','U','V')  
#define V4L2_PIX_FMT_HI240 v4l2_fourcc('H','I','2','4')  
#define V4L2_PIX_FMT_HM12 v4l2_fourcc('H','M','1','2')  

#define V4L2_PIX_FMT_SBGGR8 v4l2_fourcc('B','A','8','1')  

#define V4L2_PIX_FMT_MJPEG v4l2_fourcc('M','J','P','G')  
#define V4L2_PIX_FMT_JPEG v4l2_fourcc('J','P','E','G')  
#define V4L2_PIX_FMT_DV v4l2_fourcc('d','v','s','d')  
#define V4L2_PIX_FMT_MPEG v4l2_fourcc('M','P','E','G')  

#define V4L2_PIX_FMT_WNVA v4l2_fourcc('W','N','V','A')  
#define V4L2_PIX_FMT_SN9C10X v4l2_fourcc('S','9','1','0')  
#define V4L2_PIX_FMT_PWC1 v4l2_fourcc('P','W','C','1')  
#define V4L2_PIX_FMT_PWC2 v4l2_fourcc('P','W','C','2')  
#define V4L2_PIX_FMT_ET61X251 v4l2_fourcc('E','6','2','5')  

struct v4l2_fmtdesc
{
 __u32 index;
 enum v4l2_buf_type type;
 __u32 flags;
 __u8 description[32];
 __u32 pixelformat;
 __u32 reserved[4];
};

#define V4L2_FMT_FLAG_COMPRESSED 0x0001

struct v4l2_timecode
{
 __u32 type;
 __u32 flags;
 __u8 frames;
 __u8 seconds;
 __u8 minutes;
 __u8 hours;
 __u8 userbits[4];
};

#define V4L2_TC_TYPE_24FPS 1
#define V4L2_TC_TYPE_25FPS 2
#define V4L2_TC_TYPE_30FPS 3
#define V4L2_TC_TYPE_50FPS 4
#define V4L2_TC_TYPE_60FPS 5

#define V4L2_TC_FLAG_DROPFRAME 0x0001  
#define V4L2_TC_FLAG_COLORFRAME 0x0002
#define V4L2_TC_USERBITS_field 0x000C
#define V4L2_TC_USERBITS_USERDEFINED 0x0000
#define V4L2_TC_USERBITS_8BITCHARS 0x0008

struct v4l2_jpegcompression
{
 int quality;

 int APPn;
 int APP_len;
 char APP_data[60];

 int COM_len;
 char COM_data[60];

 __u32 jpeg_markers;

#define V4L2_JPEG_MARKER_DHT (1<<3)  
#define V4L2_JPEG_MARKER_DQT (1<<4)  
#define V4L2_JPEG_MARKER_DRI (1<<5)  
#define V4L2_JPEG_MARKER_COM (1<<6)  
#define V4L2_JPEG_MARKER_APP (1<<7)  
};

struct v4l2_requestbuffers
{
 __u32 count;
 enum v4l2_buf_type type;
 enum v4l2_memory memory;
 __u32 reserved[2];
};

struct v4l2_buffer
{
 __u32 index;
 enum v4l2_buf_type type;
 __u32 bytesused;
 __u32 flags;
 enum v4l2_field field;
 struct timeval timestamp;
 struct v4l2_timecode timecode;
 __u32 sequence;

 enum v4l2_memory memory;
 union {
 __u32 offset;
 unsigned long userptr;
 } m;
 __u32 length;
 __u32 input;
 __u32 reserved;
};

#define V4L2_BUF_FLAG_MAPPED 0x0001  
#define V4L2_BUF_FLAG_QUEUED 0x0002  
#define V4L2_BUF_FLAG_DONE 0x0004  
#define V4L2_BUF_FLAG_KEYFRAME 0x0008  
#define V4L2_BUF_FLAG_PFRAME 0x0010  
#define V4L2_BUF_FLAG_BFRAME 0x0020  
#define V4L2_BUF_FLAG_TIMECODE 0x0100  
#define V4L2_BUF_FLAG_INPUT 0x0200  

struct v4l2_framebuffer
{
 __u32 capability;
 __u32 flags;

 void* base;
 struct v4l2_pix_format fmt;
};

#define V4L2_FBUF_CAP_EXTERNOVERLAY 0x0001
#define V4L2_FBUF_CAP_CHROMAKEY 0x0002
#define V4L2_FBUF_CAP_LIST_CLIPPING 0x0004
#define V4L2_FBUF_CAP_BITMAP_CLIPPING 0x0008

#define V4L2_FBUF_FLAG_PRIMARY 0x0001
#define V4L2_FBUF_FLAG_OVERLAY 0x0002
#define V4L2_FBUF_FLAG_CHROMAKEY 0x0004

struct v4l2_clip
{
 struct v4l2_rect c;
 struct v4l2_clip __user *next;
};

struct v4l2_window
{
 struct v4l2_rect w;
 enum v4l2_field field;
 __u32 chromakey;
 struct v4l2_clip __user *clips;
 __u32 clipcount;
 void __user *bitmap;
};

struct v4l2_captureparm
{
 __u32 capability;
 __u32 capturemode;
 struct v4l2_fract timeperframe;
 __u32 extendedmode;
 __u32 readbuffers;
 __u32 reserved[4];
};

#define V4L2_MODE_HIGHQUALITY 0x0001  
#define V4L2_CAP_TIMEPERFRAME 0x1000  

struct v4l2_outputparm
{
 __u32 capability;
 __u32 outputmode;
 struct v4l2_fract timeperframe;
 __u32 extendedmode;
 __u32 writebuffers;
 __u32 reserved[4];
};

struct v4l2_cropcap {
 enum v4l2_buf_type type;
 struct v4l2_rect bounds;
 struct v4l2_rect defrect;
 struct v4l2_fract pixelaspect;
};

struct v4l2_crop {
 enum v4l2_buf_type type;
 struct v4l2_rect c;
};

typedef __u64 v4l2_std_id;

#define V4L2_STD_PAL_B ((v4l2_std_id)0x00000001)
#define V4L2_STD_PAL_B1 ((v4l2_std_id)0x00000002)
#define V4L2_STD_PAL_G ((v4l2_std_id)0x00000004)
#define V4L2_STD_PAL_H ((v4l2_std_id)0x00000008)
#define V4L2_STD_PAL_I ((v4l2_std_id)0x00000010)
#define V4L2_STD_PAL_D ((v4l2_std_id)0x00000020)
#define V4L2_STD_PAL_D1 ((v4l2_std_id)0x00000040)
#define V4L2_STD_PAL_K ((v4l2_std_id)0x00000080)

#define V4L2_STD_PAL_M ((v4l2_std_id)0x00000100)
#define V4L2_STD_PAL_N ((v4l2_std_id)0x00000200)
#define V4L2_STD_PAL_Nc ((v4l2_std_id)0x00000400)
#define V4L2_STD_PAL_60 ((v4l2_std_id)0x00000800)

#define V4L2_STD_NTSC_M ((v4l2_std_id)0x00001000)
#define V4L2_STD_NTSC_M_JP ((v4l2_std_id)0x00002000)
#define V4L2_STD_NTSC_443 ((v4l2_std_id)0x00004000)
#define V4L2_STD_NTSC_M_KR ((v4l2_std_id)0x00008000)

#define V4L2_STD_SECAM_B ((v4l2_std_id)0x00010000)
#define V4L2_STD_SECAM_D ((v4l2_std_id)0x00020000)
#define V4L2_STD_SECAM_G ((v4l2_std_id)0x00040000)
#define V4L2_STD_SECAM_H ((v4l2_std_id)0x00080000)
#define V4L2_STD_SECAM_K ((v4l2_std_id)0x00100000)
#define V4L2_STD_SECAM_K1 ((v4l2_std_id)0x00200000)
#define V4L2_STD_SECAM_L ((v4l2_std_id)0x00400000)
#define V4L2_STD_SECAM_LC ((v4l2_std_id)0x00800000)

#define V4L2_STD_ATSC_8_VSB ((v4l2_std_id)0x01000000)
#define V4L2_STD_ATSC_16_VSB ((v4l2_std_id)0x02000000)

#define V4L2_STD_MN (V4L2_STD_PAL_M|V4L2_STD_PAL_N|V4L2_STD_PAL_Nc|V4L2_STD_NTSC)
#define V4L2_STD_B (V4L2_STD_PAL_B|V4L2_STD_PAL_B1|V4L2_STD_SECAM_B)
#define V4L2_STD_GH (V4L2_STD_PAL_G|V4L2_STD_PAL_H|V4L2_STD_SECAM_G|V4L2_STD_SECAM_H)
#define V4L2_STD_DK (V4L2_STD_PAL_DK|V4L2_STD_SECAM_DK)

#define V4L2_STD_PAL_BG (V4L2_STD_PAL_B |  V4L2_STD_PAL_B1 |  V4L2_STD_PAL_G)
#define V4L2_STD_PAL_DK (V4L2_STD_PAL_D |  V4L2_STD_PAL_D1 |  V4L2_STD_PAL_K)
#define V4L2_STD_PAL (V4L2_STD_PAL_BG |  V4L2_STD_PAL_DK |  V4L2_STD_PAL_H |  V4L2_STD_PAL_I)
#define V4L2_STD_NTSC (V4L2_STD_NTSC_M |  V4L2_STD_NTSC_M_JP |  V4L2_STD_NTSC_M_KR)
#define V4L2_STD_SECAM_DK (V4L2_STD_SECAM_D |  V4L2_STD_SECAM_K |  V4L2_STD_SECAM_K1)
#define V4L2_STD_SECAM (V4L2_STD_SECAM_B |  V4L2_STD_SECAM_G |  V4L2_STD_SECAM_H |  V4L2_STD_SECAM_DK |  V4L2_STD_SECAM_L |  V4L2_STD_SECAM_LC)

#define V4L2_STD_525_60 (V4L2_STD_PAL_M |  V4L2_STD_PAL_60 |  V4L2_STD_NTSC |  V4L2_STD_NTSC_443)
#define V4L2_STD_625_50 (V4L2_STD_PAL |  V4L2_STD_PAL_N |  V4L2_STD_PAL_Nc |  V4L2_STD_SECAM)
#define V4L2_STD_ATSC (V4L2_STD_ATSC_8_VSB |  V4L2_STD_ATSC_16_VSB)

#define V4L2_STD_UNKNOWN 0
#define V4L2_STD_ALL (V4L2_STD_525_60 |  V4L2_STD_625_50)

struct v4l2_standard
{
 __u32 index;
 v4l2_std_id id;
 __u8 name[24];
 struct v4l2_fract frameperiod;
 __u32 framelines;
 __u32 reserved[4];
};

struct v4l2_input
{
 __u32 index;
 __u8 name[32];
 __u32 type;
 __u32 audioset;
 __u32 tuner;
 v4l2_std_id std;
 __u32 status;
 __u32 reserved[4];
};

#define V4L2_INPUT_TYPE_TUNER 1
#define V4L2_INPUT_TYPE_CAMERA 2

#define V4L2_IN_ST_NO_POWER 0x00000001  
#define V4L2_IN_ST_NO_SIGNAL 0x00000002
#define V4L2_IN_ST_NO_COLOR 0x00000004

#define V4L2_IN_ST_NO_H_LOCK 0x00000100  
#define V4L2_IN_ST_COLOR_KILL 0x00000200  

#define V4L2_IN_ST_NO_SYNC 0x00010000  
#define V4L2_IN_ST_NO_EQU 0x00020000  
#define V4L2_IN_ST_NO_CARRIER 0x00040000  

#define V4L2_IN_ST_MACROVISION 0x01000000  
#define V4L2_IN_ST_NO_ACCESS 0x02000000  
#define V4L2_IN_ST_VTR 0x04000000  

struct v4l2_output
{
 __u32 index;
 __u8 name[32];
 __u32 type;
 __u32 audioset;
 __u32 modulator;
 v4l2_std_id std;
 __u32 reserved[4];
};

#define V4L2_OUTPUT_TYPE_MODULATOR 1
#define V4L2_OUTPUT_TYPE_ANALOG 2
#define V4L2_OUTPUT_TYPE_ANALOGVGAOVERLAY 3

struct v4l2_control
{
 __u32 id;
 __s32 value;
};

struct v4l2_ext_control
{
 __u32 id;
 __u32 reserved2[2];
 union {
 __s32 value;
 __s64 value64;
 void *reserved;
 };
} __attribute__ ((packed));

struct v4l2_ext_controls
{
 __u32 ctrl_class;
 __u32 count;
 __u32 error_idx;
 __u32 reserved[2];
 struct v4l2_ext_control *controls;
};

#define V4L2_CTRL_CLASS_USER 0x00980000  
#define V4L2_CTRL_CLASS_MPEG 0x00990000  

#define V4L2_CTRL_ID_MASK (0x0fffffff)
#define V4L2_CTRL_ID2CLASS(id) ((id) & 0x0fff0000UL)
#define V4L2_CTRL_DRIVER_PRIV(id) (((id) & 0xffff) >= 0x1000)

struct v4l2_queryctrl
{
 __u32 id;
 enum v4l2_ctrl_type type;
 __u8 name[32];
 __s32 minimum;
 __s32 maximum;
 __s32 step;
 __s32 default_value;
 __u32 flags;
 __u32 reserved[2];
};

struct v4l2_querymenu
{
 __u32 id;
 __u32 index;
 __u8 name[32];
 __u32 reserved;
};

#define V4L2_CTRL_FLAG_DISABLED 0x0001
#define V4L2_CTRL_FLAG_GRABBED 0x0002
#define V4L2_CTRL_FLAG_READ_ONLY 0x0004
#define V4L2_CTRL_FLAG_UPDATE 0x0008
#define V4L2_CTRL_FLAG_INACTIVE 0x0010
#define V4L2_CTRL_FLAG_SLIDER 0x0020

#define V4L2_CTRL_FLAG_NEXT_CTRL 0x80000000

#define V4L2_CID_BASE (V4L2_CTRL_CLASS_USER | 0x900)
#define V4L2_CID_USER_BASE V4L2_CID_BASE

#define V4L2_CID_PRIVATE_BASE 0x08000000

#define V4L2_CID_USER_CLASS (V4L2_CTRL_CLASS_USER | 1)
#define V4L2_CID_BRIGHTNESS (V4L2_CID_BASE+0)
#define V4L2_CID_CONTRAST (V4L2_CID_BASE+1)
#define V4L2_CID_SATURATION (V4L2_CID_BASE+2)
#define V4L2_CID_HUE (V4L2_CID_BASE+3)
#define V4L2_CID_AUDIO_VOLUME (V4L2_CID_BASE+5)
#define V4L2_CID_AUDIO_BALANCE (V4L2_CID_BASE+6)
#define V4L2_CID_AUDIO_BASS (V4L2_CID_BASE+7)
#define V4L2_CID_AUDIO_TREBLE (V4L2_CID_BASE+8)
#define V4L2_CID_AUDIO_MUTE (V4L2_CID_BASE+9)
#define V4L2_CID_AUDIO_LOUDNESS (V4L2_CID_BASE+10)
#define V4L2_CID_BLACK_LEVEL (V4L2_CID_BASE+11)
#define V4L2_CID_AUTO_WHITE_BALANCE (V4L2_CID_BASE+12)
#define V4L2_CID_DO_WHITE_BALANCE (V4L2_CID_BASE+13)
#define V4L2_CID_RED_BALANCE (V4L2_CID_BASE+14)
#define V4L2_CID_BLUE_BALANCE (V4L2_CID_BASE+15)
#define V4L2_CID_GAMMA (V4L2_CID_BASE+16)
#define V4L2_CID_WHITENESS (V4L2_CID_GAMMA)  
#define V4L2_CID_EXPOSURE (V4L2_CID_BASE+17)
#define V4L2_CID_AUTOGAIN (V4L2_CID_BASE+18)
#define V4L2_CID_GAIN (V4L2_CID_BASE+19)
#define V4L2_CID_HFLIP (V4L2_CID_BASE+20)
#define V4L2_CID_VFLIP (V4L2_CID_BASE+21)
#define V4L2_CID_HCENTER (V4L2_CID_BASE+22)
#define V4L2_CID_VCENTER (V4L2_CID_BASE+23)
#define V4L2_CID_LASTP1 (V4L2_CID_BASE+24)  

#define V4L2_CID_MPEG_BASE (V4L2_CTRL_CLASS_MPEG | 0x900)
#define V4L2_CID_MPEG_CLASS (V4L2_CTRL_CLASS_MPEG | 1)

#define V4L2_CID_MPEG_STREAM_TYPE (V4L2_CID_MPEG_BASE+0)
enum v4l2_mpeg_stream_type {
 V4L2_MPEG_STREAM_TYPE_MPEG2_PS = 0,
 V4L2_MPEG_STREAM_TYPE_MPEG2_TS = 1,
 V4L2_MPEG_STREAM_TYPE_MPEG1_SS = 2,
 V4L2_MPEG_STREAM_TYPE_MPEG2_DVD = 3,
 V4L2_MPEG_STREAM_TYPE_MPEG1_VCD = 4,
 V4L2_MPEG_STREAM_TYPE_MPEG2_SVCD = 5,
};
#define V4L2_CID_MPEG_STREAM_PID_PMT (V4L2_CID_MPEG_BASE+1)
#define V4L2_CID_MPEG_STREAM_PID_AUDIO (V4L2_CID_MPEG_BASE+2)
#define V4L2_CID_MPEG_STREAM_PID_VIDEO (V4L2_CID_MPEG_BASE+3)
#define V4L2_CID_MPEG_STREAM_PID_PCR (V4L2_CID_MPEG_BASE+4)
#define V4L2_CID_MPEG_STREAM_PES_ID_AUDIO (V4L2_CID_MPEG_BASE+5)
#define V4L2_CID_MPEG_STREAM_PES_ID_VIDEO (V4L2_CID_MPEG_BASE+6)
#define V4L2_CID_MPEG_STREAM_VBI_FMT (V4L2_CID_MPEG_BASE+7)
enum v4l2_mpeg_stream_vbi_fmt {
 V4L2_MPEG_STREAM_VBI_FMT_NONE = 0,
 V4L2_MPEG_STREAM_VBI_FMT_IVTV = 1,
};

#define V4L2_CID_MPEG_AUDIO_SAMPLING_FREQ (V4L2_CID_MPEG_BASE+100)
enum v4l2_mpeg_audio_sampling_freq {
 V4L2_MPEG_AUDIO_SAMPLING_FREQ_44100 = 0,
 V4L2_MPEG_AUDIO_SAMPLING_FREQ_48000 = 1,
 V4L2_MPEG_AUDIO_SAMPLING_FREQ_32000 = 2,
};
#define V4L2_CID_MPEG_AUDIO_ENCODING (V4L2_CID_MPEG_BASE+101)
enum v4l2_mpeg_audio_encoding {
 V4L2_MPEG_AUDIO_ENCODING_LAYER_1 = 0,
 V4L2_MPEG_AUDIO_ENCODING_LAYER_2 = 1,
 V4L2_MPEG_AUDIO_ENCODING_LAYER_3 = 2,
};
#define V4L2_CID_MPEG_AUDIO_L1_BITRATE (V4L2_CID_MPEG_BASE+102)
enum v4l2_mpeg_audio_l1_bitrate {
 V4L2_MPEG_AUDIO_L1_BITRATE_32K = 0,
 V4L2_MPEG_AUDIO_L1_BITRATE_64K = 1,
 V4L2_MPEG_AUDIO_L1_BITRATE_96K = 2,
 V4L2_MPEG_AUDIO_L1_BITRATE_128K = 3,
 V4L2_MPEG_AUDIO_L1_BITRATE_160K = 4,
 V4L2_MPEG_AUDIO_L1_BITRATE_192K = 5,
 V4L2_MPEG_AUDIO_L1_BITRATE_224K = 6,
 V4L2_MPEG_AUDIO_L1_BITRATE_256K = 7,
 V4L2_MPEG_AUDIO_L1_BITRATE_288K = 8,
 V4L2_MPEG_AUDIO_L1_BITRATE_320K = 9,
 V4L2_MPEG_AUDIO_L1_BITRATE_352K = 10,
 V4L2_MPEG_AUDIO_L1_BITRATE_384K = 11,
 V4L2_MPEG_AUDIO_L1_BITRATE_416K = 12,
 V4L2_MPEG_AUDIO_L1_BITRATE_448K = 13,
};
#define V4L2_CID_MPEG_AUDIO_L2_BITRATE (V4L2_CID_MPEG_BASE+103)
enum v4l2_mpeg_audio_l2_bitrate {
 V4L2_MPEG_AUDIO_L2_BITRATE_32K = 0,
 V4L2_MPEG_AUDIO_L2_BITRATE_48K = 1,
 V4L2_MPEG_AUDIO_L2_BITRATE_56K = 2,
 V4L2_MPEG_AUDIO_L2_BITRATE_64K = 3,
 V4L2_MPEG_AUDIO_L2_BITRATE_80K = 4,
 V4L2_MPEG_AUDIO_L2_BITRATE_96K = 5,
 V4L2_MPEG_AUDIO_L2_BITRATE_112K = 6,
 V4L2_MPEG_AUDIO_L2_BITRATE_128K = 7,
 V4L2_MPEG_AUDIO_L2_BITRATE_160K = 8,
 V4L2_MPEG_AUDIO_L2_BITRATE_192K = 9,
 V4L2_MPEG_AUDIO_L2_BITRATE_224K = 10,
 V4L2_MPEG_AUDIO_L2_BITRATE_256K = 11,
 V4L2_MPEG_AUDIO_L2_BITRATE_320K = 12,
 V4L2_MPEG_AUDIO_L2_BITRATE_384K = 13,
};
#define V4L2_CID_MPEG_AUDIO_L3_BITRATE (V4L2_CID_MPEG_BASE+104)
enum v4l2_mpeg_audio_l3_bitrate {
 V4L2_MPEG_AUDIO_L3_BITRATE_32K = 0,
 V4L2_MPEG_AUDIO_L3_BITRATE_40K = 1,
 V4L2_MPEG_AUDIO_L3_BITRATE_48K = 2,
 V4L2_MPEG_AUDIO_L3_BITRATE_56K = 3,
 V4L2_MPEG_AUDIO_L3_BITRATE_64K = 4,
 V4L2_MPEG_AUDIO_L3_BITRATE_80K = 5,
 V4L2_MPEG_AUDIO_L3_BITRATE_96K = 6,
 V4L2_MPEG_AUDIO_L3_BITRATE_112K = 7,
 V4L2_MPEG_AUDIO_L3_BITRATE_128K = 8,
 V4L2_MPEG_AUDIO_L3_BITRATE_160K = 9,
 V4L2_MPEG_AUDIO_L3_BITRATE_192K = 10,
 V4L2_MPEG_AUDIO_L3_BITRATE_224K = 11,
 V4L2_MPEG_AUDIO_L3_BITRATE_256K = 12,
 V4L2_MPEG_AUDIO_L3_BITRATE_320K = 13,
};
#define V4L2_CID_MPEG_AUDIO_MODE (V4L2_CID_MPEG_BASE+105)
enum v4l2_mpeg_audio_mode {
 V4L2_MPEG_AUDIO_MODE_STEREO = 0,
 V4L2_MPEG_AUDIO_MODE_JOINT_STEREO = 1,
 V4L2_MPEG_AUDIO_MODE_DUAL = 2,
 V4L2_MPEG_AUDIO_MODE_MONO = 3,
};
#define V4L2_CID_MPEG_AUDIO_MODE_EXTENSION (V4L2_CID_MPEG_BASE+106)
enum v4l2_mpeg_audio_mode_extension {
 V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_4 = 0,
 V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_8 = 1,
 V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_12 = 2,
 V4L2_MPEG_AUDIO_MODE_EXTENSION_BOUND_16 = 3,
};
#define V4L2_CID_MPEG_AUDIO_EMPHASIS (V4L2_CID_MPEG_BASE+107)
enum v4l2_mpeg_audio_emphasis {
 V4L2_MPEG_AUDIO_EMPHASIS_NONE = 0,
 V4L2_MPEG_AUDIO_EMPHASIS_50_DIV_15_uS = 1,
 V4L2_MPEG_AUDIO_EMPHASIS_CCITT_J17 = 2,
};
#define V4L2_CID_MPEG_AUDIO_CRC (V4L2_CID_MPEG_BASE+108)
enum v4l2_mpeg_audio_crc {
 V4L2_MPEG_AUDIO_CRC_NONE = 0,
 V4L2_MPEG_AUDIO_CRC_CRC16 = 1,
};

#define V4L2_CID_MPEG_VIDEO_ENCODING (V4L2_CID_MPEG_BASE+200)
enum v4l2_mpeg_video_encoding {
 V4L2_MPEG_VIDEO_ENCODING_MPEG_1 = 0,
 V4L2_MPEG_VIDEO_ENCODING_MPEG_2 = 1,
};
#define V4L2_CID_MPEG_VIDEO_ASPECT (V4L2_CID_MPEG_BASE+201)
enum v4l2_mpeg_video_aspect {
 V4L2_MPEG_VIDEO_ASPECT_1x1 = 0,
 V4L2_MPEG_VIDEO_ASPECT_4x3 = 1,
 V4L2_MPEG_VIDEO_ASPECT_16x9 = 2,
 V4L2_MPEG_VIDEO_ASPECT_221x100 = 3,
};
#define V4L2_CID_MPEG_VIDEO_B_FRAMES (V4L2_CID_MPEG_BASE+202)
#define V4L2_CID_MPEG_VIDEO_GOP_SIZE (V4L2_CID_MPEG_BASE+203)
#define V4L2_CID_MPEG_VIDEO_GOP_CLOSURE (V4L2_CID_MPEG_BASE+204)
#define V4L2_CID_MPEG_VIDEO_PULLDOWN (V4L2_CID_MPEG_BASE+205)
#define V4L2_CID_MPEG_VIDEO_BITRATE_MODE (V4L2_CID_MPEG_BASE+206)
enum v4l2_mpeg_video_bitrate_mode {
 V4L2_MPEG_VIDEO_BITRATE_MODE_VBR = 0,
 V4L2_MPEG_VIDEO_BITRATE_MODE_CBR = 1,
};
#define V4L2_CID_MPEG_VIDEO_BITRATE (V4L2_CID_MPEG_BASE+207)
#define V4L2_CID_MPEG_VIDEO_BITRATE_PEAK (V4L2_CID_MPEG_BASE+208)
#define V4L2_CID_MPEG_VIDEO_TEMPORAL_DECIMATION (V4L2_CID_MPEG_BASE+209)

#define V4L2_CID_MPEG_CX2341X_BASE (V4L2_CTRL_CLASS_MPEG | 0x1000)
#define V4L2_CID_MPEG_CX2341X_VIDEO_SPATIAL_FILTER_MODE (V4L2_CID_MPEG_CX2341X_BASE+0)
enum v4l2_mpeg_cx2341x_video_spatial_filter_mode {
 V4L2_MPEG_CX2341X_VIDEO_SPATIAL_FILTER_MODE_MANUAL = 0,
 V4L2_MPEG_CX2341X_VIDEO_SPATIAL_FILTER_MODE_AUTO = 1,
};
#define V4L2_CID_MPEG_CX2341X_VIDEO_SPATIAL_FILTER (V4L2_CID_MPEG_CX2341X_BASE+1)
#define V4L2_CID_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE (V4L2_CID_MPEG_CX2341X_BASE+2)
enum v4l2_mpeg_cx2341x_video_luma_spatial_filter_type {
 V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_OFF = 0,
 V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_1D_HOR = 1,
 V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_1D_VERT = 2,
 V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_2D_HV_SEPARABLE = 3,
 V4L2_MPEG_CX2341X_VIDEO_LUMA_SPATIAL_FILTER_TYPE_2D_SYM_NON_SEPARABLE = 4,
};
#define V4L2_CID_MPEG_CX2341X_VIDEO_CHROMA_SPATIAL_FILTER_TYPE (V4L2_CID_MPEG_CX2341X_BASE+3)
enum v4l2_mpeg_cx2341x_video_chroma_spatial_filter_type {
 V4L2_MPEG_CX2341X_VIDEO_CHROMA_SPATIAL_FILTER_TYPE_OFF = 0,
 V4L2_MPEG_CX2341X_VIDEO_CHROMA_SPATIAL_FILTER_TYPE_1D_HOR = 1,
};
#define V4L2_CID_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER_MODE (V4L2_CID_MPEG_CX2341X_BASE+4)
enum v4l2_mpeg_cx2341x_video_temporal_filter_mode {
 V4L2_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER_MODE_MANUAL = 0,
 V4L2_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER_MODE_AUTO = 1,
};
#define V4L2_CID_MPEG_CX2341X_VIDEO_TEMPORAL_FILTER (V4L2_CID_MPEG_CX2341X_BASE+5)
#define V4L2_CID_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE (V4L2_CID_MPEG_CX2341X_BASE+6)
enum v4l2_mpeg_cx2341x_video_median_filter_type {
 V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_OFF = 0,
 V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_HOR = 1,
 V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_VERT = 2,
 V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_HOR_VERT = 3,
 V4L2_MPEG_CX2341X_VIDEO_MEDIAN_FILTER_TYPE_DIAG = 4,
};
#define V4L2_CID_MPEG_CX2341X_VIDEO_LUMA_MEDIAN_FILTER_BOTTOM (V4L2_CID_MPEG_CX2341X_BASE+7)
#define V4L2_CID_MPEG_CX2341X_VIDEO_LUMA_MEDIAN_FILTER_TOP (V4L2_CID_MPEG_CX2341X_BASE+8)
#define V4L2_CID_MPEG_CX2341X_VIDEO_CHROMA_MEDIAN_FILTER_BOTTOM (V4L2_CID_MPEG_CX2341X_BASE+9)
#define V4L2_CID_MPEG_CX2341X_VIDEO_CHROMA_MEDIAN_FILTER_TOP (V4L2_CID_MPEG_CX2341X_BASE+10)

struct v4l2_tuner
{
 __u32 index;
 __u8 name[32];
 enum v4l2_tuner_type type;
 __u32 capability;
 __u32 rangelow;
 __u32 rangehigh;
 __u32 rxsubchans;
 __u32 audmode;
 __s32 signal;
 __s32 afc;
 __u32 reserved[4];
};

struct v4l2_modulator
{
 __u32 index;
 __u8 name[32];
 __u32 capability;
 __u32 rangelow;
 __u32 rangehigh;
 __u32 txsubchans;
 __u32 reserved[4];
};

#define V4L2_TUNER_CAP_LOW 0x0001
#define V4L2_TUNER_CAP_NORM 0x0002
#define V4L2_TUNER_CAP_STEREO 0x0010
#define V4L2_TUNER_CAP_LANG2 0x0020
#define V4L2_TUNER_CAP_SAP 0x0020
#define V4L2_TUNER_CAP_LANG1 0x0040

#define V4L2_TUNER_SUB_MONO 0x0001
#define V4L2_TUNER_SUB_STEREO 0x0002
#define V4L2_TUNER_SUB_LANG2 0x0004
#define V4L2_TUNER_SUB_SAP 0x0004
#define V4L2_TUNER_SUB_LANG1 0x0008

#define V4L2_TUNER_MODE_MONO 0x0000
#define V4L2_TUNER_MODE_STEREO 0x0001
#define V4L2_TUNER_MODE_LANG2 0x0002
#define V4L2_TUNER_MODE_SAP 0x0002
#define V4L2_TUNER_MODE_LANG1 0x0003
#define V4L2_TUNER_MODE_LANG1_LANG2 0x0004

struct v4l2_frequency
{
 __u32 tuner;
 enum v4l2_tuner_type type;
 __u32 frequency;
 __u32 reserved[8];
};

struct v4l2_audio
{
 __u32 index;
 __u8 name[32];
 __u32 capability;
 __u32 mode;
 __u32 reserved[2];
};

#define V4L2_AUDCAP_STEREO 0x00001
#define V4L2_AUDCAP_AVL 0x00002

#define V4L2_AUDMODE_AVL 0x00001

struct v4l2_audioout
{
 __u32 index;
 __u8 name[32];
 __u32 capability;
 __u32 mode;
 __u32 reserved[2];
};

struct v4l2_vbi_format
{
 __u32 sampling_rate;
 __u32 offset;
 __u32 samples_per_line;
 __u32 sample_format;
 __s32 start[2];
 __u32 count[2];
 __u32 flags;
 __u32 reserved[2];
};

#define V4L2_VBI_UNSYNC (1<< 0)
#define V4L2_VBI_INTERLACED (1<< 1)

struct v4l2_sliced_vbi_format
{
 __u16 service_set;

 __u16 service_lines[2][24];
 __u32 io_size;
 __u32 reserved[2];
};

#define V4L2_SLICED_TELETEXT_B (0x0001)

#define V4L2_SLICED_VPS (0x0400)

#define V4L2_SLICED_CAPTION_525 (0x1000)

#define V4L2_SLICED_WSS_625 (0x4000)

#define V4L2_SLICED_VBI_525 (V4L2_SLICED_CAPTION_525)
#define V4L2_SLICED_VBI_625 (V4L2_SLICED_TELETEXT_B | V4L2_SLICED_VPS | V4L2_SLICED_WSS_625)

struct v4l2_sliced_vbi_cap
{
 __u16 service_set;

 __u16 service_lines[2][24];
 __u32 reserved[4];
};

struct v4l2_sliced_vbi_data
{
 __u32 id;
 __u32 field;
 __u32 line;
 __u32 reserved;
 __u8 data[48];
};

struct v4l2_format
{
 enum v4l2_buf_type type;
 union
 {
 struct v4l2_pix_format pix;
 struct v4l2_window win;
 struct v4l2_vbi_format vbi;
 struct v4l2_sliced_vbi_format sliced;
 __u8 raw_data[200];
 } fmt;
};

struct v4l2_streamparm
{
 enum v4l2_buf_type type;
 union
 {
 struct v4l2_captureparm capture;
 struct v4l2_outputparm output;
 __u8 raw_data[200];
 } parm;
};

#define VIDIOC_QUERYCAP _IOR ('V', 0, struct v4l2_capability)
#define VIDIOC_RESERVED _IO ('V', 1)
#define VIDIOC_ENUM_FMT _IOWR ('V', 2, struct v4l2_fmtdesc)
#define VIDIOC_G_FMT _IOWR ('V', 4, struct v4l2_format)
#define VIDIOC_S_FMT _IOWR ('V', 5, struct v4l2_format)
#define VIDIOC_REQBUFS _IOWR ('V', 8, struct v4l2_requestbuffers)
#define VIDIOC_QUERYBUF _IOWR ('V', 9, struct v4l2_buffer)
#define VIDIOC_G_FBUF _IOR ('V', 10, struct v4l2_framebuffer)
#define VIDIOC_S_FBUF _IOW ('V', 11, struct v4l2_framebuffer)
#define VIDIOC_OVERLAY _IOW ('V', 14, int)
#define VIDIOC_QBUF _IOWR ('V', 15, struct v4l2_buffer)
#define VIDIOC_DQBUF _IOWR ('V', 17, struct v4l2_buffer)
#define VIDIOC_STREAMON _IOW ('V', 18, int)
#define VIDIOC_STREAMOFF _IOW ('V', 19, int)
#define VIDIOC_G_PARM _IOWR ('V', 21, struct v4l2_streamparm)
#define VIDIOC_S_PARM _IOWR ('V', 22, struct v4l2_streamparm)
#define VIDIOC_G_STD _IOR ('V', 23, v4l2_std_id)
#define VIDIOC_S_STD _IOW ('V', 24, v4l2_std_id)
#define VIDIOC_ENUMSTD _IOWR ('V', 25, struct v4l2_standard)
#define VIDIOC_ENUMINPUT _IOWR ('V', 26, struct v4l2_input)
#define VIDIOC_G_CTRL _IOWR ('V', 27, struct v4l2_control)
#define VIDIOC_S_CTRL _IOWR ('V', 28, struct v4l2_control)
#define VIDIOC_G_TUNER _IOWR ('V', 29, struct v4l2_tuner)
#define VIDIOC_S_TUNER _IOW ('V', 30, struct v4l2_tuner)
#define VIDIOC_G_AUDIO _IOR ('V', 33, struct v4l2_audio)
#define VIDIOC_S_AUDIO _IOW ('V', 34, struct v4l2_audio)
#define VIDIOC_QUERYCTRL _IOWR ('V', 36, struct v4l2_queryctrl)
#define VIDIOC_QUERYMENU _IOWR ('V', 37, struct v4l2_querymenu)
#define VIDIOC_G_INPUT _IOR ('V', 38, int)
#define VIDIOC_S_INPUT _IOWR ('V', 39, int)
#define VIDIOC_G_OUTPUT _IOR ('V', 46, int)
#define VIDIOC_S_OUTPUT _IOWR ('V', 47, int)
#define VIDIOC_ENUMOUTPUT _IOWR ('V', 48, struct v4l2_output)
#define VIDIOC_G_AUDOUT _IOR ('V', 49, struct v4l2_audioout)
#define VIDIOC_S_AUDOUT _IOW ('V', 50, struct v4l2_audioout)
#define VIDIOC_G_MODULATOR _IOWR ('V', 54, struct v4l2_modulator)
#define VIDIOC_S_MODULATOR _IOW ('V', 55, struct v4l2_modulator)
#define VIDIOC_G_FREQUENCY _IOWR ('V', 56, struct v4l2_frequency)
#define VIDIOC_S_FREQUENCY _IOW ('V', 57, struct v4l2_frequency)
#define VIDIOC_CROPCAP _IOWR ('V', 58, struct v4l2_cropcap)
#define VIDIOC_G_CROP _IOWR ('V', 59, struct v4l2_crop)
#define VIDIOC_S_CROP _IOW ('V', 60, struct v4l2_crop)
#define VIDIOC_G_JPEGCOMP _IOR ('V', 61, struct v4l2_jpegcompression)
#define VIDIOC_S_JPEGCOMP _IOW ('V', 62, struct v4l2_jpegcompression)
#define VIDIOC_QUERYSTD _IOR ('V', 63, v4l2_std_id)
#define VIDIOC_TRY_FMT _IOWR ('V', 64, struct v4l2_format)
#define VIDIOC_ENUMAUDIO _IOWR ('V', 65, struct v4l2_audio)
#define VIDIOC_ENUMAUDOUT _IOWR ('V', 66, struct v4l2_audioout)
#define VIDIOC_G_PRIORITY _IOR ('V', 67, enum v4l2_priority)
#define VIDIOC_S_PRIORITY _IOW ('V', 68, enum v4l2_priority)
#define VIDIOC_G_SLICED_VBI_CAP _IOR ('V', 69, struct v4l2_sliced_vbi_cap)
#define VIDIOC_LOG_STATUS _IO ('V', 70)
#define VIDIOC_G_EXT_CTRLS _IOWR ('V', 71, struct v4l2_ext_controls)
#define VIDIOC_S_EXT_CTRLS _IOWR ('V', 72, struct v4l2_ext_controls)
#define VIDIOC_TRY_EXT_CTRLS _IOWR ('V', 73, struct v4l2_ext_controls)

#ifdef __OLD_VIDIOC_

#define VIDIOC_OVERLAY_OLD _IOWR ('V', 14, int)
#define VIDIOC_S_PARM_OLD _IOW ('V', 22, struct v4l2_streamparm)
#define VIDIOC_S_CTRL_OLD _IOW ('V', 28, struct v4l2_control)
#define VIDIOC_G_AUDIO_OLD _IOWR ('V', 33, struct v4l2_audio)
#define VIDIOC_G_AUDOUT_OLD _IOWR ('V', 49, struct v4l2_audioout)
#define VIDIOC_CROPCAP_OLD _IOR ('V', 58, struct v4l2_cropcap)
#endif

#define BASE_VIDIOC_PRIVATE 192  

#endif

