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
	$(if $(findstring msys,$(HOST)),,-@$(MAKE) -j4)
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
	-@$(CPDIR) $(BIN_DIR)/inc/$(PRO_NAME)/* $(PRE_DIR)/inc/$(PLAT)/$(ARCH)/
	-@$(CP) $(BIN_DIR)/lib/$(PRO_NAME)/* $(PRE_DIR)/lib/$(PLAT)/$(ARCH)/
	$(if $(PREFIX),-@$(MKDIR) $(PREFIX)/inc/$(PLAT)/$(ARCH),)
	$(if $(PREFIX),-@$(MKDIR) $(PREFIX)/lib/$(PLAT)/$(ARCH),)
	$(if $(PREFIX),-@$(CPDIR) $(BIN_DIR)/inc/$(PRO_NAME)/* $(PREFIX)/inc/$(PLAT)/$(ARCH)/,)
	$(if $(PREFIX),-@$(CP) $(BIN_DIR)/lib/$(PRO_NAME)/* $(PREFIX)/lib/$(PLAT)/$(ARCH)/,)

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

# include project
include project.mak

# ######################################################################################
# no-config
# #
all : 
	make -r f
	make -r r

rebuild :
	make -r f
	make -r r

install :
	make -r f
	make -r i

prefix :
	make -r f
	make -r p

lipo : help
clean :
	make -r f
	make -r c

update :
	make -r f
	make -r u

output : 	
error : 	
warning : 	
doc :
	make -r f
	make -r d

endif

# ######################################################################################
# null
# #

.null :

# ######################################################################################
# config
# #

# host
HOST :=$(if $(HOST),$(HOST),$(if ${shell uname | egrep -i linux},linux,))
HOST :=$(if $(HOST),$(HOST),$(if ${shell uname | egrep -i darwin},mac,))
HOST :=$(if $(HOST),$(HOST),$(if ${shell uname | egrep -i cygwin},cygwin,))
HOST :=$(if $(HOST),$(HOST),$(if ${shell uname | egrep -i mingw},msys,))
HOST :=$(if $(HOST),$(HOST),$(if ${shell uname | egrep -i msvc},msys,))
HOST :=$(if $(HOST),$(HOST),linux)

# platform
PLAT :=$(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i linux},linux,))
PLAT :=$(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i darwin},mac,))
PLAT :=$(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i cygwin},cygwin,))
PLAT :=$(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i mingw},mingw,))
PLAT :=$(if $(PLAT),$(PLAT),$(if ${shell uname | egrep -i msvc},msvc,))
PLAT :=$(if $(PLAT),$(PLAT),linux)

# architecture
ifeq ($(ARCH),)

ARCH :=$(if $(findstring msvc,$(PLAT)),x86,$(ARCH))
ARCH :=$(if $(findstring mingw,$(PLAT)),x86,$(ARCH))
ARCH :=$(if $(findstring mac,$(PLAT)),x$(shell getconf LONG_BIT),$(ARCH))
ARCH :=$(if $(findstring linux,$(PLAT)),x$(shell getconf LONG_BIT),$(ARCH))
ARCH :=$(if $(findstring x32,$(ARCH)),x86,$(ARCH))
ARCH :=$(if $(findstring ios,$(PLAT)),armv7,$(ARCH))
ARCH :=$(if $(findstring android,$(PLAT)),armv7,$(ARCH))

endif

# debug
DEBUG :=$(if $(DEBUG),$(DEBUG),y)

# debug type
DTYPE :=$(if $(findstring y,$(DEBUG)),d,r)

# small
SMALL :=$(if $(SMALL),$(SMALL),n)
SMALL :=$(if $(findstring ios,$(PLAT)),y,$(SMALL))
SMALL :=$(if $(findstring android,$(PLAT)),y,$(SMALL))

# demo
DEMO :=$(if $(DEMO),$(DEMO),y)

# profile
PROF :=$(if $(PROF),$(PROF),n)

# arm
ARM :=$(if $(findstring arm,$(ARCH)),y,n)

# x86
x86 :=$(if $(findstring x86,$(ARCH)),y,n)

# x64
x64 :=$(if $(findstring x64,$(ARCH)),y,n)

# sh4
SH4 :=$(if $(findstring sh4,$(ARCH)),y,n)

# mips
MIPS :=$(if $(findstring mips,$(ARCH)),y,n)

# sparc
SPARC :=$(if $(findstring sparc,$(ARCH)),y,n)

# the project directory
PRO_DIR 	:=${shell pwd}

# flag
CXFLAG 		:= $(if $(CXFLAG),$(CXFLAG),)

# ccache
ifeq ($(CCACHE),n)
CCACHE 		:= 
else
CCACHE 		:=${shell if [ -f "/usr/bin/ccache" ]; then echo "ccache"; elif [ -f "/usr/local/bin/ccache" ]; then echo "ccache"; else echo ""; fi }
endif

# distcc
ifeq ($(DISTCC),y)
DISTCC 		:=${shell if [ -f "/usr/bin/distcc" ]; then echo "distcc"; elif [ -f "/usr/local/bin/distcc" ]; then echo "distcc"; else echo ""; fi }
else
DISTCC 		:= 
endif

# sed
ifeq ($(HOST),mac)
SED 		:= sed -i ''
#SED 		:= perl -pi -e
else
SED 		:= sed -i
endif

# config
config : .null
	-@cp ./plat/$(PLAT)/config.h ./src/$(PRO_NAME)/config.h
	-@$(SED) "s/\[major\]/$(PRO_VERSION_MAJOR)/g" ./src/$(PRO_NAME)/config.h
	-@$(SED) "s/\[minor\]/$(PRO_VERSION_MINOR)/g" ./src/$(PRO_NAME)/config.h
	-@$(SED) "s/\[alter\]/$(PRO_VERSION_ALTER)/g" ./src/$(PRO_NAME)/config.h
	-@$(SED) "s/\[build\]/`date +%Y%m%d%H%M`/g" ./src/$(PRO_NAME)/config.h
	-@$(SED) "s/\[debug\]/\($(if $(findstring y,$(DEBUG)),1,0)\)/g" ./src/$(PRO_NAME)/config.h
	-@$(SED) "s/\[small\]/\($(if $(findstring y,$(SMALL)),1,0)\)/g" ./src/$(PRO_NAME)/config.h
	@echo "config: ==================================================================="
	@echo "config: plat:     " 							$(PLAT)
	@echo "config: arch:     " 							$(ARCH)
	@echo "config: host:     " 							$(HOST)
	@echo "config: demo:     " 							$(DEMO)
	@echo "config: prof:     " 							$(PROF)
	@echo "config: debug:    " 							$(DEBUG)
	@echo "config: small:    " 							$(SMALL)
	@echo "config: cflag:    " 							$(CFLAG)
	@echo "config: ccflag:   " 							$(CCFLAG)
	@echo "config: cxflag:   " 							$(CXFLAG)
	@echo "config: mflag:    " 							$(MFLAG)
	@echo "config: mmflag:   " 							$(MMFLAG)
	@echo "config: mxflag:   " 							$(MXFLAG)
	@echo "config: ldflag:   " 							$(LDFLAG)
	@echo "config: asflag:   " 							$(ASFLAG)
	@echo "config: arflag:   " 							$(ARFLAG)
	@echo "config: shflag:   " 							$(SHFLAG)
	@echo "config: ccache:   " 							$(CCACHE)
	@echo "config: distcc:   " 							$(DISTCC)
	@echo "config: prefix:   " 							$(PREFIX)
	@echo "config: bin:      " 							$(BIN)
	@echo "config: pre:      " 							$(PRE)
	@echo "config: sdk:      " 							$(SDK)
	@echo "config: ==================================================================="

	@echo "# config"                      				> .config.mak
	@echo "IS_CONFIG =y" 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# project"              						>> .config.mak
	@echo "PRO_DIR ="$(PRO_DIR) 						>> .config.mak
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
	@echo "# host"      		          				>> .config.mak
	@echo "HOST ="$(HOST) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# flags"      		          				>> .config.mak
	@echo "CFLAG ="$(CFLAG) 							>> .config.mak
	@echo "CCFLAG ="$(CCFLAG) 							>> .config.mak
	@echo "CXFLAG ="$(CXFLAG) 							>> .config.mak
	@echo "MFLAG ="$(MFLAG) 							>> .config.mak
	@echo "MMFLAG ="$(MMFLAG) 							>> .config.mak
	@echo "MXFLAG ="$(MXFLAG) 							>> .config.mak
	@echo "LDFLAG ="$(LDFLAG) 							>> .config.mak
	@echo "ASFLAG ="$(ASFLAG) 							>> .config.mak
	@echo "ARFLAG ="$(ARFLAG) 							>> .config.mak
	@echo "SHFLAG ="$(SHFLAG) 							>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# platform"      	          				>> .config.mak
	@echo "PLAT ="$(PLAT) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# architecture"                				>> .config.mak
	@echo "ARCH ="$(ARCH) 								>> .config.mak
	@echo "ARM ="$(ARM) 								>> .config.mak
	@echo "x86 ="$(x86) 								>> .config.mak
	@echo "x64 ="$(x64) 								>> .config.mak
	@echo "SH4 ="$(SH4) 								>> .config.mak
	@echo "MIPS ="$(MIPS) 								>> .config.mak
	@echo "SPARC ="$(SPARC) 							>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# demo" 			               				>> .config.mak
	@echo "DEMO ="$(DEMO) 								>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# toolchain"            						>> .config.mak
	@echo "SDK ="$(SDK) 								>> .config.mak
	@echo "BIN ="$(BIN) 								>> .config.mak
	@echo "PRE ="$(PRE) 								>> .config.mak
	@echo "CCACHE ="$(CCACHE) 							>> .config.mak
	@echo "DISTCC ="$(DISTCC) 							>> .config.mak
	@echo ""                              				>> .config.mak
	@echo "# export"									>> .config.mak
	@echo "export PRO_DIR" 		 						>> .config.mak
	@echo "export DEBUG" 			 					>> .config.mak
	@echo "export DTYPE" 			 					>> .config.mak
	@echo "export SMALL" 			 					>> .config.mak
	@echo "export HOST"					 				>> .config.mak
	@echo "export PLAT"					 				>> .config.mak
	@echo "export ARCH"					 				>> .config.mak
	@echo "export ARM"					 				>> .config.mak
	@echo "export x86"					 				>> .config.mak
	@echo "export x64"					 				>> .config.mak
	@echo "export SH4"					 				>> .config.mak
	@echo "export MIPS"					 				>> .config.mak
	@echo "export SPARC"								>> .config.mak
	@echo "export PROF"					 				>> .config.mak
	@echo "export DEMO"					 				>> .config.mak
	@echo "export SDK" 				 					>> .config.mak
	@echo "export BIN" 				 					>> .config.mak
	@echo "export PRE" 				 					>> .config.mak
	@echo "export CFLAG" 			 					>> .config.mak
	@echo "export CCFLAG" 			 					>> .config.mak
	@echo "export CXFLAG" 			 					>> .config.mak
	@echo "export MFLAG" 			 					>> .config.mak
	@echo "export MMFLAG" 			 					>> .config.mak
	@echo "export MXFLAG" 			 					>> .config.mak
	@echo "export LDFLAG" 			 					>> .config.mak
	@echo "export ASFLAG" 			 					>> .config.mak
	@echo "export ARFLAG" 			 					>> .config.mak
	@echo "export SHFLAG" 			 					>> .config.mak
	@echo "export CCACHE" 			 					>> .config.mak
	@echo "export DISTCC" 			 					>> .config.mak

# ######################################################################################
# help
# #

# make help
help : .null
	@cat ./INSTALL

