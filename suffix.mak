# suffix makefile

#############################################################
# make define
# #

# append names
NAMES += $(NAMES-y)

# append headers 
INC_FILES += $(INC_FILES-y)

# ccache hook compiler for optimizating make
ifneq ($(CCACHE),)
CC := $(CCACHE) $(CC)
MM := $(CCACHE) $(MM)
endif

# distcc hook compiler for optimizating make
ifneq ($(DISTCC),)
CC := $(DISTCC) $(CC)
MM := $(DISTCC) $(MM)
AS := $(DISTCC) $(AS)
endif

# append config flags
CFLAGS += $(CFLAG) $(CFLAGS-y)
CXFLAGS += $(CXFLAG) $(CXFLAGS-y)
CCFLAGS += $(CCFLAG) $(CCFLAGS-y)
MFLAGS += $(MFLAG) $(MFLAGS-y)
MMFLAGS += $(MMFLAG) $(MMFLAGS-y)
MXFLAGS += $(MXFLAG) $(MXFLAGS-y)
LDFLAGS += $(LDFLAG) $(LDFLAGS-y)
ASFLAGS += $(ASFLAG) $(ASFLAGS-y)
ARFLAGS += $(ARFLAG) $(ARFLAGS-y)
SHFLAGS += $(SHFLAG) $(SHFLAGS-y)

CFLAGS_DEBUG += $(CFLAGS_DEBUG-y)
CXFLAGS_DEBUG += $(CXFLAGS_DEBUG-y)
CCFLAGS_DEBUG += $(CCFLAGS_DEBUG-y)
MFLAGS_DEBUG += $(MFLAGS_DEBUG-y)
MMFLAGS_DEBUG += $(MMFLAGS_DEBUG-y)
MXFLAGS_DEBUG += $(MXFLAGS_DEBUG-y)
LDFLAGS_DEBUG += $(LDFLAGS_DEBUG-y)
ASFLAGS_DEBUG += $(ASFLAGS_DEBUG-y)
ARFLAGS_DEBUG += $(ARFLAGS_DEBUG-y)
SHFLAGS_DEBUG += $(SHFLAGS_DEBUG-y)

CFLAGS_RELEASE += $(CFLAGS_RELEASE-y)
CXFLAGS_RELEASE += $(CXFLAGS_RELEASE-y)
CCFLAGS_RELEASE += $(CCFLAGS_RELEASE-y)
MFLAGS_RELEASE += $(MFLAGS_RELEASE-y)
MMFLAGS_RELEASE += $(MMFLAGS_RELEASE-y)
MXFLAGS_RELEASE += $(MXFLAGS_RELEASE-y)
LDFLAGS_RELEASE += $(LDFLAGS_RELEASE-y)
ASFLAGS_RELEASE += $(ASFLAGS_RELEASE-y)
ARFLAGS_RELEASE += $(ARFLAGS_RELEASE-y)
SHFLAGS_RELEASE += $(SHFLAGS_RELEASE-y)

# append debug flags
ifeq ($(DEBUG),y)
CFLAGS += $(CFLAGS_DEBUG)
CXFLAGS += $(CXFLAGS_DEBUG)
CCFLAGS += $(CCFLAGS_DEBUG)
MFLAGS += $(MFLAGS_DEBUG)
MMFLAGS += $(MMFLAGS_DEBUG)
MXFLAGS += $(MXFLAGS_DEBUG)
LDFLAGS += $(LDFLAGS_DEBUG)
ASFLAGS += $(ASFLAGS_DEBUG)
ARFLAGS += $(ARFLAGS_DEBUG)
SHFLAGS += $(SHFLAGS_DEBUG)
else
CFLAGS += $(CFLAGS_RELEASE)
CXFLAGS += $(CXFLAGS_RELEASE)
CCFLAGS += $(CCFLAGS_RELEASE)
MFLAGS += $(MFLAGS_RELEASE)
MMFLAGS += $(MMFLAGS_RELEASE)
MXFLAGS += $(MXFLAGS_RELEASE)
LDFLAGS += $(LDFLAGS_RELEASE)
ASFLAGS += $(ASFLAGS_RELEASE)
ARFLAGS += $(ARFLAGS_RELEASE)
SHFLAGS += $(SHFLAGS_RELEASE)
endif

# suffix
LIB_SUFFIX := $(DTYPE)$(LIB_SUFFIX)
DLL_SUFFIX := $(DTYPE)$(DLL_SUFFIX)

# append source files
define MAKE_DEFINE_FILES
$(1)_C_FILES += $($(1)_C_FILES-y)
$(1)_CC_FILES += $($(1)_CC_FILES-y)
$(1)_CPP_FILES += $($(1)_CPP_FILES-y)
$(1)_M_FILES += $($(1)_M_FILES-y)
$(1)_MM_FILES += $($(1)_MM_FILES-y)
$(1)_ASM_FILES += $($(1)_ASM_FILES-y)
$(1)_OBJ_FILES += $($(1)_OBJ_FILES-y)
$(1)_INC_DIR += $($(1)_INC_DIR-y)
$(1)_LIB_DIR += $($(1)_LIB_DIR-y)
$(1)_LIBS += $($(1)_LIBS-y)
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_DEFINE_FILES,$(name))))

# remove repeat source files
define REMOVE_REPEAT_FILES
$(1)_C_FILES := $(sort $($(1)_C_FILES))
$(1)_CC_FILES := $(sort $($(1)_CC_FILES))
$(1)_CPP_FILES := $(sort $($(1)_CPP_FILES))
$(1)_M_FILES := $(sort $($(1)_M_FILES))
$(1)_MM_FILES := $(sort $($(1)_MM_FILES))
$(1)_ASM_FILES := $(sort $($(1)_ASM_FILES))
$(1)_OBJ_FILES := $(sort $($(1)_OBJ_FILES))
$(1)_INC_DIR := $(sort $($(1)_INC_DIR))
$(1)_LIB_DIR := $(sort $($(1)_LIB_DIR))
endef
$(foreach name, $(NAMES), $(eval $(call REMOVE_REPEAT_FILES,$(name))))


# cflags & ldflags
define MAKE_DEFINE_FLAGS
$(1)_CFLAGS := $(CFLAGS) $($(1)_CFLAGS) $($(1)_CFLAGS-y)
$(1)_CCFLAGS := $(CCFLAGS) $($(1)_CCFLAGS) $($(1)_CCFLAGS-y)
$(1)_CXFLAGS := $(CXFLAGS) $(addprefix $(CXFLAGS-I), $(INC_DIR)) $(addprefix $(CXFLAGS-I), $($(1)_INC_DIR)) $($(1)_CXFLAGS) $($(1)_CXFLAGS-y)
$(1)_MFLAGS := $(MFLAGS) $($(1)_MFLAGS) $($(1)_MFLAGS-y)
$(1)_MMFLAGS := $(MMFLAGS) $($(1)_MMFLAGS) $($(1)_MMFLAGS-y)
$(1)_MXFLAGS := $(MXFLAGS) $(addprefix $(MXFLAGS-I), $(INC_DIR)) $(addprefix $(MXFLAGS-I), $($(1)_INC_DIR)) $($(1)_MXFLAGS) $($(1)_MXFLAGS-y)
$(1)_LDFLAGS := $(LDFLAGS) $(addprefix $(LDFLAGS-L), $(LIB_DIR)) $(addprefix $(LDFLAGS-L), $($(1)_LIB_DIR)) $(addprefix $(LDFLAGS-l), $($(1)_LIBS)) $($(1)_LDFLAGS) $($(1)_LDFLAGS-y)
$(1)_ASFLAGS := $(ASFLAGS) $(addprefix $(ASFLAGS-I), $(INC_DIR)) $(addprefix $(ASFLAGS-I), $($(1)_INC_DIR)) $($(1)_ASFLAGS) $($(1)_ASFLAGS-y)
$(1)_ARFLAGS := $(ARFLAGS) $($(1)_ARFLAGS-y)
$(1)_SHFLAGS := $(SHFLAGS) $(addprefix $(LDFLAGS-L), $(LIB_DIR)) $(addprefix $(LDFLAGS-L), $($(1)_LIB_DIR)) $(addprefix $(LDFLAGS-l), $($(1)_LIBS)) $($(1)_SHFLAGS) $($(1)_SHFLAGS-y)
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_DEFINE_FLAGS,$(name))))

# generate objects & source files list
define MAKE_DEFINE_OBJS_SRCS
$(1)_OBJS := $(addsuffix $(OBJ_SUFFIX), $($(1)_FILES))
$(1)_SRCS := $(addsuffix .c, $($(1)_C_FILES)) $(addsuffix .cc, $($(1)_CC_FILES)) $(addsuffix .cpp, $($(1)_CPP_FILES)) $(addsuffix .m, $($(1)_M_FILES)) $(addsuffix .mm, $($(1)_MM_FILES)) $(addsuffix $(ASM_SUFFIX), $($(1)_ASM_FILES))
endef
$(foreach name, $(NAMES), $(eval $(name)_FILES := $($(name)_C_FILES) $($(name)_CC_FILES) $($(name)_CPP_FILES) $($(name)_M_FILES) $($(name)_MM_FILES) $($(name)_ASM_FILES)))
$(foreach name, $(NAMES), $(eval $(call MAKE_DEFINE_OBJS_SRCS,$(name))))

#############################################################
# make all
# #

define MAKE_OBJ_C
$(1)$(OBJ_SUFFIX) : $(1).c
	@echo $(CCACHE) $(DISTCC) compile.$(DTYPE) $(1).c
	@$(CC) $(2) $(3) $(CXFLAGS-o) $(1)$(OBJ_SUFFIX) $(1).c 2>>/tmp/$(PRO_NAME).out
endef

define MAKE_OBJ_CC
$(1)$(OBJ_SUFFIX) : $(1).cc
	@echo $(CCACHE) $(DISTCC) compile.$(DTYPE) $(1).cc
	@$(CC) $(2) $(3) $(CXFLAGS-o) $(1)$(OBJ_SUFFIX) $(1).cc 2>>/tmp/$(PRO_NAME).out
endef

define MAKE_OBJ_CPP
$(1)$(OBJ_SUFFIX) : $(1).cpp
	@echo $(CCACHE) $(DISTCC) compile.$(DTYPE) $(1).cpp
	@$(CC) $(2) $(3) $(CXFLAGS-o) $(1)$(OBJ_SUFFIX) $(1).cpp 2>>/tmp/$(PRO_NAME).out
endef

define MAKE_OBJ_M
$(1)$(OBJ_SUFFIX) : $(1).m
	@echo $(CCACHE) $(DISTCC) compile.$(DTYPE) $(1).m
	@$(MM) -x objective-c $(2) $(3) $(MXFLAGS-o) $(1)$(OBJ_SUFFIX) $(1).m 2>>/tmp/$(PRO_NAME).out
endef

define MAKE_OBJ_MM
$(1)$(OBJ_SUFFIX) : $(1).mm
	@echo $(CCACHE) $(DISTCC) compile.$(DTYPE) $(1).mm
	@$(MM) -x objective-c++ $(2) $(3) $(MXFLAGS-o) $(1)$(OBJ_SUFFIX) $(1).mm 2>>/tmp/$(PRO_NAME).out
endef

define MAKE_OBJ_ASM_WITH_CC
$(1)$(OBJ_SUFFIX) : $(1)$(ASM_SUFFIX)
	@echo $(CCACHE) $(DISTCC) compile.$(DTYPE) $(1)$(ASM_SUFFIX)
	@$(CC) $(2) $(CXFLAGS-o) $(1)$(OBJ_SUFFIX) $(1)$(ASM_SUFFIX) 2>>/tmp/$(PRO_NAME).out
endef

define MAKE_OBJ_ASM_WITH_AS
$(1)$(OBJ_SUFFIX) : $(1)$(ASM_SUFFIX)
	@echo compile.$(DTYPE) $(1)$(ASM_SUFFIX)
	@$(AS) $(2) $(ASFLAGS-o) $(1)$(OBJ_SUFFIX) $(1)$(ASM_SUFFIX) 2>>/tmp/$(PRO_NAME).out
endef

define MAKE_ALL
$(1)_$(2)_all: $($(2)_PREFIX)$(1)$($(2)_SUFFIX)
	$($(1)_SUFFIX_CMD1)
	$($(1)_SUFFIX_CMD2)
	$($(1)_SUFFIX_CMD3)
	$($(1)_SUFFIX_CMD4)
	$($(1)_SUFFIX_CMD5)

$($(2)_PREFIX)$(1)$($(2)_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
$(foreach file, $($(1)_C_FILES), $(eval $(call MAKE_OBJ_C,$(file),$($(1)_CXFLAGS),$($(1)_CFLAGS))))
$(foreach file, $($(1)_CC_FILES), $(eval $(call MAKE_OBJ_CC,$(file),$($(1)_CXFLAGS),$($(1)_CCFLAGS))))
$(foreach file, $($(1)_CPP_FILES), $(eval $(call MAKE_OBJ_CPP,$(file),$($(1)_CXFLAGS),$($(1)_CCFLAGS))))
$(foreach file, $($(1)_M_FILES), $(eval $(call MAKE_OBJ_M,$(file),$($(1)_MXFLAGS),$($(1)_MFLAGS))))
$(foreach file, $($(1)_MM_FILES), $(eval $(call MAKE_OBJ_MM,$(file),$($(1)_MXFLAGS),$($(1)_MMFLAGS))))

$(if $(AS)
,$(foreach file, $($(1)_ASM_FILES), $(eval $(call MAKE_OBJ_ASM_WITH_AS,$(file),$($(1)_ASFLAGS))))
,$(foreach file, $($(1)_ASM_FILES), $(eval $(call MAKE_OBJ_ASM_WITH_CC,$(file),$($(1)_CXFLAGS))))
)


$(BIN_PREFIX)$(1)$(BIN_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
	@echo link $$@
	-@$(RM) $$@
	@$(LD) $(LDFLAGS-o) $$@ $$^ $($(1)_LDFLAGS) 2>>/tmp/$(PRO_NAME).out

$(LIB_PREFIX)$(1)$(LIB_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
	@echo link $$@
	-@$(RM) $$@
	@$(AR) $($(1)_ARFLAGS) $$@ $$^ 2>>/tmp/$(PRO_NAME).out
	$(if $(RANLIB),@$(RANLIB) $$@,)

$(DLL_PREFIX)$(1)$(DLL_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
	@echo link $$@
	-@$(RM) $$@
	@$(LD) $(LDFLAGS-o) $$@ $$^ $($(1)_SHFLAGS) 2>>/tmp/$(PRO_NAME).out
endef


define MAKE_ALL_SUB_PROS
SUB_PROS_$(1)_all:
	@echo make $(1)
	@$(MAKE) --no-print-directory -C $(1) 
endef

define MAKE_ALL_DEP_PROS
DEP_PROS_$(1)_all:
	@echo make $(1)
	@$(MAKE) --no-print-directory -C $(1)
endef

all: $(foreach pro, $(DEP_PROS), DEP_PROS_$(pro)_all) $(foreach name, $(NAMES), $(if $($(name)_FILES), $(name)_$($(name)_TYPE)_all, )) $(foreach pro, $(SUB_PROS), SUB_PROS_$(pro)_all)
$(foreach name, $(NAMES), $(if $($(name)_FILES), $(eval $(call MAKE_ALL,$(name),$($(name)_TYPE))), ))
$(foreach pro, $(DEP_PROS), $(eval $(call MAKE_ALL_DEP_PROS,$(pro))))
$(foreach pro, $(SUB_PROS), $(eval $(call MAKE_ALL_SUB_PROS,$(pro))))

#############################################################
# make clean
# #
define MAKE_CLEAN
$(1)_$(2)_clean:
	-@$(RM) $($(2)_PREFIX)$(1)$($(2)_SUFFIX)
	-@$(RM) $($(1)_OBJS)
endef

define MAKE_CLEAN_FILE
$(1)_clean:
	-@$(RM) $(1)
endef

define MAKE_CLEAN_SUB_PROS
SUB_PROS_$(1)_clean:
	@echo clean $(1)
	@$(MAKE) --no-print-directory -C $(1) clean
endef

define MAKE_CLEAN_DEP_PROS
DEP_PROS_$(1)_clean:
	@echo clean $(1)
	@$(MAKE) --no-print-directory -C $(1) clean
endef

# generate full path
CLEAN_FILES := $(addprefix $(shell $(PWD))/, $(CLEAN_FILES))

clean: $(foreach pro, $(DEP_PROS), DEP_PROS_$(pro)_clean) \
	$(foreach name, $(NAMES), $(name)_$($(name)_TYPE)_clean) \
	$(foreach file, $(CLEAN_FILES), $(file)_clean) \
	$(foreach pro, $(SUB_PROS), SUB_PROS_$(pro)_clean)

$(foreach name, $(NAMES), $(eval $(call MAKE_CLEAN,$(name),$($(name)_TYPE))))
$(foreach file, $(CLEAN_FILES), $(eval $(call MAKE_CLEAN_FILE,$(file))))
$(foreach pro, $(DEP_PROS), $(eval $(call MAKE_CLEAN_DEP_PROS,$(pro))))
$(foreach pro, $(SUB_PROS), $(eval $(call MAKE_CLEAN_SUB_PROS,$(pro))))

#############################################################
# make install
# #

# install files
$(foreach name, $(NAMES), $(eval $(if $(findstring LIB, $($(name)_TYPE)), LIB_FILES += $(LIB_PREFIX)$(name)$(LIB_SUFFIX), )))
$(foreach name, $(NAMES), $(eval $(if $(findstring DLL, $($(name)_TYPE)), LIB_FILES += $(DLL_PREFIX)$(name)$(DLL_SUFFIX), )))
$(foreach name, $(NAMES), $(eval $(if $(findstring BIN, $($(name)_TYPE)), BIN_FILES += $(BIN_PREFIX)$(name)$(BIN_SUFFIX), )))
$(foreach name, $(NAMES), $(eval OBJ_FILES += $($(name)_OBJS)))

# generate full path
INC_FILES := $(addprefix $(shell $(PWD))/, $(INC_FILES))
LIB_FILES := $(addprefix $(shell $(PWD))/, $(LIB_FILES))
BIN_FILES := $(addprefix $(shell $(PWD))/, $(BIN_FILES))
OBJ_FILES := $(addprefix $(shell $(PWD))/, $(OBJ_FILES))

INC_FILES := $(sort $(INC_FILES))
LIB_FILES := $(sort $(LIB_FILES))
BIN_FILES := $(sort $(BIN_FILES))
OBJ_FILES := $(sort $(OBJ_FILES))

INSTALL_FILES := $(INC_FILES) $(LIB_FILES) $(BIN_FILES) $(OBJ_FILES)

# generate include dir
define BIN_INC_DIR
$(dir $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/inc/$(PRO_NAME)/%,$(1)))
endef

# generate library dir
define BIN_LIB_DIR
$(dir $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/lib/%,$(1)))
endef

# generate bin dir
define BIN_BIN_DIR
$(dir $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/bin/%,$(1)))
endef

# generate obj dir
define BIN_OBJ_DIR
$(dir $(patsubst $(SRC_DIR)/%,$(BIN_DIR)/obj/%,$(1)))
endef

define MAKE_INSTALL_INC_FILES
$(1)_install:
	-@$(MKDIR) $(call BIN_INC_DIR, $(1))
	-@$(CP) $(1) $(call BIN_INC_DIR, $(1))
endef

define MAKE_INSTALL_LIB_FILES
$(1)_install:
	-@$(MKDIR) $(call BIN_LIB_DIR, $(1))
	-@$(CP) $(1) $(call BIN_LIB_DIR, $(1))
endef

define MAKE_INSTALL_BIN_FILES
$(1)_install:
	-@$(MKDIR) $(call BIN_BIN_DIR, $(1))
	-@$(CP) $(1) $(call BIN_BIN_DIR, $(1))
endef

define MAKE_INSTALL_OBJ_FILES
$(1)_install:
	-@$(MKDIR) $(call BIN_OBJ_DIR, $(1))
	-@$(CP) $(1) $(call BIN_OBJ_DIR, $(1))
endef

define MAKE_INSTALL_SUB_PROS
SUB_PROS_$(1)_install:
	@echo install $(1)
	@$(MAKE) --no-print-directory -C $(1) install
endef

define MAKE_INSTALL_DEP_PROS
DEP_PROS_$(1)_install:
	@echo install $(1)
	@$(MAKE) --no-print-directory -C $(1) install
endef

install: $(foreach pro, $(DEP_PROS), DEP_PROS_$(pro)_install) $(foreach file, $(INSTALL_FILES), $(file)_install) $(foreach pro, $(SUB_PROS), SUB_PROS_$(pro)_install)
	$(INSTALL_SUFFIX_CMD1)
	$(INSTALL_SUFFIX_CMD2)
	$(INSTALL_SUFFIX_CMD3)
	$(INSTALL_SUFFIX_CMD4)
	$(INSTALL_SUFFIX_CMD5)

$(foreach file, $(INC_FILES), $(eval $(call MAKE_INSTALL_INC_FILES,$(file))))
$(foreach file, $(LIB_FILES), $(eval $(call MAKE_INSTALL_LIB_FILES,$(file))))
$(foreach file, $(BIN_FILES), $(eval $(call MAKE_INSTALL_BIN_FILES,$(file))))
$(foreach file, $(OBJ_FILES), $(eval $(call MAKE_INSTALL_OBJ_FILES,$(file))))
$(foreach pro, $(DEP_PROS), $(eval $(call MAKE_INSTALL_DEP_PROS,$(pro))))
$(foreach pro, $(SUB_PROS), $(eval $(call MAKE_INSTALL_SUB_PROS,$(pro))))

#############################################################
# make update 
# #

define MAKE_UPDATE_SUB_PROS
SUB_PROS_$(1)_update:
	@echo update $(1)
	@$(MAKE) --no-print-directory -C $(1) update
endef

define MAKE_UPDATE_DEP_PROS
DEP_PROS_$(1)_update:
	@echo update $(1)
	@$(MAKE) --no-print-directory -C $(1) update
endef

update: $(foreach pro, $(DEP_PROS), DEP_PROS_$(pro)_update) .null $(foreach pro, $(SUB_PROS), SUB_PROS_$(pro)_update)
	-@$(RM) *.b *.a *.so

$(foreach pro, $(DEP_PROS), $(eval $(call MAKE_UPDATE_DEP_PROS,$(pro))))
$(foreach pro, $(SUB_PROS), $(eval $(call MAKE_UPDATE_SUB_PROS,$(pro))))

#############################################################
# null
# #
.null :



