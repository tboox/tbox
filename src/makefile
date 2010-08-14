# sub-makefile
include $(PRO_DIR)$(_)prefix.mak

# module name
NAMES 				= tbox

# module type
tbox_TYPE 			= LIB

# basic files
tbox_C_FILES 	+= \
					bits \
					bswap \
					malloc \
					math \
					pool \
					string

# encoding files
tbox_C_FILES-$(CONFIG_ENCODING) += \
								encoding \
								encoding$(_)unicode_gb2312 \
								encoding$(_)gb2312_unicode

# zlib files
zlib_C_FILES 		= adler32 \
					crc32 \
					infback \
					inffast \
					inflate \
					inftrees \
					uncompr \
					zutil

zlib_C_FILES 					:= $(addprefix external$(_)zlib$(_),$(zlib_C_FILES))
tbox_C_FILES-$(CONFIG_ZLIB) 	+= zlib $(zlib_C_FILES)

# jpeg files
jpeg_C_FILES 		= jaricom \
					jcomapi \
					jdapimin \
					jdapistd \
					jdarith \
					jdatadst \
					jdatasrc \
					jdcoefct \
					jdcolor \
					jddctmgr \
					jdhuff \
					jdinput \
					jdmainct \
					jdmarker \
					jdmaster \
					jdmerge \
					jdpostct \
					jdsample \
					jdtrans \
					jerror \
					jfdctflt \
					jfdctfst \
					jfdctint \
					jidctflt \
					jidctfst \
					jidctint \
					jquant1 \
					jquant2 \
					jutils \
					jmemmgr \
					jmemansi

jpeg_C_FILES 					:= $(addprefix external$(_)jpeg$(_),$(jpeg_C_FILES))
tbox_C_FILES-$(CONFIG_JPEG) 	+= $(jpeg_C_FILES)

# header files
INC_FILES 			= \
					prefix.h \
					config.h \
					type.h \
					option.h \
					bits.h \
					bswap.h \
					malloc.h \
					math.h \
					pool.h \
					string.h
	
# regex
pcre_C_FILES 	= \
				pcre_compile \
				pcre_config \
				pcre_dfa_exec \
				pcre_exec \
				pcre_fullinfo \
				pcre_get \
				pcre_info \
				pcre_maketables \
				pcre_newline \
				pcre_ord2utf8 \
				pcre_refcount \
				pcre_study \
				pcre_tables \
				pcre_try_flipped \
				pcre_ucd \
				pcre_valid_utf8 \
				pcre_version \
				pcre_xclass \
				pcre_chartables

regex_C_FILES 					:= $(addprefix external$(_)pcre$(_),$(pcre_C_FILES))
tbox_C_FILES-$(CONFIG_REGEX) 	+= regex $(regex_C_FILES)

# sub-projects
ifneq ($(CONFIG_DEMO),)
SUB_PROS 			= demo$(_)$(CONFIG_DEMO)
endif

include $(PRO_DIR)$(_)suffix.mak

