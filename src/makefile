# sub-makefile
include $(PRO_DIR)/prefix.mak

# module name
NAMES 					= tbox

# module type
tbox_TYPE 				= LIB

# basic files
tbox_C_FILES 			+= \
						tbox \
						math/int32 \
						math/rand \
						math/float \
						math/fixed16 \
						memory/gpool \
						memory/malloc \
						memory/pbuffer \
						memory/sbuffer \
						memory/rbuffer \
						memory/qbuffer \
						string/pstring \
						string/sstring \
						string/rstring \
						container/item \
						container/hash \
						container/queue \
						container/stack \
						container/fpool \
						container/spool \
						container/slist \
						container/dlist \
						container/vector \
						network/http \
						network/cookies \
						encoding/encoding \
						encoding/unicode_gb2312 \
						encoding/gb2312_unicode \
						stream/bstream \
						stream/gstream \
						stream/gstream/http \
						stream/gstream/file \
						stream/gstream/data \
						stream/gstream/sock \
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
						utils/bits \
						utils/time \
						utils/md5 \
						utils/sha \
						utils/crc \
						utils/base32 \
						utils/base64 \
						xml/document \
						xml/node \
						xml/nlist \
						xml/reader \
						xml/writer \
						event/epool \
						event/eobject \
						libc/stdio/sprintf \
						libc/stdio/snprintf \
						libc/stdio/vsnprintf \
						libc/stdlib/stdlib \
						libc/string/memset \
						libc/string/memcpy \
						libc/string/memcmp \
						libc/string/memmov \
						libc/string/memdup \
						libc/string/strlen \
						libc/string/strnlen \
						libc/string/strcat \
						libc/string/strncat \
						libc/string/strcpy \
						libc/string/strncpy \
						libc/string/strdup \
						libc/string/strndup \
						libc/string/strcmp \
						libc/string/strncmp \
						libc/string/stricmp \
						libc/string/strnicmp \
						libc/string/strstr \
						libc/string/stristr \
						libc/string/strrstr \
						libc/string/strirstr \
						libc/string/strnrstr \
						libc/string/strnirstr \
						libc/string/strchr \
						libc/string/strichr \
						libc/string/strrchr \
						libc/string/strirchr \
						libc/string/strnrchr \
						libc/string/strnirchr

# pool
ifeq ($(TB_CONFIG_MEMORY_POOL_ENABLE),n)
tbox_C_FILES 			+= \
						platform/libc/malloc 
endif

# platform
tbox_C_FILES 			+= \
						platform/$(TB_CONFIG_API_FILE)/file \
						platform/$(TB_CONFIG_API_UTIL)/utils \
						platform/$(TB_CONFIG_API_MUTX)/mutex \
						platform/$(TB_CONFIG_API_SOCK)/socket \
						platform/$(TB_CONFIG_API_THRD)/thread \
						platform/$(TB_CONFIG_API_EVET)/event/event \
						platform/$(TB_CONFIG_API_EVET)/event/reactor

# flags
tbox_CPPFLAGS 			+= -DTB_PRINT_TAG=\"tbox\" 
tbox_CPPFLAGS-$(DEBUG) 	+= -DTB_TRACE_ENABLE -DTB_ASSERT_ENABLE

# header files
INC_FILES 				= \
						tbox.h \
						config.h \
						prefix.h \
						prefix/prefix.h \
						prefix/type.h \
						prefix/malloc.h \
						prefix/utils.h \
						prefix/limits.h \
						prefix/cpu.h \
						prefix/arch.h \
						prefix/keyword.h \
						prefix/compiler.h \
						prefix/version.h \
						prefix/endian.h \
						prefix/assert.h \
						prefix/check.h \
						prefix/abort.h \
						prefix/trace.h \
						prefix/config.h \
						memory/gpool.h \
						memory/prefix.h \
						memory/memory.h \
						memory/malloc.h \
						memory/pbuffer.h \
						memory/sbuffer.h \
						memory/rbuffer.h \
						memory/qbuffer.h \
						event/event.h \
						event/prefix.h \
						event/eobject.h \
						event/epool.h \
						math/prefix.h \
						math/math.h \
						math/rand.h \
						math/float.h \
						math/fixed.h \
						math/fixed6.h \
						math/fixed16.h \
						math/fixed30.h \
						math/int32.h \
						math/scalar.h \
						math/opt/prefix.h \
						math/opt/float_x86.h \
						math/opt/float_arm.h \
						math/opt/float_sh4.h \
						math/opt/fixed16_x86.h \
						math/opt/fixed16_arm.h \
						math/opt/fixed16_sh4.h \
						math/opt/fixed30_x86.h \
						math/opt/fixed30_arm.h \
						math/opt/fixed30_sh4.h \
						string/prefix.h \
						string/string.h \
						string/pstring.h \
						string/sstring.h \
						string/rstring.h \
						container/prefix.h \
						container/container.h \
						container/item.h \
						container/hash.h \
						container/slist.h \
						container/dlist.h \
						container/fpool.h \
						container/spool.h \
						container/queue.h \
						container/stack.h \
						container/vector.h \
						platform/platform.h \
						platform/prefix.h \
						platform/file.h \
						platform/utils.h \
						platform/mutex.h \
						platform/thread.h \
						platform/socket.h \
						platform/event.h \
						platform/atomic.h \
						platform/compiler/prefix.h \
						platform/compiler/gcc/prefix.h \
						platform/compiler/gcc/atomic.h \
						platform/arch/prefix.h \
						platform/arch/atomic.h \
						platform/arch/x86/prefix.h \
						platform/arch/x86/atomic.h \
						platform/arch/arm/prefix.h \
						platform/arch/arm/atomic.h \
						platform/arch/sh4/prefix.h \
						platform/arch/sh4/atomic.h \
						platform/stub/prefix.h \
						platform/stub/atomic.h \
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
						utils/prefix.h \
						utils/utils.h \
						utils/bits.h \
						utils/time.h \
						utils/sha.h \
						utils/md5.h \
						utils/crc.h \
						utils/base32.h \
						utils/base64.h \
						utils/opt/bits_x86.h \
						utils/opt/bits_arm.h \
						utils/opt/bits_sh4.h \
						utils/opt/prefix.h \
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
						libc/prefix.h \
						libc/libc.h \
						libc/ctype.h \
						libc/limits.h \
						libc/stdarg.h \
						libc/stdlib/prefix.h \
						libc/stdlib/stdlib.h \
						libc/stdio/prefix.h \
						libc/stdio/stdio.h \
						libc/string/prefix.h \
						libc/string/string.h \
						libs/prefix.h \
						libs/libs.h 

# zlib files
zlib_C_FILES 			= \
						adler32 \
						crc32 \
						infback \
						inffast \
						inflate \
						inftrees \
						uncompr \
						deflate \
						compress \
						trees \
						zutil

zlib_INC_FILES 			= \
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
SUB_PROS 				= \
					  	demo/math \
					  	demo/utils \
					  	demo/memory \
					  	demo/string \
					  	demo/gstream \
					  	demo/network \
					  	demo/platform


endif

include $(PRO_DIR)/suffix.mak

