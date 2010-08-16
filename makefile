# main makefile

# #####################################################
# includes
# #
include config.mak

# #####################################################
# make projects
# #
ifeq ($(IS_CONFIG), yes)

# include prefix
include prefix.mak

# make all
all : .null
	$(MAKE) -C $(SRC_DIR)

# make install
install : .null
	-$(RMDIR) $(BIN_DIR)
	-$(MKDIR) $(BIN_DIR)
	-$(RMDIR) $(BIN_DIR)$(_)inc
	-$(RMDIR) $(BIN_DIR)$(_)lib
	-$(RMDIR) $(BIN_DIR)$(_)obj
	-$(MKDIR) $(BIN_DIR)$(_)inc
	-$(MKDIR) $(BIN_DIR)$(_)lib
	-$(MKDIR) $(BIN_DIR)$(_)obj
	$(MAKE) -C $(SRC_DIR)
	$(MAKE) -C $(SRC_DIR) install

# make clean
clean : .null
	$(MAKE) -C $(SRC_DIR) clean

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

config :
	# generate config.h
	-cp ${shell pwd}$(_)plat$(_)$(PLAT)$(_)config.h ${shell pwd}$(_)src$(_)config.h

	# generate config.mak
	@echo "# config"                      				> config.mak
	@echo "IS_CONFIG = yes" 							>> config.mak
	@echo ""                              				>> config.mak
	@echo "# architecture"                				>> config.mak
	@echo "PLAT =" $(PLAT) 								>> config.mak
	@echo ""                              				>> config.mak
	@echo "# root"                						>> config.mak
	@echo "PRO_DIR =" ${shell pwd} 						>> config.mak
	@echo "PRO_NAME =" ${shell basename ${shell pwd}} 	>> config.mak
	@echo "DEBUG =" $(DEBUG) 							>> config.mak
	@echo ""                              				>> config.mak
	@echo "# export"									>> config.mak
	@echo "export PLAT"					 				>> config.mak
	@echo "export PRO_DIR" 		 						>> config.mak
	@echo "export PRO_NAME" 		 					>> config.mak
	@echo "export DEBUG" 			 					>> config.mak


