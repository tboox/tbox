# preifx
include $(PRO_DIR)/prefix.mak

# module name
NAMES = \
		test \
		charset

test_CPP_FILES 	= test_cpp
test_CC_FILES 	= test_cc

ifneq ($(MM),)
test_M_FILES 	= test_m
test_MM_FILES 	= test_mm
test_MXFLAGS 	:= -framework Foundation
test_LDFLAGS 	:= -framework Foundation
endif


include $(SRC_DIR)/demo/makefile

