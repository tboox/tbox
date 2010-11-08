# sub-makefile
include $(PRO_DIR)$(_)prefix.mak

# module name
NAMES 				= tbox

# module type
tbox_TYPE 			= LIB

# basic files
tbox_C_FILES 		+= \
					bits \
					bswap \
					malloc \
					math \
					pool \
					memops \
					string \
					array \
					conv \
					http \
					stream \
					stream$(_)http \
					stream$(_)file \
					stream$(_)data

# header files
INC_FILES 			= \
					prefix.h \
					config.h \
					type.h \
					tbox.h \
					option.h \
					bits.h \
					bswap.h \
					malloc.h \
					math.h \
					pool.h \
					array.h \
					conv.h \
					http.h \
					memops.h \
					string.h \
					stream.h \
					external$(_)prefix.h \
					external$(_)external.h 

# encoding files
tbox_C_FILES-$(TB_CONFIG_ENCODING) 	+= \
									encoding \
									encoding$(_)unicode_gb2312 \
									encoding$(_)gb2312_unicode

INC_FILES-$(TB_CONFIG_ENCODING) 	+= encoding.h

# format files
tbox_C_FILES-$(TB_CONFIG_FORMAT) 	+= \
									format \
									format$(_)flv \
									format$(_)avi \
									format$(_)mp3 \
									format$(_)wav \
									format$(_)bmp \
									format$(_)gif \
									format$(_)jpg \
									format$(_)png \
									format$(_)swf \
									format$(_)zip \
									format$(_)bz2 \
									format$(_)gz \
									format$(_)7z \
									format$(_)pdf \
									format$(_)xml \
									format$(_)htm \
									format$(_)txt 

INC_FILES-$(TB_CONFIG_ENCODING) 	+= format.h


# xml files
tbox_C_FILES-$(TB_CONFIG_XML) 		+= \
									xml$(_)document \
									xml$(_)node \
									xml$(_)nlist \
									xml$(_)reader \
									xml$(_)writer 

INC_FILES-$(TB_CONFIG_ENCODING) 	+= \
									xml$(_)xml.h \
									xml$(_)prefix.h \
									xml$(_)document.h \
									xml$(_)node.h \
									xml$(_)reader.h \
									xml$(_)writer.h \
									xml$(_)nlist.h

# zlib files
zlib_C_FILES 		= \
					adler32 \
					crc32 \
					infback \
					inffast \
					inflate \
					inftrees \
					uncompr \
					zutil

zlib_INC_FILES 		= \
					crc32.h \
					deflate.h \
					inffast.h \
					inffixed.h \
					inflate.h \
					inftrees.h \
					trees.h \
					zconf.h \
					zconf.in.h \
					zlib.h \
					zutil.h


zlib_C_FILES 					:= $(addprefix external$(_)zlib$(_),$(zlib_C_FILES))
tbox_C_FILES-$(TB_CONFIG_ZLIB) 	+= zlib $(zlib_C_FILES)

zlib_INC_FILES 					:= $(addprefix external$(_)zlib$(_),$(zlib_INC_FILES))
INC_FILES-$(TB_CONFIG_ZLIB) 	+= zlib.h $(zlib_INC_FILES)

# jpeg files
jpeg_C_FILES 		= \
					jaricom \
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

jpeg_INC_FILES 		= \
					cderror.h \
					cdjpeg.h \
					jconfig.h \
					jdct.h \
					jerror.h \
					jinclude.h \
					jmemsys.h \
					jmorecfg.h \
					jpegint.h \
					jpeglib.h \
					jversion.h \
					transupp.h


jpeg_C_FILES 					:= $(addprefix external$(_)jpeg$(_),$(jpeg_C_FILES))
tbox_C_FILES-$(TB_CONFIG_JPEG) 	+= $(jpeg_C_FILES)

jpeg_INC_FILES 					:= $(addprefix external$(_)jpeg$(_),$(jpeg_INC_FILES))
INC_FILES-$(TB_CONFIG_JPEG) 	+= $(jpeg_INC_FILES)
	
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

pcre_INC_FILES 	= \
				config.h \
				pcrecpparg.h \
				pcrecpp.h \
				pcrecpp_internal.h \
				pcre.h \
				pcre_internal.h \
				pcre_scanner.h \
				pcre_stringpiece.h \
				ucp.h 


pcre_C_FILES 					:= $(addprefix external$(_)pcre$(_),$(pcre_C_FILES))
tbox_C_FILES-$(TB_CONFIG_REGEX) += regex $(pcre_C_FILES)

pcre_INC_FILES 					:= $(addprefix external$(_)pcre$(_),$(pcre_INC_FILES))
INC_FILES-$(TB_CONFIG_REGEX) 	+= regex.h $(pcre_INC_FILES)

# cairo files
cairo_C_FILES 	= \
				cairo-arc \
				cairo-array \
				cairo-atomic \
				cairo-bentley-ottmann \
				cairo-font-face-twin \
				cairo-font-face-twin-data \
				cairo \
				cairo-cache \
				cairo-clip \
				cairo-color \
				cairo-debug \
				cairo-fixed \
				cairo-freelist \
				cairo-font-face \
				cairo-font-options \
				cairo-user-font \
				cairo-analysis-surface \
				cairo-gstate \
				cairo-hash \
				cairo-hull \
				cairo-image-surface \
				cairo-lzw \
				cairo-matrix \
				cairo-misc \
				cairo-mutex \
				cairo-output-stream \
				cairo-path-bounds \
				cairo-path \
				cairo-path-fill \
				cairo-path-fixed \
				cairo-path-stroke \
				cairo-pattern \
				cairo-pen \
				cairo-polygon \
				cairo-rectangle \
				cairo-region \
				cairo-scaled-font \
				cairo-scaled-font-subsets \
				cairo-slope \
				cairo-spline \
				cairo-stroke-style \
				cairo-surface \
				cairo-surface-fallback \
				cairo-system \
				cairo-traps \
				cairo-truetype-subset \
				cairo-type1-fallback \
				cairo-type1-subset \
				cairo-type3-glyph-surface \
				cairo-unicode \
				cairo-version \
				cairo-rectangular-scan-converter \
				cairo-composite-rectangles \
				cairo-surface-snapshot \
				cairo-bentley-ottmann-rectangular \
				cairo-boxes \
				cairo-spans \
				cairo-recording-surface \
				cairo-bentley-ottmann-rectilinear \
				cairo-toy-font-face \
				cairo-path-in-fill \
				cairo-tor-scan-converter \
				cairo-device \
				cairo-surface-wrapper \
				cairo-tee-surface \
				cairo-wideint \
				pixman$(_)pixman-access \
				pixman$(_)pixman-access-accessors \
				pixman$(_)pixman-cpu \
				pixman$(_)pixman-gradient-walker \
				pixman$(_)pixman-region16 \
				pixman$(_)pixman-region32 \
				pixman$(_)pixman-image 	\
				pixman$(_)pixman-implementation \
				pixman$(_)pixman-combine32 \
				pixman$(_)pixman-combine64 \
				pixman$(_)pixman-general \
				pixman$(_)pixman \
				pixman$(_)pixman-fast-path \
				pixman$(_)pixman-solid-fill \
				pixman$(_)pixman-conical-gradient \
				pixman$(_)pixman-linear-gradient \
				pixman$(_)pixman-radial-gradient \
				pixman$(_)pixman-bits-image \
				pixman$(_)pixman-utils 	\
				pixman$(_)pixman-edge \
				pixman$(_)pixman-edge-accessors \
				pixman$(_)pixman-trap \
				pixman$(_)pixman-timer \
				pixman$(_)pixman-matrix

cairo_INC_FILES = \
				cairo-analysis-surface-private.h \
				cairo-arc-private.h \
				cairo-atomic-private.h \
				cairo-beos.h \
				cairo-boxes-private.h \
				cairo-cache-private.h \
				cairo-clip-private.h \
				cairo-combsort-private.h \
				cairo-compiler-private.h \
				cairo-composite-rectangles-private.h \
				cairo-deprecated.h \
				cairo-device-private.h \
				cairo-directfb.h \
				cairo-drm.h \
				cairo-error-private.h \
				cairo-features.h \
				cairo-fixed-private.h \
				cairo-fixed-type-private.h \
				cairo-freed-pool-private.h \
				cairo-freelist-private.h \
				cairo-ft.h \
				cairo-ft-private.h \
				cairo-gl.h \
				cairo-gl-private.h \
				cairo-gstate-private.h \
				cairo.h \
				cairo-hash-private.h \
				cairo-image-info-private.h \
				cairoint.h \
				cairo-list-private.h \
				cairo-malloc-private.h \
				cairo-mutex-impl-private.h \
				cairo-mutex-list-private.h \
				cairo-mutex-private.h \
				cairo-mutex-type-private.h \
				cairo-os2.h \
				cairo-os2-private.h \
				cairo-output-stream-private.h \
				cairo-paginated-private.h \
				cairo-paginated-surface-private.h \
				cairo-path-fixed-private.h \
				cairo-path-private.h \
				cairo-pdf.h \
				cairo-pdf-operators-private.h \
				cairo-pdf-surface-private.h \
				cairo-private.h \
				cairo-ps.h \
				cairo-ps-surface-private.h \
				cairo-qt.h \
				cairo-quartz.h \
				cairo-quartz-image.h \
				cairo-quartz-private.h \
				cairo-recording-surface-private.h \
				cairo-reference-count-private.h \
				cairo-region-private.h \
				cairo-rtree-private.h \
				cairo-scaled-font-private.h \
				cairo-scaled-font-subsets-private.h \
				cairo-script.h \
				cairo-skia.h \
				cairo-slope-private.h \
				cairo-spans-private.h \
				cairo-surface-clipper-private.h \
				cairo-surface-fallback-private.h \
				cairo-surface-offset-private.h \
				cairo-surface-private.h \
				cairo-surface-snapshot-private.h \
				cairo-surface-subsurface-private.h \
				cairo-surface-wrapper-private.h \
				cairo-svg.h \
				cairo-svg-surface-private.h \
				cairo-tee-surface-private.h \
				cairo-truetype-subset-private.h \
				cairo-type1-private.h \
				cairo-type3-glyph-surface-private.h \
				cairo-types-private.h \
				cairo-user-font-private.h \
				cairo-version.h \
				cairo-vg.h \
				cairo-wideint-private.h \
				cairo-wideint-type-private.h \
				cairo-win32.h \
				cairo-win32-private.h \
				cairo-xcb.h \
				cairo-xcb-private.h \
				cairo-xcb-surface-private.h \
				cairo-xlib.h \
				cairo-xlib-private.h \
				cairo-xlib-surface-private.h \
				cairo-xlib-xrender.h \
				cairo-xlib-xrender-private.h \
				cairo-xml.h \
				config.h


cairo_C_FILES 					:= $(addprefix external$(_)cairo$(_),$(cairo_C_FILES))
tbox_C_FILES-$(TB_CONFIG_CAIRO) += $(cairo_C_FILES)

cairo_INC_FILES 				:= $(addprefix external$(_)cairo$(_),$(cairo_INC_FILES))
INC_FILES-$(TB_CONFIG_CAIRO) 	+= $(cairo_INC_FILES)

# libmad files
libmad_C_FILES 		= \
					version \
					fixed \
					bit \
					timer \
					stream \
					frame \
					synth \
					decoder \
					layer12 \
					layer3 \
					huffman

libmad_INC_FILES 	= \
					bit.h \
					config.h \
					decoder.h \
					fixed.h \
					frame.h \
					global.h \
					huffman.h \
					layer12.h \
					layer3.h \
					mad.h \
					stream.h \
					synth.h \
					timer.h \
					version.h

libmad_C_FILES 					:= $(addprefix external$(_)libmad$(_),$(libmad_C_FILES))
tbox_C_FILES-$(TB_CONFIG_LIBMAD) += $(libmad_C_FILES)

libmad_INC_FILES 				:= $(addprefix external$(_)libmad$(_),$(libmad_INC_FILES))
INC_FILES-$(TB_CONFIG_LIBMAD) 	+= $(libmad_INC_FILES)

# sub-projects
ifneq ($(TB_CONFIG_DEMO),)
SUB_PROS 			= demo$(_)$(TB_CONFIG_DEMO)
endif

include $(PRO_DIR)$(_)suffix.mak

