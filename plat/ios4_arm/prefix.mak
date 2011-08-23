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
SDK 			= /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.2.sdk 
#SDK 			= /Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS4.3.sdk 
endif

ifeq ($(HOST),mac)
# for mac
CC 				= $(BIN)/gcc-4.2 -arch armv6
AR 				= $(BIN)/ar
STRIP 			= $(BIN)/strip
RANLIB 			= $(BIN)/ranlib
LD 				= $(BIN)/llvm-gcc-4.2 -arch armv6
AS				= 
else
# for linux
PRE 			= $(BIN)/arm-apple-darwin9-
CC 				= $(PRE)gcc
AR 				= $(PRE)ar
STRIP 			= $(PRE)strip
RANLIB 			= $(PRE)ranlib
LD 				= $(PRE)g++
AS				= 
endif

RM 				= rm -f
RMDIR 			= rm -rf
CP 				= cp
CPDIR 			= cp -r
MKDIR 			= mkdir -p
MAKE 			= make
PWD 			= pwd

# cppflags: c/c++ files
CPPFLAGS_RELEASE 	= \
	-O2 -DNDEBUG \
	-fomit-frame-pointer -freg-struct-return -fno-bounds-check 

CPPFLAGS_DEBUG 	= -g
CPPFLAGS 		= -c -Wall \
				  -I$(SDK)/usr/include \
				  -I$(SDK)/usr/include/c++/4.2.1 \
				  -I$(SDK)/usr/lib/gcc/arm-apple-darwin10/4.2.1/include
CPPFLAGS-I 		= -I
CPPFLAGS-o 		= -o

# cflags: c files
CFLAGS_RELEASE 	= 
CFLAGS_DEBUG 	= 
CFLAGS 			=

# cxxflags: c++ files
CXXFLAGS_RELEASE = -fno-rtti
CXXFLAGS_DEBUG 	= 
CXXFLAGS 		= 

# ldflags
LDFLAGS_RELEASE = -s -Wl,-O2,--sort-common,--as-needed
LDFLAGS_DEBUG 	= 
LDFLAGS 		= -arch=armv6 -nostdlib \
				  -L$(SDK)/usr/lib \
				  -L$(SDK)/usr/lib/system \
				  -L$(SDK)/usr/lib/gcc/arm-apple-darwin10/4.2.1
LDFLAGS-L 		= -L
LDFLAGS-l 		= -l
LDFLAGS-o 		= -o

# asflags
ASFLAGS_RELEASE = Wa,-march=native
ASFLAGS_DEBUG 	= 
ASFLAGS 		= -c
ASFLAGS-I 		= -I
ASFLAGS-o 		= -o

# arflags
ARFLAGS 		= -cr

# share ldflags
SHFLAGS 		= -shared -Wl,-soname

# include sub-config
include 		$(PLAT_DIR)/config.mak


