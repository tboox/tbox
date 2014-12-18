# prefix makefile

# include project
include 		$(PRO_DIR)/project.mak

# the source directory
SRC_DIR 		= $(PRO_DIR)/src

# the binary directory
BIN_DIR 		= $(PRO_DIR)/bin

# the package directory
PKG_DIR 		= $(PRO_DIR)/pkg

# the platform directory
PLAT_DIR 		= $(PRO_DIR)/plat/$(PLAT)

# include platform prefix
-include 		$(PLAT_DIR)/prefix.mak

