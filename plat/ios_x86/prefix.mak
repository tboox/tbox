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
PRE 			= 
CC 				= $(PRE)gcc -arch i386
AR 				= $(PRE)ar
STRIP 			= $(PRE)strip
RANLIB 			= $(PRE)ranlib
LD 				= $(PRE)g++ -arch i386
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
LDFLAGS_RELEASE = 
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


