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

    # check interfaces
    check_interfaces

