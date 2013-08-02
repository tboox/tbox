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
						memory/scache \
						memory/memory \
						memory/pbuffer \
						memory/sbuffer \
						memory/rbuffer \
						memory/qbuffer \
						string/pstring \
						string/sstring \
						string/rstring \
						algorithm/find \
						algorithm/sort \
						container/item \
						container/hash \
						container/queue \
						container/stack \
						container/slist \
						container/dlist \
						container/vector \
						container/iterator \
						network/dns \
						network/url \
						network/ipv4 \
						network/http \
						network/cookies \
						charset/charset \
						charset/ascii \
						charset/utf8 \
						charset/utf16 \
						charset/utf32 \
						charset/ucs2 \
						charset/ucs4 \
						charset/gb2312 \
						stream/bstream \
						stream/gstream \
						stream/gstream/http \
						stream/gstream/file \
						stream/gstream/data \
						stream/gstream/sock \
						stream/gstream/tstream/tstream \
						stream/gstream/tstream/charset \
						stream/gstream/tstream/null \
						stream/gstream/tstream/zip \
						object/object \
						object/null \
						object/data \
						object/date \
						object/opool \
						object/array \
						object/string \
						object/number \
						object/boolean \
						object/dictionary \
						zip/zip \
						zip/vlc/fixed \
						zip/vlc/golomb \
						zip/vlc/gamma \
						zip/rlc \
						utils/bits \
						utils/md5 \
						utils/sha \
						utils/crc \
						utils/url \
						utils/base32 \
						utils/base64 \
						xml/node \
						xml/reader \
						xml/writer \
						aio/aioo \
						aio/aipp \
						aio/aicp \
						libc/misc/time/time \
						libc/misc/time/mktime \
						libc/misc/time/gmmktime \
						libc/misc/time/gmtime \
						libc/misc/time/localtime \
						libc/stdio/sprintf \
						libc/stdio/snprintf \
						libc/stdio/swprintf \
						libc/stdio/vsnprintf \
						libc/stdio/vswprintf \
						libc/stdlib/stdlib \
						libc/stdlib/mbstowcs \
						libc/stdlib/wcstombs \
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
						libc/string/strlcpy \
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
						libc/string/strnirchr \
						libc/string/wcslen \
						libc/string/wcsnlen \
						libc/string/wcscat \
						libc/string/wcsncat \
						libc/string/wcscpy \
						libc/string/wcsncpy \
						libc/string/wcslcpy \
						libc/string/wcsdup \
						libc/string/wcsndup \
						libc/string/wcscmp \
						libc/string/wcsncmp \
						libc/string/wcsicmp \
						libc/string/wcsnicmp \
						libc/string/wcsstr \
						libc/string/wcsistr \
						libc/string/wcsrstr \
						libc/string/wcsirstr \
						libc/string/wcsnrstr \
						libc/string/wcsnirstr \
						libc/string/wcschr \
						libc/string/wcsichr \
						libc/string/wcsrchr \
						libc/string/wcsirchr \
						libc/string/wcsnrchr \
						libc/string/wcsnirchr \
						libm/idivi8 \
						libm/ilog2i \
						libm/isqrti \

tbox_C_FILES-$(TB_CONFIG_THIRD_ZLIB) += \
						zip/zlib \
						zip/gzip \
						zip/zlibraw \

tbox_C_FILES-$(TB_CONFIG_TYPE_FLOAT) += \
						libm/isin \
						libm/isinf \
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
						libm/sincosf \
						libm/pow

# pool
ifeq ($(TB_CONFIG_MEMORY_POOL),y)
tbox_C_FILES 			+= memory/malloc
else
tbox_C_FILES 			+= platform/$(TB_CONFIG_API_MEMORY)/malloc 
endif

# platform
tbox_C_FILES 			+= \
						platform/path \
						platform/printf \
						platform/tstore \
						platform/platform \
						platform/$(TB_CONFIG_API_DNS)/dns \
						platform/$(TB_CONFIG_API_DLL)/dynamic \
						platform/$(TB_CONFIG_API_FILE)/file \
						platform/$(TB_CONFIG_API_UTIL)/utils \
						platform/$(TB_CONFIG_API_MUTX)/mutex \
						platform/$(TB_CONFIG_API_SOCK)/socket \
						platform/$(TB_CONFIG_API_THRD)/thread \
						platform/$(TB_CONFIG_API_EVET)/event \
						platform/$(TB_CONFIG_API_EVET)/epool \
						platform/$(TB_CONFIG_API_TIME)/time \
						platform/$(TB_CONFIG_API_PRINTF)/printf \
						platform/$(TB_CONFIG_API_FILE)/directory \
						platform/$(TB_CONFIG_API_PROCESS)/process \
						platform/$(TB_CONFIG_API_ASIO)/aio/reactor/aioo \
						platform/$(TB_CONFIG_API_ASIO)/aio/reactor/aipp \
						platform/$(TB_CONFIG_API_ASIO)/aio/reactor/aicp

# flags
tbox_CXFLAGS 			+= -DTB_EXCEPTION_ENABLE -DTB_PRINT_TAG=\"tbox\" 
tbox_CXFLAGS-$(DEBUG) 	+= -DTB_TRACE_ENABLE -DTB_ASSERT_ENABLE

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
						prefix/packed.h \
						prefix/config.h \
						memory/vpool.h \
						memory/fpool.h \
						memory/tpool.h \
 						memory/gpool.h \
						memory/spool.h \
						memory/rpool.h \
						memory/scache.h \
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
						algorithm/prefix.h \
						algorithm/algorithm.h \
						algorithm/find.h \
						algorithm/sort.h \
						container/prefix.h \
						container/container.h \
						container/item.h \
						container/hash.h \
						container/slist.h \
						container/dlist.h \
						container/queue.h \
						container/stack.h \
						container/vector.h \
						container/iterator.h \
						platform/platform.h \
						platform/prefix.h \
						platform/dns.h \
						platform/file.h \
						platform/path.h \
						platform/time.h \
						platform/utils.h \
						platform/mutex.h \
						platform/tstore.h \
						platform/thread.h \
						platform/socket.h \
						platform/event.h \
						platform/epool.h \
						platform/printf.h \
						platform/atomic.h \
						platform/dynamic.h \
						platform/process.h \
						platform/atomic.h \
						platform/directory.h \
						platform/exception.h \
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
						platform/windows/prefix.h \
						platform/windows/atomic.h \
						platform/windows/exception.h \
						platform/unix/prefix.h \
						platform/unix/exception.h \
						network/prefix.h \
						network/network.h \
						network/http.h \
						network/ipv4.h \
						network/url.h \
						network/dns.h \
						network/cookies.h \
						charset/charset.h \
						charset/prefix.h \
						stream/prefix.h \
						stream/stream.h \
						stream/bstream.h \
						stream/gstream.h \
						object/prefix.h \
						object/object.h \
						object/null.h \
						object/data.h \
						object/date.h \
						object/opool.h \
						object/array.h \
						object/number.h \
						object/string.h \
						object/boolean.h \
						object/dictionary.h \
						utils/prefix.h \
						utils/utils.h \
						utils/bits.h \
						utils/sha.h \
						utils/md5.h \
						utils/crc.h \
						utils/url.h \
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
						libc/stdlib/prefix.h \
						libc/stdlib/stdlib.h \
						libc/stdio/prefix.h \
						libc/stdio/stdio.h \
						libc/string/prefix.h \
						libc/string/string.h \
						libc/misc/misc.h \
						libc/misc/prefix.h \
						libc/misc/ctype.h \
						libc/misc/limits.h \
						libc/misc/stdarg.h \
						libc/misc/setjmp.h \
						libc/misc/signal.h \
						libc/misc/time/prefix.h \
						libc/misc/time/time.h \
						libc/misc/time/type.h \
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
						libm/floor.h \
						third/prefix.h \
						third/third.h


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
ifeq ($(DEMO),y)
SUB_PROS 				= \
					  	demo/aio \
					  	demo/xml \
					  	demo/flv \
						demo/libm \
						demo/libc \
					  	demo/math \
					  	demo/utils \
					  	demo/other \
					  	demo/object \
					  	demo/memory \
					  	demo/string \
					  	demo/stream \
					  	demo/network \
					  	demo/platform \
					  	demo/algorithm \
					  	demo/container


endif

include $(PRO_DIR)/suffix.mak

