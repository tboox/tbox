# architecture makefile configure

# prefix & suffix
BIN_PREFIX 			= 
BIN_SUFFIX 			= .b

OBJ_PREFIX 			= 
OBJ_SUFFIX 			= .o

LIB_PREFIX 			= lib
LIB_SUFFIX 			= .a

DLL_PREFIX 			= 
DLL_SUFFIX 			= .dylib

ASM_SUFFIX 			= .S

# cpu bits
BITS 				:= $(if $(findstring x64,$(ARCH)),64,)
BITS 				:= $(if $(findstring x86,$(ARCH)),32,)
BITS 				:= $(if $(BITS),$(BITS),$(shell getconf LONG_BIT))

# tool
PRE 				= xcrun -sdk macosx 
CC 					= $(PRE)clang
MM 					= $(PRE)clang
LD 					= $(PRE)clang
AR 					= $(PRE)ar
STRIP 				= $(PRE)strip
RANLIB 				= $(PRE)ranlib
AS					= yasm
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make
PWD 				= pwd

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -fvisibility=hidden
CXFLAGS_DEBUG 		= -g  
CXFLAGS 			= -m$(BITS) -c -Wall -Werror -Wno-error=deprecated-declarations -Qunused-arguments -mssse3
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

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
CXFLAGS_DEBUG 		+= -fno-omit-frame-pointer -ftrapv
endif

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				= \
					-std=c99 \
					-D_GNU_SOURCE=1 -D_REENTRANT \
					-fno-math-errno -fno-tree-vectorize 

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	= 
CCFLAGS_DEBUG 		= 
CCFLAGS 			= \
					-D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
					-D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600

# mxflags: .m/.mm files
MXFLAGS_RELEASE 	= -fvisibility=hidden
MXFLAGS_DEBUG 		= -g  
MXFLAGS 			= \
					-m$(BITS) -c -Wall -Werror -Wno-error=deprecated-declarations -Qunused-arguments \
					-mssse3 $(ARCH_CXFLAGS) -fmessage-length=0 -pipe -fpascal-strings \
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

# prof
ifeq ($(PROF),y)
MXFLAGS 			+= -g -fno-omit-frame-pointer 
else
MXFLAGS_RELEASE 	+= -fomit-frame-pointer 
MXFLAGS_DEBUG 		+= -fno-omit-frame-pointer -ftrapv
endif

# mflags: .m files
MFLAGS_RELEASE 		= 
MFLAGS_DEBUG 		= 
MFLAGS 				= -std=c99

# mmflags: .mm files
MMFLAGS_RELEASE 	= 
MMFLAGS_DEBUG 		=	 
MMFLAGS 			=

# ldflags
LDFLAGS_RELEASE 	= 
LDFLAGS_DEBUG 		= -rdynamic 
LDFLAGS 			= -m$(BITS)
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-o 			= -o

# prof
ifeq ($(PROF),y)
else
LDFLAGS_RELEASE 	+= -s
LDFLAGS_DEBUG 		+= -ftrapv
endif

# asflags
ASFLAGS_RELEASE 	= Wa,-march=native
ASFLAGS_DEBUG 		= 
ASFLAGS 			= -m$(BITS) -f elf $(ARCH_ASFLAGS)
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

# arflags
ARFLAGS 			= -cr

# share ldflags
SHFLAGS 			= $(ARCH_LDFLAGS) -dynamiclib

# config
include 			$(PLAT_DIR)/config.mak


