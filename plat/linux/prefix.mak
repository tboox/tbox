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
CXFLAGS_DEBUG 		= -g 
CXFLAGS 			= -m$(BITS) -c -Wall -mssse3
CXFLAGS-I 			= -I
CXFLAGS-o 			= -o

# arch
ifeq ($(ARCH),x86)
CXFLAGS 			+= -I/usr/include/i386-linux-gnu 
endif

# opti
ifeq ($(SMALL),y)
CXFLAGS_RELEASE 	+= -Os
else
CXFLAGS_RELEASE 	+= -O3
endif

# prof
ifeq ($(PROF),y)
CXFLAGS 			+= -g -pg -fno-omit-frame-pointer 
else
CXFLAGS_RELEASE 	+= -fomit-frame-pointer 
CXFLAGS_DEBUG 		+= -fno-omit-frame-pointer $(CXFLAGS_CHECK)
endif

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
					-Werror=implicit-function-declaration -Werror=missing-prototypes -Werror=return-type
#					-Werror

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	= -fno-rtti
CCFLAGS_DEBUG 		= 
CCFLAGS 			= -D_ISOC99_SOURCE -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_POSIX_C_SOURCE=200112 -D_XOPEN_SOURCE=600

# ldflags
LDFLAGS_RELEASE 	= -static
LDFLAGS_DEBUG 		= -rdynamic 
LDFLAGS 			= -m$(BITS) 
LDFLAGS-L 			= -L
LDFLAGS-l 			= -l
LDFLAGS-o 			= -o

# prof
ifeq ($(PROF),y)
LDFLAGS 			+= -pg 
else
LDFLAGS_RELEASE 	+= -s
LDFLAGS_DEBUG 		+= $(LDFLAGS_CHECK)
endif

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= -m$(BITS) -f elf
ASFLAGS-I 			= -I
ASFLAGS-o 			= -o

# arflags
ARFLAGS 			= -cr

# share ldflags
SHFLAGS 			= -m$(BITS) -shared -Wl,-soname

# include sub-config
include 			$(PLAT_DIR)/config.mak


