# sub-makefile
include $(PRO_DIR)/prefix.mak

define MAKE_MODULES
$(1)_TYPE 				= BIN
$(1)_C_FILES 			+= $(1)
$(1)_OBJ_FILES 			+= $(demo_OBJ_FILES)
$(1)_LIBS 				+= tbox $(TB_CONFIG_DEMO_LIBS)
$(1)_LIB_DIR 			+= ../.. $(demo_LIB_DIR)
$(1)_INC_DIR 			+= ../.. $(demo_INC_DIR)
$(1)_CXFLAGS 			+= -DTB_PRINT_TAG=\"tbox\" $(demo_CXFLAGS)
$(1)_CXFLAGS-$(DEBUG) 	+= -DTB_TRACE_ENABLE -DTB_ASSERT_ENABLE $(demo_CXFLAGS-y)
$(1)_LDFLAGS 			+= $(demo_LDFLAGS)
endef
NAMES += $(NAMES-y)
NAMES-y =
$(foreach name, $(NAMES), $(eval $(call MAKE_MODULES,$(name))))

include $(PRO_DIR)/suffix.mak

