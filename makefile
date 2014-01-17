# main makefile

# ######################################################################################
# includes
# #
${shell if [ ! -f ".config.mak" ]; then touch .config.mak; fi }
include .config.mak

# ######################################################################################
# make shortcut
# #
a : all
f : config
r : rebuild
i : install
p : prefix
c : clean
u : update
o : output
e : error
w : warning
d : doc
h : help

# ######################################################################################
# make projects
# #
ifeq ($(IS_CONFIG), y)

# include prefix
include prefix.mak

# make all
all : .null
	@echo "" > /tmp/$(PRO_NAME).out
	@echo make $(PRO_NAME)
	@$(MAKE) --no-print-directory -C $(SRC_DIR) 

# make rebuild
rebuild : .null
	@$(MAKE) c
	-@$(MAKE) -j4
	@$(MAKE)
	@$(MAKE) i
	@$(MAKE) p

# make install
install : .null
	@echo "" > /tmp/$(PRO_NAME).out
	@echo install $(PRO_NAME)
	-@$(RMDIR) $(BIN_DIR)
	-@$(MKDIR) $(BIN_DIR)
	-@$(MKDIR) $(BIN_DIR)/inc
	-@$(MKDIR) $(BIN_DIR)/lib
	-@$(MKDIR) $(BIN_DIR)/obj
	@$(MAKE) --no-print-directory -C $(SRC_DIR)
	@$(MAKE) --no-print-directory -C $(SRC_DIR) install

# make prefix
prefix : .null
	-@$(MKDIR) $(PRE_DIR)/inc/$(PLAT)/$(ARCH)
	-@$(MKDIR) $(PRE_DIR)/lib/$(PLAT)/$(ARCH)
	-@$(CPDIR) $(BIN_DIR)/inc/$(PRO_NAME) $(PRE_DIR)/inc/$(PLAT)/$(ARCH)/
	-@$(CP) $(BIN_DIR)/lib/$(LIB_PREFIX)$(PRO_NAME)$(LIB_SUFFIX) $(PRE_DIR)/lib/$(PLAT)/$(ARCH)/$(LIB_PREFIX)$(PRO_NAME)$(DTYPE)$(LIB_SUFFIX)
	$(if $(PREFIX),-@$(MKDIR) $(PREFIX)/inc/$(PLAT)/$(ARCH),)
	$(if $(PREFIX),-@$(MKDIR) $(PREFIX)/lib/$(PLAT)/$(ARCH),)
	$(if $(PREFIX),-@$(CPDIR) $(BIN_DIR)/inc/$(PRO_NAME) $(PREFIX)/inc/$(PLAT)/$(ARCH)/,)
	$(if $(PREFIX),-@$(CP) $(BIN_DIR)/lib/$(LIB_PREFIX)$(PRO_NAME)$(LIB_SUFFIX) $(PREFIX)/lib/$(PLAT)/$(ARCH)/$(LIB_PREFIX)$(PRO_NAME)$(DTYPE)$(LIB_SUFFIX),)

# make lipo
lipo : .null
	./tool/lipo $(PRO_NAME) $(DEBUG) $(SDK) $(ARCH1) $(ARCH2)

# make clean
clean : .null
	@echo "" > /tmp/$(PRO_NAME).out
	@echo clean $(PRO_NAME)
	@$(MAKE) --no-print-directory -C $(SRC_DIR) clean

# make update
update : .null
	@echo "" > /tmp/$(PRO_NAME).out
	@echo update $(PRO_NAME)
	@$(MAKE) --no-print-directory -C $(SRC_DIR) update
	@$(MAKE) --no-print-directory -C $(SRC_DIR)
	@$(MAKE) --no-print-directory -C $(SRC_DIR) install
	@$(MAKE) --no-print-directory prefix

# make output
output : .null
	@echo output $(PRO_NAME)
	@cat /tmp/$(PRO_NAME).out

# make error
error : .null
	@echo error $(PRO_NAME)
	@cat /tmp/$(PRO_NAME).out | egrep -i "error|undefined|cannot|错误" | cat

# make warning
warning : .null
	@echo warning $(PRO_NAME)
	@cat /tmp/$(PRO_NAME).out | egrep warning

# make doc
doc : .null
	doxygen ./doc/doxygen/doxygen.conf

else

# ######################################################################################
# no-config
# #
all : 
	make f
	make r

rebuild :
	make f
	make r

install :
	make f
	make i

prefix :
	make f
	make p

lipo : help
clean :
	make f
	make c

update :
	make f
	make u

output : 	
error : 	
warning : 	
doc :
	make f
	make d

endif

# ######################################################################################
# null
# #

.null :

# ######################################################################################
# config
# #

# platform
PLAT := $(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i linux},linux,))
PLAT := $(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i darwin},mac,))
PLAT := $(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i cygwin},mingw,))
PLAT := $(if $(PLAT),$(PLAT),linux)

# architecture
ifeq ($(ARCH),)

ARCH := $(if $(findstring mingw,$(PLAT)),x86,$(ARCH))
ARCH := $(if $(findstring mac,$(PLAT)),x$(shell getconf LONG_BIT),$(ARCH))
ARCH := $(if $(findstring linux,$(PLAT)),x$(shell getconf LONG_BIT),$(ARCH))
ARCH := $(if $(findstring x32,$(ARCH)),x86,$(ARCH))
ARCH := $(if $(findstring ios,$(PLAT)),armv7,$(ARCH))
ARCH := $(if $(findstring android,$(PLAT)),armv7,$(ARCH))

endif

# debug
DEBUG := $(if $(DEBUG),$(DEBUG),y)

# debug type
DTYPE := $(if $(findstring y,$(DEBUG)),d,r)

# small
SMALL := $(if $(SMALL),$(SMALL),y)

# demo
DEMO := $(if $(DEMO),$(DEMO),y)

# profile
PROF := $(if $(PROF),$(PROF),n)

# zlib
ZLIB := $(if $(ZLIB),$(ZLIB),n)

# project
PRO_DIR 	:= ${shell pwd}
PRO_NAME 	:= ${shell basename ${shell pwd}}

# ccache
ifeq ($(CCACHE),n)
CCACHE 		:= 
else
CCACHE 		:= ${shell if [ -f "/usr/bin/ccache" ]; then echo "ccache"; elif [ -f "/usr/local/bin/ccache" ]; then echo "ccache"; else echo ""; fi }
endif

# distcc
ifeq ($(DISTCC),y)
DISTCC 		:= ${shell if [ -f "/usr/bin/distcc" ]; then echo "distcc"; elif [ -f "/usr/local/bin/distcc" ]; then echo "distcc"; else echo ""; fi }
else
DISTCC 		:= 
endif

config : .null
	-@cp ./plat/$(PLAT)/config.h ./src/config.h
	-@perl -pi -e "s/\[build\]/\"`date +%Y%m%d%H%M`\"/g" ./src/config.h
	@echo "config: ==================================================================="
	@echo "config: name:     " 							$(PRO_NAME)
	@echo "config: plat:     " 							$(PLAT)
	@echo "config: arch:     " 							$(ARCH)
	@echo "config: demo:     " 							$(DEMO)
	@echo "config: prof:     " 							$(PROF)
	@echo "config: zlib:     " 							$(ZLIB)
	@echo "config: debug:    " 							$(DEBUG)
	@echo "config: small:    " 							$(SMALL)
	@echo "config: prefix:   " 							$(PREFIX)
	@echo "config: bin:      " 							$(BIN)
	@echo "config: pre:      " 							$(PRE)
	@echo "config: sdk:      " 							$(SDK)
	@echo "config: ndk:      " 							$(NDK)
	@echo "config: ==================================================================="

	@echo "# config"                      				> .config.mak
	@echo "IS_CONFIG =y" 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# project"              						>> .config.mak
	@echo "PRO_DIR ="$(PRO_DIR) 						>> .config.mak
	@echo "PRO_NAME ="$(PRO_NAME) 						>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# profile"              						>> .config.mak
	@echo "PROF ="$(PROF) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# debug"              						>> .config.mak
	@echo "DEBUG ="$(DEBUG) 							>> .config.mak
	@echo "DTYPE ="$(DTYPE) 							>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# small"              						>> .config.mak
	@echo "SMALL ="$(SMALL) 							>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# prefix"              						>> .config.mak
	@echo "PREFIX ="$(PREFIX) 							>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# platform"      	          				>> .config.mak
	@echo "PLAT ="$(PLAT) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# architecture"                				>> .config.mak
	@echo "ARCH ="$(ARCH) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# demo" 			               				>> .config.mak
	@echo "DEMO ="$(DEMO) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# zlib" 			               				>> .config.mak
	@echo "ZLIB ="$(ZLIB) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# toolchain"            						>> .config.mak
	@echo "SDK ="$(SDK) 								>> .config.mak
	@echo "NDK ="$(NDK) 								>> .config.mak
	@echo "BIN ="$(BIN) 								>> .config.mak
	@echo "PRE ="$(PRE) 								>> .config.mak
	@echo "HOST ="$(HOST) 								>> .config.mak
	@echo "CCACHE ="$(CCACHE) 							>> .config.mak
	@echo "DISTCC ="$(DISTCC) 							>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# export"									>> .config.mak
	@echo "export PRO_DIR" 		 						>> .config.mak
	@echo "export PRO_NAME" 		 					>> .config.mak
	@echo "export DEBUG" 			 					>> .config.mak
	@echo "export DTYPE" 			 					>> .config.mak
	@echo "export SMALL" 			 					>> .config.mak
	@echo "export PLAT"					 				>> .config.mak
	@echo "export ARCH"					 				>> .config.mak
	@echo "export PROF"					 				>> .config.mak
	@echo "export DEMO"					 				>> .config.mak
	@echo "export ZLIB"					 				>> .config.mak
	@echo "export SDK" 				 					>> .config.mak
	@echo "export NDK" 				 					>> .config.mak
	@echo "export BIN" 				 					>> .config.mak
	@echo "export PRE" 				 					>> .config.mak
	@echo "export HOST" 			 					>> .config.mak
	@echo "export CCACHE" 			 					>> .config.mak
	@echo "export DISTCC" 			 					>> .config.mak

# ######################################################################################
# help
# #

# make help
help : .null
	@cat ./README

