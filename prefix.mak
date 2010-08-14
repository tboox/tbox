# prefix makefile

# path seperator
_ 				= /

# path
SRC_DIR 		= $(PRO_DIR)$(_)src
BIN_DIR 		= $(PRO_DIR)$(_)bin$(_)$(PLAT)
INC_DIR 		= $(PRO_DIR)$(_)inc$(_)$(PLAT)
LIB_DIR 		= $(PRO_DIR)$(_)lib$(_)$(PLAT)
PLAT_DIR 		= $(PRO_DIR)$(_)plat$(_)$(PLAT)
PSRC_DIR 		= $(PRO_DIR)$(_)src$(_)pro

# default path
INC_DIR_DEF 	= $(PRO_DIR)$(_)inc$(_)$(PLAT)
LIB_DIR_DEF 	= $(PRO_DIR)$(_)lib$(_)$(PLAT)

# eplat 
INC_DIR 		+= $(PRO_DIR)$(_)..$(_)eplat$(_)bin$(_)$(PLAT)$(_)inc
LIB_DIR 		+= $(PRO_DIR)$(_)..$(_)eplat$(_)bin$(_)$(PLAT)$(_)lib

# exports
export 			_

# architecture makefile
-include 		$(PLAT_DIR)$(_)prefix.mak

