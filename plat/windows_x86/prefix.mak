# architecture makefile configure

# prefix & suffix
BIN_PREFIX 		= 
BIN_SUFFIX 		= .exe

OBJ_PREFIX 		= 
OBJ_SUFFIX 		= .obj

LIB_PREFIX 		= 
LIB_SUFFIX 		= .lib

DLL_PREFIX 		= 
DLL_SUFFIX 		= .dll

ASM_SUFFIX 		= .S

# tool
PRE 			= i686-w64-mingw32-
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
	-fomit-frame-pointer -freg-struct-return -fno-bounds-check \
	-march=native -pipe # gcc >= 4.2.3

CPPFLAGS_DEBUG 	= -g
CPPFLAGS 		= -c -Wall -msse2
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
LDFLAGS 		= 
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


