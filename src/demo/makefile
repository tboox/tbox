# preifx
include $(PRO_DIR)/prefix.mak

# module name
NAMES 										= demo

# type
demo_TYPE 									= BIN

# files
demo_C_FILES 								+= \
											demo \
											flv/flv \
											flv/libflv \
											libc/time \
											libc/wchar \
											libc/string \
											libc/stdlib \
											libm/integer \
											math/random \
											utils/url \
											utils/crc \
											utils/md5 \
											utils/sha \
											utils/bits \
											utils/dump \
											utils/base32 \
											utils/base64 \
											other/test \
											stream/basic_stream/null \
											stream/basic_stream/cache \
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
											network/date \
											network/whois \
											network/spider \
											network/cookies \
											platform/file \
											platform/lock \
											platform/path \
											platform/event \
											platform/utils \
											platform/timer \
											platform/ltimer \
											platform/atomic \
											platform/process \
											platform/barrier \
											platform/atomic64 \
											platform/backtrace \
											platform/directory \
											platform/exception \
											platform/semaphore \
											platform/cache_time \
											platform/thread_pool \
											platform/thread_store \
											container/heap \
											container/hash \
											container/list \
											container/queue \
											container/stack \
											container/vector \
											container/single_list \
											container/bloom_filter \
											algorithm/find \
											algorithm/sort 

demo_C_FILES-$(TB_CONFIG_TYPE_FLOAT) 		+= \
											math/fixed \
											libm/float \
											libm/double

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_ZIP) 	+= \
											stream/basic_stream/zip

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_XML) 	+= \
											xml/reader \
											xml/writer \
											xml/document 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_OBJECT) += \
											utils/option \
											stream/basic_stream \
											object/json \
											object/bin \
											object/xml \
											object/xplist \
											object/bplist \
											object/dump 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_CHARSET) += \
											other/charset \
											stream/basic_stream/charset 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_DATABASE) += \
											database/sql 

ifeq ($(TB_CONFIG_MODULE_HAVE_ASIO),y)
demo_C_FILES 								+= \
											asio/dns \
											asio/http \
											asio/aiopc \
											asio/aiopd \
											asio/aicpc \
											asio/aicpd \
											stream/async_stream/null \
											stream/async_stream/cache \
											stream/transfer \
											stream/transfer_pool 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_CHARSET) += \
											stream/async_stream/charset 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_OBJECT) += \
											stream/async_stream

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_ZIP) 	+= \
											stream/async_stream/zip 
endif


# libs
demo_LIBS 									+= tbox$(DTYPE) 
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_OPENSSL) 	+= ssl crypto
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_POLARSSL) 	+= polarssl
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_MYSQL) 	+= mysqlclient
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_SQLITE3) 	+= sqlite3
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_ZLIB) 		+= z
demo_LIBS-y 								+= $(TB_CONFIG_DEMO_LIBS)

# path
demo_LIB_DIR 								+= ..

# flags
demo_CXFLAGS 								+= -DTB_TRACE_PREFIX=\"demo\"

# command
demo_SUFFIX_CMD1 							+= $(TB_CONFIG_DEMO_CMD1)

# suffix
include $(PRO_DIR)/suffix.mak

