# main makefile

# #####################################################
# includes
# #
${shell if [ ! -f "config.mak" ]; then touch config.mak; fi }
include config.mak

# #####################################################
# make shortcut
# #
a : all
f : config
i : install
p : prefix
c : clean
u : update
o : output
e : error
w : warning
d : doc

# #####################################################
# make projects
# #
ifeq ($(IS_CONFIG), yes)

# include prefix
include prefix.mak

# make all
all : .null
	@echo "" > /tmp/$(PRO_NAME).out
	@echo make $(PRO_NAME)
	@$(MAKE) --no-print-directory -C $(SRC_DIR)

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
	-@$(RMDIR) $(PRE_DIR)/inc/$(PLAT)/$(ARCH)/$(PRO_NAME)
	-@$(RM) $(PRE_DIR)/lib/$(PLAT)/$(ARCH)/$(LIB_PREFIX)$(PRO_NAME)$(LIB_SUFFIX)
	-@$(MKDIR) $(PRE_DIR)
	-@$(MKDIR) $(PRE_DIR)/inc
	-@$(MKDIR) $(PRE_DIR)/inc/$(PLAT)
	-@$(MKDIR) $(PRE_DIR)/inc/$(PLAT)/$(ARCH)
	-@$(MKDIR) $(PRE_DIR)/lib
	-@$(MKDIR) $(PRE_DIR)/lib/$(PLAT)
	-@$(MKDIR) $(PRE_DIR)/lib/$(PLAT)/$(ARCH)
	-@$(CP) -r $(BIN_DIR)/inc/$(PRO_NAME) $(PRE_DIR)/inc/$(PLAT)/$(ARCH)/
	-@$(CP) $(BIN_DIR)/lib/$(LIB_PREFIX)$(PRO_NAME)$(LIB_SUFFIX) $(PRE_DIR)/lib/$(PLAT)/$(ARCH)/

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
	@cat /tmp/$(PRO_NAME).out | grep -Pi "error|undefined"

# make warning
warning : .null
	@echo warning $(PRO_NAME)
	@cat /tmp/$(PRO_NAME).out | grep warning

# make doc
doc : .null
	doxygen ./doc/doxygen/doxygen.conf

.null :

else

# #####################################################
# no-config
# #
all :
	@echo "please make config..."

endif

# #####################################################
# config
# #

# platform
ifeq ($(PLAT),)
PLAT := linux
endif

# architecture
ifeq ($(ARCH),)
ARCH := x86
endif

# linux, cygwin, mac
ifeq ($(HOST),)
HOST := linux
endif

# debug
ifeq ($(DEBUG),)
DEBUG := n
endif

ifeq ($(DEBUG),y)
IS_DEBUG = 1
else
IS_DEBUG = 0
endif

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

config :
	# generate config.h
	-cp ${shell pwd}/plat/$(PLAT)/config.h ${shell pwd}/src/config.h

	# append config.h
	@echo "// config" 									>> ${shell pwd}/src/config.h
	@echo "#ifndef $(PRO_NAME)_AUTO_CONFIG_H" 			>> ${shell pwd}/src/config.h
	@echo "#define $(PRO_NAME)_AUTO_CONFIG_H" 			>> ${shell pwd}/src/config.h
	@echo "#define $(PRO_NAME)_CONFIG_DEBUG $(IS_DEBUG)">> ${shell pwd}/src/config.h
	@echo "#endif" 										>> ${shell pwd}/src/config.h

	# generate config.mak
	@echo "# config"                      				> config.mak
	@echo "IS_CONFIG = yes" 							>> config.mak
	@echo ""                              				>> config.mak
	@echo "# project"              						>> config.mak
	@echo "PRO_DIR =" $(PRO_DIR) 						>> config.mak
	@echo "PRO_NAME =" $(PRO_NAME) 						>> config.mak
	@echo ""                              				>> config.mak
	@echo "# debug"              						>> config.mak
	@echo "DEBUG =" $(DEBUG) 							>> config.mak
	@echo ""                              				>> config.mak
	@echo "# platform"      	          				>> config.mak
	@echo "PLAT =" $(PLAT) 								>> config.mak
	@echo ""                              				>> config.mak
	@echo "# architecture"                				>> config.mak
	@echo "ARCH =" $(ARCH) 								>> config.mak
	@echo ""                              				>> config.mak
	@echo "# toolchain"            						>> config.mak
	@echo "SDK =" $(SDK) 								>> config.mak
	@echo "NDK =" $(NDK) 								>> config.mak
	@echo "BIN =" $(BIN) 								>> config.mak
	@echo "HOST =" $(HOST) 								>> config.mak
	@echo "CCACHE =" $(CCACHE) 							>> config.mak
	@echo "DISTCC =" $(DISTCC) 							>> config.mak
	@echo ""                              				>> config.mak
	@echo "# export"									>> config.mak
	@echo "export PRO_DIR" 		 						>> config.mak
	@echo "export PRO_NAME" 		 					>> config.mak
	@echo "export DEBUG" 			 					>> config.mak
	@echo "export PLAT"					 				>> config.mak
	@echo "export ARCH"					 				>> config.mak
	@echo "export SDK" 				 					>> config.mak
	@echo "export NDK" 				 					>> config.mak
	@echo "export BIN" 				 					>> config.mak
	@echo "export HOST" 			 					>> config.mak
	@echo "export CCACHE" 			 					>> config.mak
	@echo "export DISTCC" 			 					>> config.mak


