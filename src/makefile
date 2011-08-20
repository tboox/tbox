# sub-makefile
include $(PRO_DIR)/prefix.mak

# module name
NAMES 				= tbox

# module type
tbox_TYPE 			= LIB

# basic files
tbox_C_FILES 		+= \
					tbox \
					math/int32 \
					math/sint64 \
					math/uint64 \
					math/float \
					math/fixed16 \
					memory/mpool \
					memory/memops \
					string/string \
					string/cstring \
					container/pool \
					container/vector \
					container/slist \
					container/dlist \
					network/http \
					network/dns \
					network/cookies \
					encoding/encoding \
					encoding/unicode_gb2312 \
					encoding/gb2312_unicode \
					stream/bstream \
					stream/gstream \
					stream/gstream/http \
					stream/gstream/file \
					stream/gstream/data \
					stream/tstream/tstream \
					stream/tstream/encoding \
					stream/tstream/zip \
					zip/zip \
					zip/vlc/fixed \
					zip/vlc/golomb \
					zip/vlc/gamma \
					zip/rlc \
					zip/zlib \
					zip/gzip \
					zip/zlibraw \
					format/format \
					format/flv \
					format/avi \
					format/mp3 \
					format/wav \
					format/bmp \
					format/gif \
					format/jpg \
					format/png \
					format/swf \
					format/zip \
					format/bz2 \
					format/gz \
					format/7z \
					format/pdf \
					format/xml \
					format/htm \
					format/txt \
					libc/sprintf \
					libc/snprintf \
					libc/vsnprintf \
					utils/time \
					utils/bits\
					utils/conv \
					utils/md5 \
					utils/sha \
					utils/crc \
					utils/base32 \
					utils/base64 \
					xml/document \
					xml/node \
					xml/nlist \
					xml/reader \
					xml/writer 

ifneq ($(TB_CONFIG_PLAT),)
tbox_C_FILES 		+= \
					platform/$(TB_CONFIG_PLAT)/file \
					platform/$(TB_CONFIG_PLAT)/utils \
					platform/$(TB_CONFIG_PLAT)/mutex \
					platform/$(TB_CONFIG_PLAT)/audio \
					platform/$(TB_CONFIG_PLAT)/socket \
					platform/$(TB_CONFIG_PLAT)/thread

ifeq ($(TB_CONFIG_MEMORY_POOL_ENABLE),n)
tbox_C_FILES 		+= \
					platform/$(TB_CONFIG_PLAT)/malloc 
endif
endif

# header files
INC_FILES 			= \
					prefix.h \
					config.h \
					tbox.h \
					memory/memory.h \
					memory/memops.h \
					memory/prefix.h \
					memory/mpool.h \
					math/math.h \
					math/prefix.h \
					math/float.h \
					math/fixed.h \
					math/fixed6.h \
					math/fixed16.h \
					math/fixed30.h \
					math/int32.h \
					math/sint64.h \
					math/uint64.h \
					math/scalar.h \
					math/opt/prefix.h \
					math/opt/fixed16_x86.h \
					math/opt/fixed16_arm.h \
					math/opt/fixed16_sh4.h \
					math/opt/float_x86.h \
					math/opt/float_arm.h \
					math/opt/float_sh4.h \
					string/prefix.h \
					string/string.h \
					string/cstring.h \
					container/prefix.h \
					container/container.h \
					container/pool.h \
					container/vector.h \
					container/slist.h \
					platform/platform.h \
					platform/prefix.h \
					platform/file.h \
					platform/utils.h \
					platform/mutex.h \
					platform/audio.h \
					platform/thread.h \
					platform/socket.h \
					network/prefix.h \
					network/network.h \
					network/http.h \
					network/dns.h \
					network/cookies.h \
					encoding/encoding.h \
					encoding/prefix.h \
					stream/prefix.h \
					stream/stream.h \
					stream/bstream.h \
					stream/gstream.h \
					format/format.h \
					format/prefix.h \
					libc/prefix.h \
					libc/libc.h \
					utils/prefix.h \
					utils/utils.h \
					utils/bits.h \
					utils/conv.h \
					utils/time.h \
					utils/sha.h \
					utils/md5.h \
					utils/crc.h \
					utils/base32.h \
					utils/base64.h \
					xml/xml.h \
					xml/prefix.h \
					xml/document.h \
					xml/node.h \
					xml/reader.h \
					xml/writer.h \
					xml/nlist.h \
					zip/prefix.h \
					zip/zip.h \
					zip/vlc.h \
					zip/rlc.h \
					zip/zlib.h \
					zip/zlibraw.h \
					zip/gzip.h \
					libs/prefix.h \
					libs/libs.h 

# zlib files
zlib_C_FILES 		= \
					adler32 \
					crc32 \
					infback \
					inffast \
					inflate \
					inftrees \
					uncompr \
					zutil

zlib_INC_FILES 		= \
					crc32.h \
					deflate.h \
					inffast.h \
					inffixed.h \
					inflate.h \
					inftrees.h \
					trees.h \
					zconf.h \
					zconf.in.h \
					zlib.h \
					zutil.h

zlib_C_FILES 						:= $(addprefix libs/zlib/,$(zlib_C_FILES))
tbox_C_FILES-$(TB_CONFIG_LIBS_ZLIB)	+= $(zlib_C_FILES)

zlib_INC_FILES 						:= $(addprefix libs/zlib/,$(zlib_INC_FILES))
INC_FILES-$(TB_CONFIG_LIBS_ZLIB) 	+= $(zlib_INC_FILES)

# sub-projects
ifeq ($(TB_CONFIG_DEMO),y)
SUB_PROS 			= demo/$(TB_CONFIG_PLAT)
endif

include $(PRO_DIR)/suffix.mak

