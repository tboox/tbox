# preifx
include $(PRO_DIR)/prefix.mak

# module name
NAMES 									= demo

# type
demo_TYPE 								= BIN

# files
demo_C_FILES 							+= \
										demo \
										flv/flv \
										flv/libflv \
										xml/reader \
										xml/writer \
										xml/document \
										libc/time \
										libc/wchar \
										libc/string \
										libc/stdlib \
										libm/float \
										libm/double \
										libm/integer \
										asio/dns \
										asio/http \
										asio/aiopc \
										asio/aiopd \
										asio/aicpc \
										asio/aicpd \
										math/rand \
										math/fixed \
										utils/url \
										utils/crc \
										utils/md5 \
										utils/sha \
										utils/bits \
										utils/dump \
										utils/option \
										utils/base32 \
										utils/base64 \
										other/test \
										other/charset \
										object/json \
										object/xml \
										object/bin \
										object/xplist \
										object/bplist \
										object/dump \
										spider/spider \
										spider/queue \
										stream/transfer \
										stream/transfer_stream \
										stream/async_stream \
										stream/basic_stream \
										stream/basic_stream/zip \
										stream/basic_stream/null \
										stream/basic_stream/cache \
										stream/basic_stream/charset \
										stream/async_stream/zip \
										stream/async_stream/null \
										stream/async_stream/cache \
										stream/async_stream/charset \
										string/string_pool \
										string/scoped_string \
										string/static_string \
										memory/check \
										memory/fixed_pool \
										memory/block_pool \
										memory/global_pool \
										memory/static_fixed_pool \
										memory/static_block_pool \
										memory/static_tiny_pool \
										memory/memops \
										memory/queue_buffer \
										memory/scoped_buffer \
										memory/static_buffer \
										network/dns \
										network/url \
										network/ipv4 \
										network/http \
										network/whois \
										network/cookies \
										platform/file \
										platform/lock \
										platform/path \
										platform/ctime \
										platform/event \
										platform/utils \
										platform/timer \
										platform/tstore \
										platform/ltimer \
										platform/atomic \
										platform/process \
										platform/barrier \
										platform/atomic64 \
										platform/backtrace \
										platform/directory \
										platform/exception \
										platform/semaphore \
										platform/thread_pool \
										container/heap \
										container/hash \
										container/dlist \
										container/slist \
										container/queue \
										container/stack \
										container/vector \
										algorithm/find \
										algorithm/sort 

# libs
demo_LIBS 								+= tbox 
demo_LIBS-$(TB_CONFIG_HAVE_OPENSSL) 	+= ssl crypto
demo_LIBS-$(TB_CONFIG_HAVE_POLARSSL) 	+= polarssl
demo_LIBS-$(TB_CONFIG_HAVE_ZLIB) 		+= z
demo_LIBS-y 							+= $(TB_CONFIG_DEMO_LIBS)

# path
demo_LIB_DIR 							+= ..

# flags
demo_CXFLAGS 							+= -DTB_TRACE_PREFIX=\"demo\"

# command
demo_SUFFIX_CMD1 						+= $(TB_CONFIG_DEMO_CMD1)

# suffix
include $(PRO_DIR)/suffix.mak

