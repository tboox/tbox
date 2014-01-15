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

# tool
PRE 				= $(BIN)/arm-linux-androideabi-
CC 					= $(PRE)gcc
AR 					= $(PRE)ar
STRIP 				= $(PRE)strip
RANLIB 				= $(PRE)ranlib
LD 					= $(PRE)g++
AS					= 
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make
PWD 				= pwd

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -freg-struct-return -fno-bounds-check -fvisibility=hidden
CXFLAGS_DEBUG 		= -g -D__tb_debug__
CXFLAGS 			= -c -Wall -fomit-frame-pointer -march=$(ARCH) \
					  -I$(NDK)/platforms/android-8/arch-arm/usr/include 
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
CFLAGS 				= \
					-std=c99 \
					-D_GNU_SOURCE=1 -D_REENTRANT \
					-Wno-parentheses \
					-Wno-switch -Wno-format-zero-length -Wdisabled-optimization \
					-Wpointer-arith -Wredundant-decls -Wno-pointer-sign -Wwrite-strings \
					-Wtype-limits -Wundef -Wmissing-prototypes -Wno-pointer-to-int-cast \
					-Wstrict-prototypes -fno-math-errno -fno-signed-zeros -fno-tree-vectorize \
					-Werror=implicit-function-declaration -Werror=missing-prototypes 

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	=
CCFLAGS_DEBUG 		= 
CCFLAGS 			= \
					-D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
					-D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600 \
					-I$(NDK)/sources/cxx-stl/stlport/stlport

# ldflags
LDFLAGS_RELEASE 	= -s
LDFLAGS_DEBUG 		= 
LDFLAGS 			= -nostdlib \
					-L$(NDK)/platforms/android-8/arch-arm/usr/lib/ \
					$(NDK)/platforms/android-8/arch-arm/usr/lib/crtbegin_dynamic.o \
					$(NDK)/platforms/android-8/arch-arm/usr/lib/crtend_android.o
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-o 			= -o

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= 
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

# arflags
ARFLAGS 			= -cr

# share ldflags
SHFLAGS 			= 

# include sub-config
include 			$(PLAT_DIR)/config.mak


