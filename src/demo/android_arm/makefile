# sub-makefile
include $(PRO_DIR)/prefix.mak

# module name
NAMES = test \
		time \
		http \
		bits \
		stdlib \
		sint64 \
		uint64 \
		scalar \
		memops \
		string \
		cstring \
		cookies \
		gstream \
		estream \
		zstream \
		format \
		vector \
		slist \
		dlist \
		base32 \
		base64 \
		sha \
		md5 \
		crc \
		dns \
		xml

define MAKE_MODULES
$(1)_TYPE 		= BIN
$(1)_C_FILES 	= $(1)
$(1)_LIBS 		= tbox supc++ log gcc c m dl
$(1)_OBJ_FILES 	= crtbegin_dynamic crtend_android
$(1)_LIB_DIR 	= ../../
$(1)_INC_DIR 	= ../../
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_MODULES,$(name))))

test_CPP_FILES 	= test2

include $(PRO_DIR)/suffix.mak

