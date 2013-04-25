# architecture makefile configure

# prefix & suffix
BIN_PREFIX 			= 
BIN_SUFFIX 			= .b

OBJ_PREFIX 			= 
OBJ_SUFFIX 			= .o
	
LIB_PREFIX 			= lib
LIB_SUFFIX 			= .a
	
DLL_PREFIX 			= lib
DLL_SUFFIX 			= .so

ASM_SUFFIX 			= .S

# toolchain
PRE 				= xcrun -sdk iphoneos 
CC 					= $(PRE)gcc 
MM 					= $(PRE)gcc
AR 					= $(PRE)ar
STRIP 				= $(PRE)strip
RANLIB 				= $(PRE)ranlib
LD 					= $(PRE)gcc
AS					= $(PLAT_DIR)/gas-preprocessor.pl $(PRE)gcc
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make
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
CXFLAGS_RELEASE 	= -fomit-frame-pointer -freg-struct-return -fno-bounds-check -fvisibility=hidden
CXFLAGS_DEBUG 		= -g -D__tb_debug__
CXFLAGS 			= -arch $(ARCH) -D__tb_arch_$(ARCH)__ -c -Wall  \
					-mthumb $(CPU_CXFLAGS) -miphoneos-version-min=$(SDK) \
					-fmessage-length=0  -Wreturn-type -Wunused-variable \
					-pipe -Wno-trigraphs -fpascal-strings \
					--sysroot=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS$(SDK).sdk
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

# opti
ifeq ($(SMALL),y)
CXFLAGS_RELEASE 	+= -Os
else
CXFLAGS_RELEASE 	+= -O3
endif

# small
CXFLAGS-$(SMALL) 	+= -D__tb_small__

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				= -std=gnu99

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	= 
CCFLAGS_DEBUG 		= 
CCFLAGS 			= 

# mxflags: .m/.mm files
MXFLAGS_RELEASE 	= \
					-O3 -DNDEBUG \
					-fomit-frame-pointer -freg-struct-return -fno-bounds-check \
					-fvisibility=hidden

MXFLAGS_DEBUG 		= -g -DDEBUG=1
MXFLAGS 			= -arch $(ARCH) -D__tb_arch_$(ARCH)__  -c -Wall  \
					-mthumb $(CPU_CXFLAGS) -miphoneos-version-min=$(SDK) \
					-fmessage-length=0  -Wreturn-type -Wunused-variable \
					-pipe -Wno-trigraphs -fpascal-strings \
					"-DIBOutlet=__attribute__((iboutlet))" \
					"-DIBOutletCollection(ClassName)=__attribute__((iboutletcollection(ClassName)))" \
					"-DIBAction=void)__attribute__((ibaction)" \
					--sysroot=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS$(SDK).sdk
MXFLAGS-I 			= -I
MXFLAGS-o 			= -o

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
LDFLAGS 			= -arch $(ARCH) \
					-isysroot /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS$(SDK).sdk \
					--sysroot=/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS$(SDK).sdk
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-o 			= -o

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= -arch $(ARCH) -c -fPIC
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

# arflags
ARFLAGS 			= -cr

# share ldflags
SHFLAGS 			= -dynamiclib -Wl,-single_module

# config
include 			$(PLAT_DIR)/config.mak


