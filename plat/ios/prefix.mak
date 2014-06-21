# architecture makefile configure

# prefix & suffix
BIN_PREFIX 			= 
BIN_SUFFIX 			= .b

OBJ_PREFIX 			= 
OBJ_SUFFIX 			= .o
	
LIB_PREFIX 			= lib
LIB_SUFFIX 			= .a
	
DLL_PREFIX 			= lib
DLL_SUFFIX 			= .dylib

ASM_SUFFIX 			= .S

# prefix
PRE_ 				:= $(if $(BIN),$(BIN)/$(PRE),xcrun -sdk iphoneos )

# toolchain
CC 					= $(PRE_)clang
MM 					= $(PRE_)clang
AR 					= $(PRE_)ar
STRIP 				= $(PRE_)strip
RANLIB 				= $(PRE_)ranlib
LD 					= $(PRE_)clang
AS					= $(PLAT_DIR)/gas-preprocessor.pl $(PRE_)clang
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make -r
PWD 				= pwd

# cpu flags
ifeq ($(ARCH),armv6)
CPU_CXFLAGS 		= -mcpu=arm1176jzf-s
endif

ifeq ($(ARCH),armv7)
CPU_CXFLAGS 		= -mcpu=cortex-a8
endif

ifeq ($(ARCH),armv7s)
CPU_CXFLAGS 		= -mcpu=cortex-a8
endif

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -fomit-frame-pointer -fvisibility=hidden
CXFLAGS_DEBUG 		= -g -D__tb_debug__
CXFLAGS 			= \
					-arch $(ARCH) -c -Wall -mthumb $(CPU_CXFLAGS) \
					-Werror -Wno-error=deprecated-declarations -Qunused-arguments \
					-fmessage-length=0 -pipe -fpascal-strings
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

# opti
ifeq ($(SMALL),y)
CXFLAGS_RELEASE 	+= -Os
else
CXFLAGS_RELEASE 	+= -O3
endif

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				= -std=gnu99

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	= 
CCFLAGS_DEBUG 		= 
CCFLAGS 			= 

# mxflags: .m/.mm files
MXFLAGS_RELEASE 	= -fomit-frame-pointer -fvisibility=hidden
MXFLAGS_DEBUG 		= -g -D__tb_debug__
MXFLAGS 			= \
					-arch $(ARCH) -c -Wall -mthumb $(CPU_CXFLAGS) \
					-Werror -Wno-error=deprecated-declarations -Qunused-arguments \
					-fmessage-length=0 -pipe -fpascal-strings \
					"-DIBOutlet=__attribute__((iboutlet))" \
					"-DIBOutletCollection(ClassName)=__attribute__((iboutletcollection(ClassName)))" \
					"-DIBAction=void)__attribute__((ibaction)"
MXFLAGS-I 			= -I
MXFLAGS-o 			= -o

# opti
ifeq ($(SMALL),y)
MXFLAGS_RELEASE 	+= -Os
else
MXFLAGS_RELEASE 	+= -O3
endif

# small
MXFLAGS-$(SMALL) 	+= 

# mflags: .m files
MFLAGS_RELEASE 		= 
MFLAGS_DEBUG 		= 
MFLAGS 				= -std=gnu99

# mmflags: .mm files
MMFLAGS_RELEASE 	= 
MMFLAGS_DEBUG 		=	 
MMFLAGS 			=

# ldflags
LDFLAGS_RELEASE 	= -s
LDFLAGS_DEBUG 		= 
LDFLAGS 			= -arch $(ARCH) -framework Foundation
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-f 			=
LDFLAGS-o 			= -o

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= -arch $(ARCH) -c -fPIC
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

# arflags
ARFLAGS_RELEASE 	= 
ARFLAGS_DEBUG 		= 
ARFLAGS 			= -cr
ARFLAGS-o 			= 

# shflags
SHFLAGS_RELEASE 	= -s
SHFLAGS_DEBUG 		= 
SHFLAGS 			= -arch $(ARCH) -dynamiclib -Wl,-single_module

# config
include 			$(PLAT_DIR)/config.mak


