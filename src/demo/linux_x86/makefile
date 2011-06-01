# sub-makefile
include $(PRO_DIR)/prefix.mak

# module name
NAMES = conv math memops dns string cstring time http cookies gstream estream zstream format vector slist xml

define MAKE_MODULES
$(1)_TYPE 		= BIN
$(1)_C_FILES 	= $(1)
$(1)_LIBS 		= tbox z m asound dl pthread
$(1)_LIB_DIR 	= ../../
$(1)_INC_DIR 	= ../../
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_MODULES,$(name))))

include $(PRO_DIR)/suffix.mak

