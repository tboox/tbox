# sub-makefile
include $(PRO_DIR)/prefix.mak

# module name
NAMES = test \
		time \
		http \
		bitops \
		stdlib \
		sint64 \
		uint64 \
		scalar \
		memops \
		strops \
		string \
		cookies \
		gstream \
		estream \
		zstream \
		format \
		hash \
		slist \
		dlist \
		queue \
		stack \
		spool \
		fpool \
		mpool \
		vector \
		base32 \
		base64 \
		sha \
		md5 \
		crc \
		dns \
		xml

define MAKE_MODULES
$(1)_TYPE 				= BIN
$(1)_C_FILES 			= $(1)
$(1)_LIBS 				= tbox z m dl pthread
$(1)_LIB_DIR 			= ../../
$(1)_INC_DIR 			= ../../
$(1)_CPPFLAGS 			= -DTB_PRINT_TAG=\"tbox\"
$(1)_CPPFLAGS-$(DEBUG) 	= -DTB_TRACE_ENABLE -DTB_ASSERT_ENABLE
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_MODULES,$(name))))

test_CPP_FILES 	= test2

include $(PRO_DIR)/suffix.mak

