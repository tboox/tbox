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
						math/fixed16 \
						memory/vpool \
						memory/fpool \
						memory/tpool \
						memory/gpool \
						memory/spool \
						memory/rpool \
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
						container/slist \
						container/dlist \
						container/vector \
						network/dns \
						network/url \
						network/ipv4 \
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
						stream/gstream/tstream/tstream \
						stream/gstream/tstream/null \
						stream/gstream/tstream/zip \
						stream/gstream/tstream/encoding \
						zip/zip \
						zip/vlc/fixed \
						zip/vlc/golomb \
						zip/vlc/gamma \
						zip/rlc \
						zip/zlib \
						zip/gzip \
						zip/zlibraw \
						utils/bits \
						utils/time \
						utils/md5 \
						utils/sha \
						utils/crc \
						utils/base32 \
						utils/base64 \
						xml/node \
						xml/reader \
						aio/aioo \
						aio/aipp \
						aio/aicp \
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

tbox_C_FILES-$(TB_CONFIG_TYPE_FLOAT) += \
						libm/isinf \
						libm/isinff \
						libm/isnan \
						libm/isnanf \
						libm/sqrt \
						libm/sqrtf \
						libm/sin \
						libm/sinf \
						libm/cos \
						libm/cosf \
						libm/tan \
						libm/tanf \
						libm/atan \
						libm/atanf \
						libm/asin \
						libm/asinf \
						libm/acos \
						libm/acosf \
						libm/exp \
						libm/expf \
						libm/expi \
						libm/expif \
						libm/exp1 \
						libm/exp1f \
						libm/atan2 \
						libm/atan2f \
						libm/log2 \
						libm/log2f \
						libm/sincos \
						libm/sincosf 

# pool
ifeq ($(TB_CONFIG_MEMORY_POOL),n)
tbox_C_FILES 			+= \
						platform/libc/malloc 
endif

# platform
tbox_C_FILES 			+= \
						platform/$(TB_CONFIG_API_DNS)/dns \
						platform/$(TB_CONFIG_API_FILE)/file \
						platform/$(TB_CONFIG_API_UTIL)/utils \
						platform/$(TB_CONFIG_API_MUTX)/mutex \
						platform/$(TB_CONFIG_API_SOCK)/socket \
						platform/$(TB_CONFIG_API_THRD)/thread \
						platform/$(TB_CONFIG_API_EVET)/event \
						platform/$(TB_CONFIG_API_EVET)/epool \
						platform/$(TB_CONFIG_API_ASIO)/aio/reactor/aioo \
						platform/$(TB_CONFIG_API_ASIO)/aio/reactor/aipp \
						platform/$(TB_CONFIG_API_ASIO)/aio/reactor/aicp

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
						memory/vpool.h \
						memory/fpool.h \
						memory/tpool.h \
 						memory/gpool.h \
						memory/spool.h \
						memory/rpool.h \
						memory/prefix.h \
						memory/memory.h \
						memory/malloc.h \
						memory/pbuffer.h \
						memory/sbuffer.h \
						memory/rbuffer.h \
						memory/qbuffer.h \
						aio/aio.h \
						aio/prefix.h \
						aio/aioo.h \
						aio/aipp.h \
						aio/aicp.h \
						math/prefix.h \
						math/math.h \
						math/rand.h \
						math/fixed.h \
						math/fixed6.h \
						math/fixed16.h \
						math/fixed30.h \
						math/int32.h \
						math/opt/prefix.h \
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
						container/queue.h \
						container/stack.h \
						container/vector.h \
						platform/platform.h \
						platform/prefix.h \
						platform/dns.h \
						platform/file.h \
						platform/utils.h \
						platform/mutex.h \
						platform/thread.h \
						platform/socket.h \
						platform/event.h \
						platform/epool.h \
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
						platform/generic/prefix.h \
						platform/generic/atomic.h \
						network/prefix.h \
						network/network.h \
						network/http.h \
						network/ipv4.h \
						network/url.h \
						network/dns.h \
						network/cookies.h \
						encoding/encoding.h \
						encoding/prefix.h \
						stream/prefix.h \
						stream/stream.h \
						stream/bstream.h \
						stream/gstream.h \
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
						utils/opt/bits_gcc.h \
						utils/opt/prefix.h \
						utils/base64.h \
						xml/xml.h \
						xml/prefix.h \
						xml/node.h \
						xml/reader.h \
						xml/writer.h \
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
						third/prefix.h \
						third/third.h \
						libm/prefix.h \
						libm/math.h \
						libm/libm.h \
						libm/inf.h \
						libm/nan.h \
						libm/maf.h \
						libm/mif.h \
						libm/pi.h \
						libm/fabs.h \
						libm/round.h \
						libm/ceil.h \
						libm/floor.h

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
						zlib.h \
						zutil.h

zlib_C_FILES 						:= $(addprefix third/zlib/,$(zlib_C_FILES))
tbox_C_FILES-$(TB_CONFIG_THIRD_ZLIB)+= $(zlib_C_FILES)

zlib_INC_FILES 						:= $(addprefix third/zlib/,$(zlib_INC_FILES))
INC_FILES-$(TB_CONFIG_THIRD_ZLIB) 	+= $(zlib_INC_FILES)

# sub-projects
ifeq ($(TB_CONFIG_DEMO),y)
SUB_PROS 				= \
					  	demo/aio \
					  	demo/xml \
					  	demo/flv \
					  	demo/math \
					  	demo/utils \
					  	demo/other \
					  	demo/memory \
					  	demo/string \
					  	demo/stream \
					  	demo/network \
					  	demo/platform \
					  	demo/container


endif

include $(PRO_DIR)/suffix.mak

