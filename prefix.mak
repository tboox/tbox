# prefix makefile

# the project name
PRO_NAME 		= tbox

# the source directory
SRC_DIR 		= $(PRO_DIR)/src

# the binary directory
BIN_DIR 		= $(PRO_DIR)/bin/$(PLAT)/$(ARCH)

# the prefix directory
PRE_DIR 		= $(PRO_DIR)/pre

# the include directory
INC_DIR 		= $(PRO_DIR)/inc/$(PLAT)/$(ARCH) $(PRO_DIR)/inc

# the library directory
LIB_DIR 		= $(PRO_DIR)/lib/$(PLAT)/$(ARCH)

# the platform directory
PLAT_DIR 		= $(PRO_DIR)/plat/$(PLAT)

# platform prefix
-include 		$(PLAT_DIR)/prefix.mak

