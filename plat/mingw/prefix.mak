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

# prefix
PRE 				:= $(if $(findstring x86,$(ARCH)),$(if $(findstring mac,$(HOST)),i386-mingw32-,i686-w64-mingw32-),$(PRE))
PRE 				:= $(if $(findstring x64,$(ARCH)),x86_64-w64-mingw32-,$(PRE))
PRE_ 				:= $(if $(BIN),$(BIN)/$(PRE),$(PRE))

# cpu bits
BITS 				:= $(if $(findstring x64,$(ARCH)),64,)
BITS 				:= $(if $(findstring x86,$(ARCH)),32,)

# tool
CC 					= $(PRE_)gcc
AR 					= $(PRE_)ar
STRIP 				= $(PRE_)strip
RANLIB 				= $(PRE_)ranlib
LD 					= $(PRE_)g++
AS					= 
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make
PWD 				= pwd

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -freg-struct-return -fvisibility=hidden 
CXFLAGS_DEBUG 		= -g 
CXFLAGS 			= -m$(BITS) -c -Wall -Werror -Wno-error=deprecated-declarations -Qunused-arguments 
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

# sse
ifneq ($(HOST),mac)
CXFLAGS 			+= -mssse3 
endif

# opti
ifeq ($(SMALL),y)
CXFLAGS_RELEASE 	+= -Os
else
CXFLAGS_RELEASE 	+= -O3
endif

# prof
ifeq ($(PROF),y)
CXFLAGS 			+= -g -fno-omit-frame-pointer 
else
CXFLAGS_RELEASE 	+= -fomit-frame-pointer 
CXFLAGS_DEBUG 		+= -fno-omit-frame-pointer 
endif

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				= \
					-std=c99 \
					-D_GNU_SOURCE=1 -D_REENTRANT -fno-math-errno

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	=
CCFLAGS_DEBUG 		= 
CCFLAGS 			= \
					-D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
					-D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600

# ldflags
LDFLAGS_RELEASE 	= $(if $(findstring y,$(PROF)),,-s)
LDFLAGS_DEBUG 		= 
LDFLAGS 			= -m$(BITS) -static
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-o 			= -o

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= -m$(BITS) -f elf $(ARCH_ASFLAGS)
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

# arflags
ARFLAGS 			= -cr

# shflags
SHFLAGS_RELEASE 	= -s
SHFLAGS 			= -shared -Wl,-soname

# include sub-config
include 		$(PLAT_DIR)/config.mak


