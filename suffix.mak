# suffix makefile

#############################################################
# make define
# #

# append names
NAMES += $(NAMES-y)

# append headers 
INC_FILES += $(INC_FILES-y)

# append debug cflags
ifeq ($(DEBUG),y)
CFLAGS := $(CFLAGS) $(CFLAGS_DEBUG)
else
CFLAGS := $(CFLAGS) $(CFLAGS_RELEASE) 
endif

# append source files
define MAKE_DEFINE_FILES
$(1)_C_FILES += $($(1)_C_FILES-y)
$(1)_CPP_FILES += $($(1)_CPP_FILES-y)
$(1)_ASM_FILES += $($(1)_ASM_FILES-y)
$(1)_OBJ_FILES += $($(1)_OBJ_FILES-y)
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_DEFINE_FILES,$(name))))

# remove repeat source files
define REMOVE_REPEAT_FILES
$(1)_C_FILES := $(sort $($(1)_C_FILES))
$(1)_CPP_FILES := $(sort $($(1)_CPP_FILES))
$(1)_ASM_FILES := $(sort $($(1)_ASM_FILES))
$(1)_OBJ_FILES := $(sort $($(1)_OBJ_FILES))
endef
$(foreach name, $(NAMES), $(eval $(call REMOVE_REPEAT_FILES,$(name))))


# cflags & ldflags
define MAKE_DEFINE_FLAGS
$(1)_CFLAGS := $(CFLAGS) $(addprefix $(CFLAGS-I), $(INC_DIR)) $(addprefix $(CFLAGS-I), $($(1)_INC_DIR)) $($(1)_CFLAGS)
$(1)_LDFLAGS := $(LDFLAGS) $(addprefix $(LDFLAGS-L), $(LIB_DIR)) $(addprefix $(LDFLAGS-L), $($(1)_LIB_DIR)) $(addprefix $(LDFLAGS-l), $($(1)_LIBS)) $($(1)_LDFLAGS)
$(1)_ASFLAGS := $(ASFLAGS) $(addprefix $(ASFLAGS-I), $(INC_DIR)) $(addprefix $(ASFLAGS-I), $($(1)_INC_DIR)) $($(1)_ASFLAGS)
$(1)_ARFLAGS := $(ARFLAGS)
$(1)_SHFLAGS := $(SHFLAGS) $($(1)_SHFLAGS)
endef
$(foreach name, $(NAMES), $(eval $(call MAKE_DEFINE_FLAGS,$(name))))

# generate objects & source files list
define MAKE_DEFINE_OBJS_SRCS
$(1)_OBJS := $(addsuffix $(OBJ_SUFFIX), $($(1)_FILES))
$(1)_SRCS := $(addsuffix .c, $($(1)_C_FILES)) $(addsuffix .cpp, $($(1)_CPP_FILES)) $(addsuffix $(ASM_SUFFIX), $($(1)_ASM_FILES))
endef
$(foreach name, $(NAMES), $(eval $(name)_FILES := $($(name)_C_FILES) $($(name)_CPP_FILES) $($(name)_ASM_FILES)))
$(foreach name, $(NAMES), $(eval $(call MAKE_DEFINE_OBJS_SRCS,$(name))))

#############################################################
# make all
# #

define MAKE_OBJ_C
$(2)$(OBJ_SUFFIX) : $(2).c
	$(CC) $(1) $(CFLAGS-o) $(2)$(OBJ_SUFFIX) $(2).c
endef

define MAKE_OBJ_CPP
$(2)$(OBJ_SUFFIX) : $(2).cpp
	$(CC) $(1) $(CFLAGS-o) $(2)$(OBJ_SUFFIX) $(2).cpp
endef

define MAKE_OBJ_ASM_WITH_CC
$(2)$(OBJ_SUFFIX) : $(2)$(ASM_SUFFIX)
	$(CC) $(1) $(CFLAGS-o) $(2)$(OBJ_SUFFIX) $(2)$(ASM_SUFFIX)
endef

define MAKE_OBJ_ASM_WITH_AS
$(2)$(OBJ_SUFFIX) : $(2)$(ASM_SUFFIX)
	$(AS) $(1) $(ASFLAGS-o) $(2)$(OBJ_SUFFIX) $(2)$(ASM_SUFFIX)
endef

define MAKE_ALL
$(1)_$(2)_all: $($(2)_PREFIX)$(1)$($(2)_SUFFIX)
	$($(1)_SUFFIX_CMD1)
	$($(1)_SUFFIX_CMD2)
	$($(1)_SUFFIX_CMD3)
	$($(1)_SUFFIX_CMD4)
	$($(1)_SUFFIX_CMD5)

$($(2)_PREFIX)$(1)$($(2)_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
$(foreach file, $($(1)_C_FILES), $(eval $(call MAKE_OBJ_C,$($(1)_CFLAGS),$(file))))
$(foreach file, $($(1)_CPP_FILES), $(eval $(call MAKE_OBJ_CPP,$($(1)_CFLAGS),$(file))))

$(if $(AS)
,$(foreach file, $($(1)_ASM_FILES), $(eval $(call MAKE_OBJ_ASM_WITH_AS,$($(1)_ASFLAGS),$(file))))
,$(foreach file, $($(1)_ASM_FILES), $(eval $(call MAKE_OBJ_ASM_WITH_CC,$($(1)_CFLAGS),$(file))))
)


$(BIN_PREFIX)$(1)$(BIN_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
	-$(RM) $$@
	$(LD) $(LDFLAGS-o) $$@ $$^ $($(1)_LDFLAGS)

$(LIB_PREFIX)$(1)$(LIB_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
	-$(RM) $$@
	$(AR) $($(1)_ARFLAGS) $$@ $$^
	$(if $(RANLIB),$(RANLIB) $$@,)

$(DLL_PREFIX)$(1)$(DLL_SUFFIX): $($(1)_OBJS) $(addsuffix $(OBJ_SUFFIX), $($(1)_OBJ_FILES))
	-$(RM) $$@
	$(LD) $(LDFLAGS-o) $$@ $$^ $($(1)_LDFLAGS) $($(1)_SHFLAGS)
endef


define MAKE_ALL_SUB_PROS
SUB_PROS_$(1)_all:
	$(MAKE) -C $(1)
endef

define MAKE_ALL_DEP_PROS
DEP_PROS_$(1)_all:
	$(MAKE) -C $(1)
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
	-$(RM) $($(2)_PREFIX)$(1)$($(2)_SUFFIX)
	-$(RM) $($(1)_OBJS)
endef

define MAKE_CLEAN_FILE
$(1)_clean:
	-$(RM) $(1)
endef

define MAKE_CLEAN_SUB_PROS
SUB_PROS_$(1)_clean:
	$(MAKE) -C $(1) clean
endef

define MAKE_CLEAN_DEP_PROS
DEP_PROS_$(1)_clean:
	$(MAKE) -C $(1) clean
endef

# generate full path
CLEAN_FILES := $(addprefix $(shell $(PWD))$(_), $(CLEAN_FILES))

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
INC_FILES := $(addprefix $(shell $(PWD))$(_), $(INC_FILES))
LIB_FILES := $(addprefix $(shell $(PWD))$(_), $(LIB_FILES))
BIN_FILES := $(addprefix $(shell $(PWD))$(_), $(BIN_FILES))
OBJ_FILES := $(addprefix $(shell $(PWD))$(_), $(OBJ_FILES))

INC_FILES := $(sort $(INC_FILES))
LIB_FILES := $(sort $(LIB_FILES))
BIN_FILES := $(sort $(BIN_FILES))
OBJ_FILES := $(sort $(OBJ_FILES))

INSTALL_FILES := $(INC_FILES) $(LIB_FILES) $(BIN_FILES) $(OBJ_FILES)

# generate include dir
define BIN_INC_DIR
$(dir $(patsubst $(SRC_DIR)$(_)%,$(BIN_DIR)$(_)inc$(_)$(PRO_NAME)$(_)%,$(1)))
endef

# generate library dir
define BIN_LIB_DIR
$(dir $(patsubst $(SRC_DIR)$(_)%,$(BIN_DIR)$(_)lib$(_)%,$(1)))
endef

# generate bin dir
define BIN_BIN_DIR
$(dir $(patsubst $(SRC_DIR)$(_)%,$(BIN_DIR)$(_)bin$(_)%,$(1)))
endef

# generate obj dir
define BIN_OBJ_DIR
$(dir $(patsubst $(SRC_DIR)$(_)%,$(BIN_DIR)$(_)obj$(_)%,$(1)))
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
	$(MAKE) -C $(1) install
endef

define MAKE_INSTALL_DEP_PROS
DEP_PROS_$(1)_install:
	$(MAKE) -C $(1) install
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

