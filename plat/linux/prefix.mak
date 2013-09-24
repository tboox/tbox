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
ifneq ($(BIN),)
PRE_ 				:= $(BIN)/$(PRE)
else
PRE_ 				:=
endif

# cc
CC_ 				:= ${shell if [ -f "/usr/bin/clang" ]; then echo "clang"; elif [ -f "/usr/local/bin/clang" ]; then echo "clang"; else echo "gcc"; fi }
CC_CHECK 			= ${shell if $(CC_) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi }

# ld
LD_ 				:= ${shell if [ -f "/usr/bin/clang++" ]; then echo "clang++"; elif [ -f "/usr/local/bin/clang++" ]; then echo "clang++"; else echo "g++"; fi }
LD_CHECK 			= ${shell if $(LD_) $(1) -S -o /dev/null -xc /dev/null > /dev/null 2>&1; then echo "$(1)"; else echo "$(2)"; fi }

# tool
CC 					= $(PRE_)$(CC_)
AR 					= $(PRE_)ar
STRIP 				= $(PRE_)strip
RANLIB 				= $(PRE_)ranlib
LD 					= $(PRE_)$(LD_)
AS					= yasm
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make
PWD 				= pwd

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -fomit-frame-pointer -freg-struct-return -fno-bounds-check -fvisibility=hidden
CXFLAGS_DEBUG 		= -g -pg -D__tb_debug__ -fno-omit-frame-pointer $(call CC_CHECK,-ftrapv,) $(call CC_CHECK,-fsanitize=address,) #-fsanitize=thread
CXFLAGS 			= -c -Wall -D__tb_arch_$(ARCH)__
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

# arch
ifeq ($(ARCH),x86)
CXFLAGS 			+= -m32 -mssse3 -I/usr/include/i386-linux-gnu 
endif

ifeq ($(ARCH),x64)
CXFLAGS 			+= -m64 -mssse3
endif

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
					-fno-math-errno \
					-Wno-parentheses -Wstrict-prototypes -Wno-unused-function \
					-Wno-switch -Wno-format-zero-length -Wdisabled-optimization \
					-Wpointer-arith -Wwrite-strings \
					-Wundef -Wmissing-prototypes  \
					-fno-signed-zeros -fno-tree-vectorize \
					-Werror=unused-variable -Wtype-limits -Wno-pointer-sign -Wno-pointer-to-int-cast \
					-Werror=implicit-function-declaration -Werror=missing-prototypes 
#					-Werror

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	= -fno-rtti
CCFLAGS_DEBUG 		= 
CCFLAGS 			= \
					-D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE \
					-D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600

# ldflags
LDFLAGS_RELEASE 	= -static -s
LDFLAGS_DEBUG 		= -rdynamic -pg $(call LD_CHECK,-ftrapv,) $(call LD_CHECK,-fsanitize=address,) #-fsanitize=thread
LDFLAGS 			=  
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-o 			= -o

ifeq ($(ARCH),x86)
LDFLAGS 			+= -m32 
endif

ifeq ($(ARCH),x64)
LDFLAGS 			+= -m64
endif

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= -f elf
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

ifeq ($(ARCH),x64)
ASFLAGS 			+= -m32
endif

ifeq ($(ARCH),x64)
ASFLAGS 			+= -m64
endif

# arflags
ARFLAGS 			= -cr

# share ldflags
SHFLAGS 			= -shared -Wl,-soname

# include sub-config
include 			$(PLAT_DIR)/config.mak


