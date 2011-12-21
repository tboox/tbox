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
PRE 			= wine $(BIN)/
CC 				= $(PRE)cl.exe 
#CC 				= $(PRE)icl.exe 
AR 				= 
STRIP 			= 
RANLIB 			= 
LD 				= $(PRE)link.exe
#LD 				= $(PRE)xilink.exe
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
	-O2 -DNDEBUG

CPPFLAGS_DEBUG 	= -g
CPPFLAGS 		= -c -X -nologo -W3 -EHsc -DWIN32 -D_CONSOLE \
				  -I$(SDK)/include
CPPFLAGS-I 		= -I
CPPFLAGS-o 		= -o

# cflags: c files
CFLAGS_RELEASE 	= 
CFLAGS_DEBUG 	= 
CFLAGS 			= 

# cxxflags: c++ files
CXXFLAGS_RELEASE = 
CXXFLAGS_DEBUG 	= 
CXXFLAGS 		= 

# ldflags
LDFLAGS_RELEASE = 
LDFLAGS_DEBUG 	= 
LDFLAGS 		= \
				  -libpath:$(SDK)/lib \
				  kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib \
				  shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \
				  -nologo -subsystem:console -incremental:no -machine:I386
LDFLAGS-L 		= -libpath:
LDFLAGS-l 		= -l
LDFLAGS-o 		= -o

# asflags
ASFLAGS_RELEASE = 
ASFLAGS_DEBUG 	= 
ASFLAGS 		= -c
ASFLAGS-I 		= -I
ASFLAGS-o 		= -o

# arflags
ARFLAGS 		= 

# share ldflags
SHFLAGS 		= 

# include sub-config
include 		$(PLAT_DIR)/config.mak


