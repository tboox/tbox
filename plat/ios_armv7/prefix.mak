# architecture makefile configure

# prefix & suffix
BIN_PREFIX 		= 
BIN_SUFFIX 		= .b

OBJ_PREFIX 		= 
OBJ_SUFFIX 		= .o

LIB_PREFIX 		= lib
LIB_SUFFIX 		= .a

DLL_PREFIX 		= lib
DLL_SUFFIX 		= .so

ASM_SUFFIX 		= .S

ifeq ($(BIN),)
BIN 			= /Developer/Platforms/iPhoneOS.platform/Developer/usr/bin
endif

ifeq ($(SDK),)
SDK 			= /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS5.0.sdk
endif

CC 				= $(BIN)/gcc 
AR 				= $(BIN)/ar
STRIP 			= $(BIN)/strip
RANLIB 			= $(BIN)/ranlib
LD 				= $(BIN)/gcc
AS				= 
RM 				= rm -f
RMDIR 			= rm -rf
CP 				= cp
CPDIR 			= cp -r
MKDIR 			= mkdir -p
MAKE 			= make
PWD 			= pwd

# cppflags: c/c++ files
CPPFLAGS_RELEASE 	= \
	-Os -DNDEBUG \
	-fomit-frame-pointer -freg-struct-return -fno-bounds-check \
	-fvisibility=hidden

CPPFLAGS_DEBUG 	= -DDEBUG=1 -gdwarf-2 
CPPFLAGS 		= -arch armv7 -c -Wall -mthumb -miphoneos-version-min=5.0 \
				  -fmessage-length=0  -Wreturn-type -Wunused-variable \
				  -pipe -Wno-trigraphs -fpascal-strings \
				  --sysroot=$(SDK)
CPPFLAGS-I 		= -I
CPPFLAGS-o 		= -o

# cflags: c files
CFLAGS_RELEASE 	= 
CFLAGS_DEBUG 	= 
CFLAGS 			= -std=gnu99

# cxxflags: c++ files
CXXFLAGS_RELEASE = -fno-rtti
CXXFLAGS_DEBUG 	= 
CXXFLAGS 		= 

# ldflags
LDFLAGS_RELEASE =
LDFLAGS_DEBUG 	= 
LDFLAGS 		= -arch armv7 -isysroot $(SDK) --sysroot=$(SDK)
LDFLAGS-L 		= -L
LDFLAGS-l 		= -l
LDFLAGS-o 		= -o

# asflags
ASFLAGS_RELEASE = 
ASFLAGS_DEBUG 	= 
ASFLAGS 		= -c
ASFLAGS-I 		= -I
ASFLAGS-o 		= -o

# arflags
ARFLAGS 		= -cr

# share ldflags
SHFLAGS 		= -dynamiclib -Wl,-single_module

# include sub-config
include 		$(PLAT_DIR)/config.mak


