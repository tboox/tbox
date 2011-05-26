# sub-makefile
include $(PRO_DIR)/prefix.mak

# module name
NAMES 				= tbox

# module type
tbox_TYPE 			= LIB

# basic files
tbox_C_FILES 		+= \
					tbox \
					math/exp \
					math/pow \
					math/log \
					math/sqrt \
					memory/mpool \
					memory/memops \
					string/string \
					string/cstring \
					container/pool \
					container/vector \
					container/slist \
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
					utils/time \
					utils/bits\
					utils/conv \
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
					math/prefix.h \
					math/math.h \
					math/exp.h \
					math/pow.h \
					math/log.h \
					math/abs.h \
					math/sqrt.h \
					math/round.h \
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
					stream/prefix.h \
					stream/stream.h \
					stream/bstream.h \
					stream/gstream.h \
					format/format.h \
					utils/prefix.h \
					utils/utils.h \
					utils/bits.h \
					utils/conv.h \
					utils/time.h \
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

zlib_C_FILES 					:= $(addprefix libs/zlib/,$(zlib_C_FILES))
tbox_C_FILES-$(TB_CONFIG_ZLIB) 	+= $(zlib_C_FILES)

zlib_INC_FILES 					:= $(addprefix libs/zlib/,$(zlib_INC_FILES))
INC_FILES-$(TB_CONFIG_ZLIB) 	+= $(zlib_INC_FILES)

# jpeg files
jpeg_C_FILES 		= \
					jaricom \
					jcomapi \
					jdapimin \
					jdapistd \
					jdarith \
					jdatadst \
					jdatasrc \
					jdcoefct \
					jdcolor \
					jddctmgr \
					jdhuff \
					jdinput \
					jdmainct \
					jdmarker \
					jdmaster \
					jdmerge \
					jdpostct \
					jdsample \
					jdtrans \
					jerror \
					jfdctflt \
					jfdctfst \
					jfdctint \
					jidctflt \
					jidctfst \
					jidctint \
					jquant1 \
					jquant2 \
					jutils \
					jmemmgr \
					jmemansi

jpeg_INC_FILES 		= \
					cderror.h \
					cdjpeg.h \
					jconfig.h \
					jdct.h \
					jerror.h \
					jinclude.h \
					jmemsys.h \
					jmorecfg.h \
					jpegint.h \
					jpeglib.h \
					jversion.h \
					transupp.h


jpeg_C_FILES 					:= $(addprefix libs/jpeg/,$(jpeg_C_FILES))
tbox_C_FILES-$(TB_CONFIG_JPEG) 	+= $(jpeg_C_FILES)

jpeg_INC_FILES 					:= $(addprefix libs/jpeg/,$(jpeg_INC_FILES))
INC_FILES-$(TB_CONFIG_JPEG) 	+= $(jpeg_INC_FILES)

# libmad files
libmad_C_FILES 		= \
					version \
					fixed \
					bit \
					timer \
					stream \
					frame \
					synth \
					decoder \
					layer12 \
					layer3 \
					huffman

libmad_INC_FILES 	= \
					bit.h \
					config.h \
					decoder.h \
					fixed.h \
					frame.h \
					global.h \
					huffman.h \
					layer12.h \
					layer3.h \
					mad.h \
					stream.h \
					synth.h \
					timer.h \
					version.h

libmad_C_FILES 					:= $(addprefix libs/libmad/,$(libmad_C_FILES))
tbox_C_FILES-$(TB_CONFIG_LIBMAD) += $(libmad_C_FILES)

libmad_INC_FILES 				:= $(addprefix libs/libmad/,$(libmad_INC_FILES))
INC_FILES-$(TB_CONFIG_LIBMAD) 	+= $(libmad_INC_FILES)

# sub-projects
ifeq ($(TB_CONFIG_DEMO),y)
SUB_PROS 			= demo/$(TB_CONFIG_PLAT)
endif

include $(PRO_DIR)/suffix.mak

