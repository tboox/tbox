# sub-makefile
include $(PRO_DIR)/prefix.mak

ifeq ($(TB_CONFIG_PLAT),linux_x86)
demo_LIBS 				= z m dl pthread
demo_OBJ_FILES 			=
endif

ifeq ($(TB_CONFIG_PLAT),freebsd_x86)
demo_LIBS 				= z m pthread
demo_OBJ_FILES 			=
endif

ifeq ($(TB_CONFIG_PLAT),solaris_x86)
demo_LIBS 				= z m pthread
demo_OBJ_FILES 			=
endif

ifeq ($(TB_CONFIG_PLAT),mac_x86)
demo_LIBS 				= z m dl pthread
demo_OBJ_FILES 			=
endif

ifeq ($(TB_CONFIG_PLAT),windows_x86)
demo_LIBS 				= ws2_32
demo_OBJ_FILES 			=
endif

ifeq ($(TB_CONFIG_PLAT),linux_arm)
demo_LIBS 				= z m dl pthread
demo_OBJ_FILES 			=
endif

ifeq ($(TB_CONFIG_PLAT),android_arm)
demo_LIBS 				= supc++ log gcc c m dl
demo_OBJ_FILES 			= ../objs/crtbegin_dynamic ../objs/crtend_android
endif

ifeq ($(TB_CONFIG_PLAT),ios4_arm)
demo_LIBS 				= gcc gcc_eh stdc++ z m dl pthread
demo_OBJ_FILES 			= ../objs/crt1
endif

define MAKE_MODULES
$(1)_TYPE 				= BIN
$(1)_C_FILES 			+= $(1)
$(1)_OBJ_FILES 			+= $(demo_OBJ_FILES)
$(1)_LIBS 				+= tbox $(demo_LIBS)
$(1)_LIB_DIR 			+= ../..
$(1)_INC_DIR 			+= ../..
$(1)_CPPFLAGS 			+= -DTB_PRINT_TAG=\"tbox\"
$(1)_CPPFLAGS-$(DEBUG) 	+= -DTB_TRACE_ENABLE -DTB_ASSERT_ENABLE
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_MODULES,$(name))))

include $(PRO_DIR)/suffix.mak

