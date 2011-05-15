# main makefile

# #####################################################
# includes
# #
${shell if [ ! -f "config.mak" ]; then touch config.mak; fi }
include config.mak

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
	-@$(RMDIR) $(BIN_DIR)/inc
	-@$(RMDIR) $(BIN_DIR)/lib
	-@$(RMDIR) $(BIN_DIR)/obj
	-@$(MKDIR) $(BIN_DIR)/inc
	-@$(MKDIR) $(BIN_DIR)/lib
	-@$(MKDIR) $(BIN_DIR)/obj
	@$(MAKE) --no-print-directory -C $(SRC_DIR)
	@$(MAKE) --no-print-directory -C $(SRC_DIR) install

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

# make output
output: .null
	@echo output $(PRO_NAME)
	@cat /tmp/$(PRO_NAME).out

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

# default args
ifeq ($(PLAT),)
PLAT := linux_x86
endif

ifeq ($(DEBUG),)
DEBUG := n
endif

PRO_DIR 	:= ${shell pwd}
PRO_NAME 	:= ${shell basename ${shell pwd}}

ifeq ($(CCACHE),n)
CCACHE 		:= 
else
CCACHE 		:= ${shell if [ -f "/usr/bin/ccache" ]; then echo "ccache"; elif [ -f "/usr/local/bin/ccache" ]; then echo "ccache"; else echo ""; fi }
endif

ifeq ($(DISTCC),y)
DISTCC 		:= ${shell if [ -f "/usr/bin/distcc" ]; then echo "distcc"; elif [ -f "/usr/local/bin/distcc" ]; then echo "distcc"; else echo ""; fi }
else
DISTCC 		:= 
endif

# is debug?
ifeq ($(DEBUG),y)
IS_DEBUG = 1
else
IS_DEBUG = 0
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
	@echo "# architecture"                				>> config.mak
	@echo "PLAT =" $(PLAT) 								>> config.mak
	@echo ""                              				>> config.mak
	@echo "# root"                						>> config.mak
	@echo "PRO_DIR =" $(PRO_DIR) 						>> config.mak
	@echo "PRO_NAME =" $(PRO_NAME) 						>> config.mak
	@echo "DEBUG =" $(DEBUG) 							>> config.mak
	@echo "CCACHE =" $(CCACHE) 							>> config.mak
	@echo "DISTCC =" $(DISTCC) 							>> config.mak
	@echo ""                              				>> config.mak
	@echo "# export"									>> config.mak
	@echo "export PLAT"					 				>> config.mak
	@echo "export PRO_DIR" 		 						>> config.mak
	@echo "export PRO_NAME" 		 					>> config.mak
	@echo "export DEBUG" 			 					>> config.mak
	@echo "export CCACHE" 			 					>> config.mak
	@echo "export DISTCC" 			 					>> config.mak


