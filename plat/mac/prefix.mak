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

# prefix
PRE_ 				:= $(if $(BIN),$(BIN)/$(PRE),)

# cc
CC_ 				:= ${shell if [ -f "/usr/bin/clang" ]; then echo "clang"; elif [ -f "/usr/local/bin/clang" ]; then echo "clang"; else echo "gcc"; fi }
CC_ 				:= $(if $(findstring y,$(PROF)),gcc,$(CC_))
CC 					= $(PRE_)$(CC_)
ifeq ($(CXFLAGS_CHECK),)
CC_CHECK 			= ${shell if $(CC) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi }
CXFLAGS_CHECK 		:= $(call CC_CHECK,-ftrapv,) $(call CC_CHECK,-fsanitize=address,) #-fsanitize=thread
export CXFLAGS_CHECK
endif

# ld
LD_ 				:= ${shell if [ -f "/usr/bin/clang++" ]; then echo "clang++"; elif [ -f "/usr/local/bin/clang++" ]; then echo "clang++"; else echo "g++"; fi }
LD_ 				:= $(if $(findstring y,$(PROF)),g++,$(LD_))
LD 					= $(PRE_)$(LD_)
ifeq ($(LDFLAGS_CHECK),)
LD_CHECK 			= ${shell if $(LD) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi }
LDFLAGS_CHECK 		:= $(call LD_CHECK,-ftrapv,) $(call LD_CHECK,-fsanitize=address,) #-fsanitize=thread
export LDFLAGS_CHECK
endif

# cpu bits
BITS 				:= $(if $(findstring x64,$(ARCH)),64,)
BITS 				:= $(if $(findstring x86,$(ARCH)),32,)
BITS 				:= $(if $(BITS),$(BITS),$(shell getconf LONG_BIT))

# tool
MM 					= $(PRE_)$(CC_)
AR 					= $(PRE_)ar
STRIP 				= $(PRE_)strip
RANLIB 				= $(PRE_)ranlib
AS					= yasm
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
CXFLAGS 			= -m$(BITS) -c -Wall -mssse3
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
ifeq ($(ARCH),x64)
CXFLAGS 			+= -pg
endif
else
CXFLAGS_RELEASE 	+= -fomit-frame-pointer 
CXFLAGS_DEBUG 		+= -fno-omit-frame-pointer $(CXFLAGS_CHECK)
endif

# small
CXFLAGS-$(SMALL) 	+= -D__tb_small__

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				= \
					-std=c99 \
					-D_GNU_SOURCE=1 -D_REENTRANT \
					-fno-math-errno \
					-Wno-parentheses -Wstrict-prototypes \
					-Wno-switch -Wno-format-zero-length -Wdisabled-optimization \
					-Wpointer-arith -Wwrite-strings \
					-Wundef -Wmissing-prototypes  \
					-fno-tree-vectorize \
					-Werror=unused-variable -Wno-pointer-sign -Wno-pointer-to-int-cast \
					-Werror=implicit-function-declaration -Werror=missing-prototypes 

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	= 
CCFLAGS_DEBUG 		= 
CCFLAGS 			= \
					-D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
					-D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600

# mxflags: .m/.mm files
MXFLAGS_RELEASE 	= -freg-struct-return -fno-bounds-check -fvisibility=hidden
MXFLAGS_DEBUG 		= -g -D__tb_debug__ 
MXFLAGS 			= -m$(BITS) -c -Wall -mssse3 $(ARCH_CXFLAGS) \
					-fmessage-length=0  -Wreturn-type -Wunused-variable \
					-pipe -Wno-trigraphs -fpascal-strings \
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
ifeq ($(ARCH),x64)
MXFLAGS 			+= -pg
endif
else
MXFLAGS_RELEASE 	+= -fomit-frame-pointer 
MXFLAGS_DEBUG 		+= -fno-omit-frame-pointer $(CXFLAGS_CHECK)
endif

# small
MXFLAGS-$(SMALL) 	+= -D__tb_small__

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
ifeq ($(ARCH),x64)
LDFLAGS 			+= -pg
endif
else
LDFLAGS_RELEASE 	+= -s
LDFLAGS_DEBUG 		+= $(LDFLAGS_CHECK)
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


