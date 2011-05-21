# prefix makefile

# path
SRC_DIR 		= $(PRO_DIR)/src
BIN_DIR 		= $(PRO_DIR)/bin/$(PLAT)
INC_DIR 		= $(PRO_DIR)/inc/$(PLAT)
LIB_DIR 		= $(PRO_DIR)/lib/$(PLAT)
PLAT_DIR 		= $(PRO_DIR)/plat/$(PLAT)
PSRC_DIR 		= $(PRO_DIR)/src/pro

# default path
INC_DIR_DEF 	= $(PRO_DIR)/inc/$(PLAT)
LIB_DIR_DEF 	= $(PRO_DIR)/lib/$(PLAT)

# tplat 
INC_DIR 		+= $(PRO_DIR)/../tplat/bin/$(PLAT)/inc
LIB_DIR 		+= $(PRO_DIR)/../tplat/bin/$(PLAT)/lib

# architecture makefile
-include 		$(PLAT_DIR)/prefix.mak

