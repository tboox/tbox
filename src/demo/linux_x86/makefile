# sub-makefile
include $(PRO_DIR)$(_)prefix.mak

# module name
NAMES 							= string cstring memops http time math conv
NAMES-$(TB_CONFIG_ENCODING) 	+= encoding
NAMES-$(TB_CONFIG_REGEX) 		+= regex
NAMES-$(TB_CONFIG_FORMAT) 		+= format
NAMES-$(TB_CONFIG_XML) 			+= xml
NAMES-$(TB_CONFIG_STREAM) 		+= gstream zstream

ifeq ($(TB_CONFIG_ENCODING),y)
NAMES-$(TB_CONFIG_STREAM) 		+= estream
endif

# module type
regex_TYPE 			= BIN
encoding_TYPE 		= BIN
string_TYPE 		= BIN
cstring_TYPE 		= BIN
memops_TYPE 		= BIN
format_TYPE 		= BIN
xml_TYPE 			= BIN
http_TYPE 			= BIN
time_TYPE 			= BIN
math_TYPE 			= BIN
conv_TYPE 			= BIN
gstream_TYPE 		= BIN
zstream_TYPE 		= BIN
estream_TYPE 		= BIN

# c-source files
regex_C_FILES 		= regex
encoding_C_FILES 	= encoding
string_C_FILES 		= string
cstring_C_FILES 	= cstring
memops_C_FILES 		= memops
format_C_FILES 		= format
xml_C_FILES 		= xml
http_C_FILES 		= http
time_C_FILES 		= time
math_C_FILES 		= math
conv_C_FILES 		= conv
gstream_C_FILES 	= gstream
estream_C_FILES 	= estream
zstream_C_FILES 	= zstream

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

cstring_LIBS 		= tbox tplat z m dl pthread
cstring_LIB_DIR 	= ../../ /usr/lib/
cstring_INC_DIR 	= /usr/include

memops_LIBS 		= tbox tplat z m dl pthread
memops_LIB_DIR 		= ../../ /usr/lib/
memops_INC_DIR 		= /usr/include

format_LIBS 		= tbox tplat z m dl pthread
format_LIB_DIR 		= ../../ /usr/lib/
format_INC_DIR 		= /usr/include

gstream_LIBS 		= tbox tplat z m dl pthread
gstream_LIB_DIR 	= ../../ /usr/lib/
gstream_INC_DIR 	= /usr/include

zstream_LIBS 		= tbox tplat z m dl pthread
zstream_LIB_DIR 	= ../../ /usr/lib/
zstream_INC_DIR 	= /usr/include

estream_LIBS 		= tbox tplat z m dl pthread
estream_LIB_DIR 	= ../../ /usr/lib/
estream_INC_DIR 	= /usr/include

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

