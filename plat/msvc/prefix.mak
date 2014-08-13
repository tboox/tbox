# architecture makefile configure

# prefix & suffix
BIN_PREFIX 			= 
BIN_SUFFIX 			= .exe

OBJ_PREFIX 			= 
OBJ_SUFFIX 			= .obj

LIB_PREFIX 			= 
LIB_SUFFIX 			= .lib

DLL_PREFIX 			= 
DLL_SUFFIX 			= .dll

ASM_SUFFIX 			= .S

# tool
CC 					= cl.exe
AR 					= link.exe
STRIP 				= 
RANLIB 				= 
LD 					= link.exe
AS					= 
RM 					= rm -f
RMDIR 				= rm -rf
CP 					= cp
CPDIR 				= cp -r
MKDIR 				= mkdir -p
MAKE 				= make -r
PWD 				= pwd

# pdb
ifeq ($(PDB),)
PDB 				:= $(subst \,\\,$(HOME)$(PRO_NAME)$(DTYPE).pdb)
export PDB
endif

# cxflags: .c/.cc/.cpp files
CXFLAGS_RELEASE 	= -MD -GL -Gy -Zi
CXFLAGS_DEBUG 		= -Od -GS -MDd -ZI -D__tb_debug__
CXFLAGS 			= \
					-Fd"$(PDB)" -SSE2 \
					-D_MBCS -D_CRT_SECURE_NO_WARNINGS -DNOCRYPT -DNOGDI -Gf -Gd -Gm -W3 -WX -nologo -c -TP \
					-I'/usr/local/inc'
CXFLAGS-I 			= -I
CXFLAGS-o 			= -Fo

# opti
ifeq ($(SMALL),y)
CXFLAGS_RELEASE 	+= -Os
else
CXFLAGS_RELEASE 	+= -Ox
endif

# cflags: .c files
CFLAGS_RELEASE 		= 
CFLAGS_DEBUG 		= 
CFLAGS 				=

# ccflags: .cc/.cpp files
CCFLAGS_RELEASE 	=
CCFLAGS_DEBUG 		= 
CCFLAGS 			= 

# ldflags
LDFLAGS_RELEASE 	=
LDFLAGS_DEBUG 		= -debug
LDFLAGS 			= \
 					-pdb:"$(PDB)" \
					-manifest -manifestuac:"level='asInvoker' uiAccess='false'" \
					-nologo -machine:x86 -dynamicbase -nxcompat -libpath:'$(HOME)lib\msvc\x86' -libpath:'$(HOME)tool\msys\local\lib'
LDFLAGS-L 			= -libpath:
LDFLAGS-l 			= 
LDFLAGS-f 			= .lib
LDFLAGS-o 			= -out:

# asflags
ASFLAGS_RELEASE 	= 
ASFLAGS_DEBUG 		= 
ASFLAGS 			= 
ASFLAGS-I 			=
ASFLAGS-o 			= 

# arflags
ARFLAGS_RELEASE 	= 
ARFLAGS_DEBUG 		= -debug 
ARFLAGS 			= -lib -nologo -machine:x86 -pdb:"$(PDB)" -libpath:'$(HOME)lib\msvc\x86' -libpath:'$(HOME)tool\msys\local\lib'
ARFLAGS-o 			= -out:

# shflags
SHFLAGS_RELEASE 	= 
SHFLAGS_DEBUG 		= -debug 
SHFLAGS 			= -dll -nologo -machine:x86 -pdb:"$(PDB)" -libpath:'$(HOME)lib\msvc\x86' -libpath:'$(HOME)tool\msys\local\lib'

# include sub-config
include 		$(PLAT_DIR)/config.mak


