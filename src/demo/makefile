# preifx
include $(PRO_DIR)/prefix.mak

# module name
NAMES = demo

# type
demo_TYPE 				= BIN

# files
demo_C_FILES 			+= \
						demo \
						flv/flv \
						flv/libflv \
						libc/time \
						libc/wchar \
						libc/string \
						libc/stdlib \
						libm/float \
						libm/double \
						libm/integer \
						asio/addr \
						asio/aiopc \
						asio/aiopd \
						asio/aicpc \
						asio/aicpd \
						math/rand \
						math/fixed \
						other/test \
						other/charset \
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
demo_LIBS 				+= tbox $(TB_CONFIG_DEMO_LIBS)

# path
demo_LIB_DIR 			+= ..

# flags
demo_CXFLAGS 			+= -DTB_EXCEPTION_ENABLE -DTB_PRINT_TAG=\"tbox\" 
demo_CXFLAGS-$(DEBUG) 	+= -DTB_TRACE_ENABLE -DTB_ASSERT_ENABLE 

# command
demo_SUFFIX_CMD1 		+= $(TB_CONFIG_DEMO_CMD1)

# suffix
include $(PRO_DIR)/suffix.mak

