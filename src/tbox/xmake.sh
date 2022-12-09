#!/bin/sh

target "tbox"
    set_kind "static"

    # add defines
    add_defines "__tb_prefix__=\"tbox\""

    # set the auto-generated config.h
    set_configdir "${buildir}/${plat}/${arch}/${mode}"
    add_configfiles "tbox.config.h.in"

    # add include directories
    add_includedirs ".."
    add_includedirs "${buildir}/${plat}/${arch}/${mode}"

    # add the header files for installing
    add_headerfiles "../(tbox/**.h)"
    add_headerfiles "../(tbox/**.S)"
    add_headerfiles "${buildir}/${plat}/${arch}/${mode}/tbox.config.h" "tbox"

    # add frameworks
    if is_plat "macosx"; then
        add_frameworks "CoreFoundation" "CoreServices"
    fi

    # add options
    add_options "info" "float" "wchar" "exception" "force_utf8" "micro" "deprecated"

    # add modules
    add_options "xml" "zip" "hash" "regex" "coroutine" "object" "charset" "database"

    # add the common source files
    add_files "*.c"
    add_files "hash/bkdr.c"
    add_files "hash/fnv32.c"
    add_files "hash/adler32.c"
    add_files "math/**.c"
    add_files "libc/**.c"
    add_files "utils/*.c"
    add_files "prefix/**.c"
    add_files "memory/**.c"
    add_files "string/**.c"
    add_files "stream/**.c"
    add_files "network/**.c"
    add_files "algorithm/**.c"
    add_files "container/**.c"
    add_files "libm/impl/libm.c"
    add_files "libm/idivi8.c"
    add_files "libm/ilog2i.c"
    add_files "libm/isqrti.c"
    add_files "libm/isqrti64.c"
    add_files "libm/idivi8.c"
    add_files "platform/*.c"
    add_files "platform/impl/*.c"

    # check interfaces
    check_interfaces

