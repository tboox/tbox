# prefix
include $(PRO_DIR)/prefix.mak

# module name
NAMES 					= tbox

# module type
tbox_TYPE 				= LIB

# basic files
tbox_C_FILES 			+= \
						tbox \
						math/int32 \
						math/fixed16 \
						math/random/random \
						math/random/linear \
						prefix/state \
						memory/page_pool \
						memory/impl/prefix \
						memory/impl/static_page_pool \
						memory/impl/native_page_pool \
						memory/static_pool \
						memory/static_fixed_pool \
						memory/pool \
						memory/fixed_pool \
						memory/global_pool \
						memory/string_pool \
						memory/memory \
						memory/buffer \
						memory/static_buffer \
						memory/queue_buffer \
						string/string \
						string/static_string \
						algorithm/sort \
						algorithm/heap_sort \
						algorithm/quick_sort \
						algorithm/insert_sort \
						algorithm/bubble_sort \
						algorithm/find \
						algorithm/rfind \
						algorithm/binary_find \
						algorithm/walk \
						algorithm/rwalk \
						algorithm/count \
						container/hash \
						container/heap \
						container/list \
						container/queue \
						container/stack \
						container/vector \
						container/list_entry \
						container/single_list \
						container/single_list_entry \
						container/bloom_filter \
						container/iterator \
						container/iterator/str \
						container/iterator/ptr \
						container/iterator/mem \
						container/iterator/long \
						container/iterator/size \
						container/item/str \
						container/item/ptr \
						container/item/mem \
						container/item/hash \
						container/item/true \
						container/item/long \
						container/item/size \
						container/item/uint8 \
						container/item/uint16 \
						container/item/uint32 \
						network/network \
						network/dns/cache \
						network/dns/server \
						network/dns/looker \
						network/url \
						network/ipv4 \
						network/http \
						network/cookies \
						asio/aioo \
						asio/aiop \
						stream/static_stream \
						stream/stream \
						stream/filter \
						stream/impl/stream/http \
						stream/impl/stream/file \
						stream/impl/stream/data \
						stream/impl/stream/sock \
						stream/impl/stream/filter \
						stream/impl/filter/cache \
						stream/impl/filter/chunked \
						stream/transfer \
						utils/bits \
						utils/md5 \
						utils/sha \
						utils/crc \
						utils/url \
						utils/dump \
						utils/used \
						utils/trace \
						utils/base32 \
						utils/base64 \
						utils/singleton \
						utils/lock_profiler \
						libc/misc/time/time \
						libc/misc/time/mktime \
						libc/misc/time/gmmktime \
						libc/misc/time/gmtime \
						libc/misc/time/localtime \
						libc/stdio/puts \
						libc/stdio/wputs \
						libc/stdio/printf \
						libc/stdio/wprintf \
						libc/stdio/sprintf \
						libc/stdio/snprintf \
						libc/stdio/swprintf \
						libc/stdio/vsnprintf \
						libc/stdio/vswprintf \
						libc/stdlib/stdlib \
						libc/stdlib/random \
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
						libc/string/strnstr \
						libc/string/strnistr \
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
						libm/isqrti 

# float
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

# platform
tbox_C_FILES 			+= \
						platform/dns \
						platform/page \
						platform/path \
						platform/file \
						platform/time \
						platform/sched \
						platform/timer \
						platform/ltimer \
						platform/print \
						platform/mutex \
						platform/event \
						platform/socket \
						platform/thread \
						platform/memory \
						platform/process \
						platform/dynamic \
						platform/atomic64 \
						platform/platform \
						platform/hostname \
						platform/directory \
						platform/semaphore \
						platform/backtrace \
						platform/cache_time \
						platform/thread_pool \
						platform/thread_store \
						platform/aioo \
						platform/aiop 


# arm
tbox_ASM_FILES-$(ARM) 	+= utils/opt/arm/crc

# openssl
tbox_C_FILES-$(TB_CONFIG_THIRD_HAVE_OPENSSL) += \
						network/ssl/openssl 

# polarssl
tbox_C_FILES-$(TB_CONFIG_THIRD_HAVE_POLARSSL) += \
						network/ssl/polarssl 

# xml
tbox_C_FILES-$(TB_CONFIG_MODULE_HAVE_XML) += \
						xml/node \
						xml/reader \
						xml/writer

# charset
tbox_C_FILES-$(TB_CONFIG_MODULE_HAVE_CHARSET) += \
						charset/charset \
						charset/ascii \
						charset/utf8 \
						charset/utf16 \
						charset/utf32 \
						charset/ucs2 \
						charset/ucs4 \
						charset/gb2312 \
						charset/iso8859 \
						stream/impl/filter/charset

# asio
ifeq ($(TB_CONFIG_MODULE_HAVE_ASIO),y)
tbox_C_FILES 			+= \
						asio/aico \
						asio/aicp \
						asio/http \
						asio/dns \
						stream/async_stream \
						stream/impl/async_stream/http \
						stream/impl/async_stream/file \
						stream/impl/async_stream/data \
						stream/impl/async_stream/sock \
						stream/impl/async_stream/filter \
						stream/impl/async_stream/prefix \
						stream/async_transfer \
						stream/transfer_pool \
						platform/aicp

tbox_C_FILES-$(TB_CONFIG_THIRD_HAVE_OPENSSL) += \
						asio/ssl 
tbox_C_FILES-$(TB_CONFIG_THIRD_HAVE_POLARSSL) += \
						asio/ssl 
endif

# object
ifeq ($(TB_CONFIG_MODULE_HAVE_OBJECT),y)
tbox_C_FILES 			+= \
						utils/option \
						container/item/obj \
						object/object \
						object/null \
						object/data \
						object/date \
						object/array \
						object/string \
						object/number \
						object/boolean \
						object/dictionary \
						object/reader/bin \
						object/reader/json \
						object/reader/bplist \
						object/reader/reader \
						object/writer/bin \
						object/writer/json \
						object/writer/bplist \
						object/writer/writer

tbox_C_FILES-$(TB_CONFIG_MODULE_HAVE_XML) += \
						object/reader/xml \
						object/writer/xml \
						object/reader/xplist \
						object/writer/xplist
endif

# zip
ifeq ($(TB_CONFIG_MODULE_HAVE_ZIP),y)
tbox_C_FILES 			+= \
						zip/zip \
						zip/vlc/fixed \
						zip/vlc/golomb \
						zip/vlc/gamma \
						zip/rlc \
						stream/impl/filter/zip 

tbox_C_FILES-$(TB_CONFIG_THIRD_HAVE_ZLIB) += \
						zip/zlib \
						zip/gzip \
						zip/zlibraw 
endif

# database
ifeq ($(TB_CONFIG_MODULE_HAVE_DATABASE),y)
tbox_C_FILES 			+= \
						database/sql \
						database/value 

tbox_C_FILES-$(TB_CONFIG_THIRD_HAVE_MYSQL) += \
						database/impl/mysql

tbox_C_FILES-$(TB_CONFIG_THIRD_HAVE_SQLITE3) += \
						database/impl/sqlite3
endif

# ios
ifeq ($(PLAT),ios)
tbox_M_FILES 			+= \
						platform/mach/ios/directory
endif

# android
ifeq ($(PLAT),android)
tbox_C_FILES 			+= \
						platform/linux/android/android \
						platform/linux/android/directory 
endif

# msvc
ifeq ($(PLAT),msvc)
tbox_C_FILES 			+= \
						platform/windows/interface/interface \
						platform/windows/interface/ws2_32 \
						platform/windows/interface/dbghelp \
						platform/windows/interface/mswsock \
						platform/windows/interface/kernel32 \
						platform/windows/interface/iphlpapi 
endif

# mingw
ifeq ($(PLAT),mingw)
tbox_C_FILES 			+= \
						platform/windows/interface/interface \
						platform/windows/interface/ws2_32 \
						platform/windows/interface/dbghelp \
						platform/windows/interface/mswsock \
						platform/windows/interface/kernel32 \
						platform/windows/interface/iphlpapi 
endif

# cygwin
ifeq ($(PLAT),cygwin)
tbox_C_FILES 			+= \
						platform/windows/interface/interface \
						platform/windows/interface/ws2_32 \
						platform/windows/interface/dbghelp \
						platform/windows/interface/mswsock \
						platform/windows/interface/kernel32 \
						platform/windows/interface/iphlpapi 
endif

# flags
tbox_CXFLAGS 			+= -D__tb_prefix__=\"tbox\"

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
						prefix/asm.h \
						prefix/arch.h \
						prefix/keyword.h \
						prefix/compiler.h \
						prefix/assembler.h \
						prefix/version.h \
						prefix/endian.h \
						prefix/assert.h \
						prefix/check.h \
						prefix/abort.h \
						prefix/trace.h \
						prefix/state.h \
						prefix/packed.h \
						prefix/config.h \
						prefix/prefix.S \
						prefix/asm/prefix.h \
						prefix/asm/prefix.S \
						prefix/asm/asm.h \
						prefix/asm/cache.h \
						prefix/asm/arm/prefix.h \
						prefix/asm/arm/prefix.S \
						memory/page_pool.h \
						memory/static_pool.h \
						memory/static_fixed_pool.h \
 						memory/global_pool.h \
						memory/pool.h \
						memory/fixed_pool.h \
						memory/string_pool.h \
						memory/prefix.h \
						memory/memory.h \
						memory/buffer.h \
						memory/static_buffer.h \
						memory/queue_buffer.h \
						asio/asio.h \
						asio/prefix.h \
						asio/aioo.h \
						asio/aioe.h \
						asio/aiop.h \
						asio/aicp.h \
						asio/aico.h \
						asio/aice.h \
						asio/http.h \
						asio/dns.h \
						asio/ssl.h \
						math/prefix.h \
						math/math.h \
						math/fixed.h \
						math/fixed6.h \
						math/fixed16.h \
						math/fixed30.h \
						math/int32.h \
						math/random/prefix.h \
						math/random/random.h \
						math/opt/prefix.h \
						math/opt/fixed16_x86.h \
						math/opt/fixed16_arm.h \
						math/opt/fixed16_sh4.h \
						math/opt/fixed30_x86.h \
						math/opt/fixed30_arm.h \
						math/opt/fixed30_sh4.h \
						string/prefix.h \
						string/string.h \
						string/string.h \
						string/static_string.h \
						algorithm/prefix.h \
						algorithm/algorithm.h \
						algorithm/for.h \
						algorithm/for_if.h \
						algorithm/rfor.h \
						algorithm/rfor_if.h \
						algorithm/sort.h \
						algorithm/heap_sort.h \
						algorithm/quick_sort.h \
						algorithm/insert_sort.h \
						algorithm/bubble_sort.h \
						algorithm/find.h \
						algorithm/rfind.h \
						algorithm/binary_find.h \
						algorithm/walk.h \
						algorithm/rwalk.h \
						algorithm/count.h \
						container/prefix.h \
						container/container.h \
						container/item.h \
						container/hash.h \
						container/heap.h \
						container/list.h \
						container/queue.h \
						container/stack.h \
						container/vector.h \
						container/iterator.h \
						container/list_entry.h \
						container/single_list.h \
						container/single_list_entry.h \
						container/bloom_filter.h \
						platform/platform.h \
						platform/prefix.h \
						platform/dns.h \
						platform/page.h \
						platform/file.h \
						platform/path.h \
						platform/time.h \
						platform/sched.h \
						platform/mutex.h \
						platform/timer.h \
						platform/ltimer.h \
						platform/thread.h \
						platform/socket.h \
						platform/event.h \
						platform/print.h \
						platform/atomic.h \
						platform/memory.h \
						platform/barrier.h \
						platform/dynamic.h \
						platform/process.h \
						platform/hostname.h \
						platform/spinlock.h \
						platform/atomic64.h \
						platform/directory.h \
						platform/exception.h \
						platform/backtrace.h \
						platform/semaphore.h \
						platform/cache_time.h \
						platform/thread_pool.h \
						platform/thread_store.h \
						platform/compiler/prefix.h \
						platform/compiler/gcc/prefix.h \
						platform/compiler/gcc/barrier.h \
						platform/compiler/gcc/atomic.h \
						platform/compiler/gcc/atomic64.h \
						platform/arch/frame.h \
						platform/arch/prefix.h \
						platform/arch/atomic.h \
						platform/arch/barrier.h \
						platform/arch/x86/frame.h \
						platform/arch/x86/prefix.h \
						platform/arch/x86/atomic.h \
						platform/arch/x86/barrier.h \
						platform/arch/x64/frame.h \
						platform/arch/x64/prefix.h \
						platform/arch/x64/atomic.h \
						platform/arch/x64/barrier.h \
						platform/arch/arm/prefix.h \
						platform/arch/arm/atomic.h \
						platform/windows/prefix.h \
						platform/windows/atomic.h \
						platform/windows/barrier.h \
						platform/windows/atomic64.h \
						platform/windows/exception.h \
						platform/windows/interface/prefix.h \
						platform/windows/interface/interface.h \
						platform/windows/interface/kernel32.h \
						platform/windows/interface/mswsock.h \
						platform/windows/interface/dbghelp.h \
						platform/windows/interface/iphlpapi.h \
						platform/windows/interface/ws2_32.h \
						platform/libc/prefix.h \
						platform/libc/exception.h \
						platform/mach/prefix.h \
						platform/mach/barrier.h \
						network/prefix.h \
						network/network.h \
						network/http.h \
						network/ipv4.h \
						network/url.h \
						network/ssl.h \
						network/dns/dns.h \
						network/dns/cache.h \
						network/dns/looker.h \
						network/dns/server.h \
						network/dns/prefix.h \
						network/cookies.h \
						charset/charset.h \
						charset/prefix.h \
						stream/prefix.h \
						stream/stream.h \
						stream/stream.h \
						stream/async_stream.h \
						stream/static_stream.h \
						stream/transfer_pool.h \
						stream/async_transfer.h \
						stream/transfer.h \
						stream/transfer.h \
						stream/filter.h \
						object/prefix.h \
						object/object.h \
						object/null.h \
						object/data.h \
						object/date.h \
						object/array.h \
						object/number.h \
						object/string.h \
						object/boolean.h \
						object/dictionary.h \
						object/reader/prefix.h \
						object/reader/reader.h \
						object/reader/xml.h \
						object/reader/bin.h \
						object/reader/json.h \
						object/reader/xplist.h \
						object/reader/bplist.h \
						object/writer/prefix.h \
						object/writer/writer.h \
						object/writer/xml.h \
						object/writer/bin.h \
						object/writer/json.h \
						object/writer/xplist.h \
						object/writer/bplist.h \
						utils/prefix.h \
						utils/utils.h \
						utils/bits.h \
						utils/sha.h \
						utils/md5.h \
						utils/crc.h \
						utils/url.h \
						utils/used.h \
						utils/dump.h \
						utils/trace.h \
						utils/option.h \
						utils/base32.h \
						utils/base64.h \
						utils/singleton.h \
						utils/lock_profiler.h \
						utils/opt/x86/bits.h \
						utils/opt/x86/prefix.h \
						utils/opt/arm/bits.h \
						utils/opt/arm/prefix.h \
						utils/opt/sh4/bits.h \
						utils/opt/sh4/prefix.h \
						utils/opt/gcc/bits.h \
						utils/opt/gcc/prefix.h \
						utils/opt/prefix.h \
						utils/base64.h \
						xml/xml.h \
						xml/prefix.h \
						xml/node.h \
						xml/reader.h \
						xml/writer.h \
						database/prefix.h \
						database/sql.h \
						database/value.h \
						database/database.h \
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
						libm/floor.h

# demos
ifeq ($(DEMO),y)
SUB_PROS 				= demo
endif

# suffix
include $(PRO_DIR)/suffix.mak

