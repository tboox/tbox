# preifx
include $(PRO_DIR)/prefix.mak

# module name
NAMES 										= demo

# type
demo_TYPE 									= BIN

# files
demo_C_FILES 								+= \
											demo \
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
											other/flv \
											other/libflv \
											stream/stream \
											stream/stream/zip \
											stream/stream/null \
											stream/stream/cache \
											stream/stream/charset \
											string/string \
											string/static_string \
											memory/check \
											memory/pool \
											memory/fixed_pool \
											memory/large_pool \
											memory/small_pool \
											memory/string_pool \
											memory/memops \
											memory/buffer \
											memory/queue_buffer \
											memory/static_buffer \
											memory/impl/static_fixed_pool \
											network/dns \
											network/url \
											network/ipv4 \
											network/http \
											network/whois \
											network/spider \
											network/cookies \
											network/impl/date \
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
											platform/processor \
											platform/backtrace \
											platform/directory \
											platform/exception \
											platform/semaphore \
											platform/cache_time \
											platform/thread_pool \
											platform/thread_store \
											container/heap \
											container/hash \
											container/stack \
											container/vector \
											container/queue \
											container/circle_queue \
											container/list \
											container/list_entry \
											container/single_list \
											container/single_list_entry \
											container/bloom_filter \
											algorithm/find \
											algorithm/sort 

demo_C_FILES-$(TB_CONFIG_TYPE_FLOAT) 		+= \
											math/fixed \
											libm/float \
											libm/double

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_XML) 	+= \
											xml/reader \
											xml/writer \
											xml/document 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_OBJECT) += \
											utils/option \
											object/json \
											object/bin \
											object/xml \
											object/xplist \
											object/bplist \
											object/dump 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_CHARSET) += \
											other/charset
											

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_DATABASE) += \
											database/sql 

demo_C_FILES-$(TB_CONFIG_MODULE_HAVE_ASIO) 	+= \
											asio/dns \
											asio/http \
											asio/httpd \
											asio/aiopc \
											asio/aiopd \
											asio/aicpc \
											asio/aicpd \
											stream/async_stream \
											stream/async_stream/zip \
											stream/async_stream/null \
											stream/async_stream/cache \
											stream/async_stream/charset \
											stream/async_transfer \
											stream/transfer_pool 


# libs
demo_LIBS 									+= tbox$(DTYPE) 
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_OPENSSL) 	+= ssl crypto
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_POLARSSL) 	+= polarssl
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_MYSQL) 	+= mysqlclient
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_SQLITE3) 	+= sqlite3
demo_LIBS-$(TB_CONFIG_THIRD_HAVE_ZLIB) 		+= z
demo_LIBS-y 								+= $(TB_CONFIG_DEMO_LIBS)

# path
demo_LIB_DIR 								+= ../tbox

# flags
demo_CXFLAGS 								+= -D__tb_prefix__=\"demo\"

# command
demo_SUFFIX_CMD1 							+= $(TB_CONFIG_DEMO_CMD1)

# suffix
include $(PRO_DIR)/suffix.mak

