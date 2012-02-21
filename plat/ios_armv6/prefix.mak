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

CC 				= $(BIN)/llvm-gcc-4.2 -arch armv6 
AR 				= $(BIN)/ar
STRIP 			= $(BIN)/strip
RANLIB 			= $(BIN)/ranlib
LD 				= $(BIN)/llvm-gcc-4.2 -arch armv6 
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
CPPFLAGS 		= -c -Wall -mthumb -miphoneos-version-min=5.0 \
				  -fmessage-length=0  -Wreturn-type -Wunused-variable -Wuninitialized \
				  -pipe -Wno-trigraphs -fpascal-strings \
				  -I$(SDK)/usr/include \
				  -I$(SDK)/usr/include/c++/4.2.1 \
				  -I$(SDK)/usr/lib/gcc/arm-apple-darwin10/4.2.1/include
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


