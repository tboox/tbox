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
										stream/tstream \
										stream/mstream \
										stream/astream \
										stream/gstream \
										stream/gstream/zip \
										stream/gstream/null \
										stream/gstream/cache \
										stream/gstream/charset \
										stream/astream/zip \
										stream/astream/null \
										stream/astream/cache \
										stream/astream/charset \
										string/pstring \
										string/sstring \
										memory/check \
										memory/fpool \
										memory/gpool \
										memory/vpool \
										memory/spool \
										memory/rpool \
										memory/tpool \
										memory/memops \
										memory/scache \
										memory/pbuffer \
										memory/qbuffer \
										memory/sbuffer \
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
demo_LIBS 								+= tbox $(TB_CONFIG_DEMO_LIBS)
demo_LIBS-$(TB_CONFIG_HAVE_ZLIB) 		+= z
demo_LIBS-$(TB_CONFIG_HAVE_OPENSSL) 	+= ssl crypto
demo_LIBS-$(TB_CONFIG_HAVE_POLARSSL) 	+= polarssl

# path
demo_LIB_DIR 							+= ..

# flags
demo_CXFLAGS 							+= -DTB_TRACE_PREFIX=\"demo\"

# command
demo_SUFFIX_CMD1 						+= $(TB_CONFIG_DEMO_CMD1)

# suffix
include $(PRO_DIR)/suffix.mak

