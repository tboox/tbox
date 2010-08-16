# sub-makefile
include $(PRO_DIR)$(_)prefix.mak

# module name
NAMES 						= string
NAMES-$(TB_CONFIG_ENCODING) 	+= encoding
NAMES-$(TB_CONFIG_REGEX) 		+= regex

# module type
regex_TYPE 			= BIN
encoding_TYPE 		= BIN
string_TYPE 		= BIN

# c-source files
regex_C_FILES 		= regex
encoding_C_FILES 	= encoding
string_C_FILES 		= string

# lib files
regex_LIBS 			= tbox tplat z m dl pthread
regex_LIB_DIR 		= ../../ /usr/lib/ 
regex_INC_DIR 		= /usr/include 

encoding_LIBS 		= tbox tplat z m dl pthread
encoding_LIB_DIR 	= ../../ /usr/lib/
encoding_INC_DIR 	= /usr/include

string_LIBS 		= tbox tplat z m dl pthread
string_LIB_DIR 		= ../../ /usr/lib/
string_INC_DIR 		= /usr/include

include $(PRO_DIR)$(_)suffix.mak

