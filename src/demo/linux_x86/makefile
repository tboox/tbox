# sub-makefile
include $(PRO_DIR)$(_)prefix.mak

# module name
NAMES 							= string memops stream http time math conv
NAMES-$(TB_CONFIG_ENCODING) 	+= encoding
NAMES-$(TB_CONFIG_REGEX) 		+= regex
NAMES-$(TB_CONFIG_FORMAT) 		+= format
NAMES-$(TB_CONFIG_XML) 			+= xml

# module type
regex_TYPE 			= BIN
encoding_TYPE 		= BIN
string_TYPE 		= BIN
memops_TYPE 		= BIN
format_TYPE 		= BIN
stream_TYPE 		= BIN
xml_TYPE 			= BIN
http_TYPE 			= BIN
time_TYPE 			= BIN
math_TYPE 			= BIN
conv_TYPE 			= BIN

# c-source files
regex_C_FILES 		= regex
encoding_C_FILES 	= encoding
string_C_FILES 		= string
memops_C_FILES 		= memops
format_C_FILES 		= format
stream_C_FILES 		= stream
xml_C_FILES 		= xml
http_C_FILES 		= http
time_C_FILES 		= time
math_C_FILES 		= math
conv_C_FILES 		= conv

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

memops_LIBS 		= tbox tplat z m dl pthread
memops_LIB_DIR 		= ../../ /usr/lib/
memops_INC_DIR 		= /usr/include

format_LIBS 		= tbox tplat z m dl pthread
format_LIB_DIR 		= ../../ /usr/lib/
format_INC_DIR 		= /usr/include

stream_LIBS 		= tbox tplat z m dl pthread
stream_LIB_DIR 		= ../../ /usr/lib/
stream_INC_DIR 		= /usr/include

xml_LIBS 			= tbox tplat z m dl pthread
xml_LIB_DIR 		= ../../ /usr/lib/
xml_INC_DIR 		= /usr/include

http_LIBS 			= tbox tplat z m dl pthread
http_LIB_DIR 		= ../../ /usr/lib/
http_INC_DIR 		= /usr/include

time_LIBS 			= tbox tplat z m dl pthread
time_LIB_DIR 		= ../../ /usr/lib/
time_INC_DIR 		= /usr/include

math_LIBS 			= tbox tplat z m dl pthread
math_LIB_DIR 		= ../../ /usr/lib/
math_INC_DIR 		= /usr/include

conv_LIBS 			= tbox tplat z m dl pthread
conv_LIB_DIR 		= ../../ /usr/lib/
conv_INC_DIR 		= /usr/include

include $(PRO_DIR)$(_)suffix.mak

