# prefix makefile

# path
SRC_DIR 		= $(PRO_DIR)/src
BIN_DIR 		= $(PRO_DIR)/bin/$(PLAT)/$(ARCH)
PRE_DIR 		= $(PRO_DIR)/pre
INC_DIR 		= $(PRO_DIR)/inc/$(PLAT)/$(ARCH)
LIB_DIR 		= $(PRO_DIR)/lib/$(PLAT)/$(ARCH)

PLAT_DIR 		= $(PRO_DIR)/plat/$(PLAT)
PSRC_DIR 		= $(PRO_DIR)/src/pro

# platform prefix
-include 		$(PLAT_DIR)/prefix.mak

