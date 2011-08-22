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

# tool
PRE 			= $(BIN)/arm-linux-androideabi-
CC 				= $(PRE)gcc
AR 				= $(PRE)ar
STRIP 			= $(PRE)strip
RANLIB 			= $(PRE)ranlib
LD 				= $(PRE)g++
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
	-O2 -DNDEBUG \
	-fomit-frame-pointer -freg-struct-return -fno-bounds-check 

CPPFLAGS_DEBUG 	= -g
CPPFLAGS 		= \
				-c -Wall \
				-I$(SDK)/platforms/android-4/arch-arm/usr/include 
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
LDFLAGS 		= -nostdlib \
				  -L$(SDK)/platforms/android-3/arch-arm/usr/lib 
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
SHFLAGS 		= \
				-shared -fPIC\
				-L$(SDK)/platforms/android-3/arch-arm/usr/lib \
				-lsupc++ -lgcc -lc -lm

# include sub-config
include 		$(PLAT_DIR)/config.mak


