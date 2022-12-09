#!/bin/sh

set_project "tbox"
set_version "1.7.1" "%Y%m%d%H%M"

# set warning all as error
set_warnings "all" "error"

# set language: c99
set_languages "c99"

# add defines to config.h
set_configvar "_GNU_SOURCE" 1
set_configvar "_REENTRANT" 1

# add build modes
if is_mode "debug"; then
    add_defines "__tb_debug__"
fi

# small or micro?
if has_config "small"; then
    add_defines "__tb_small__"
    set_configvar "TB_CONFIG_SMALL" 1
    add_cxflags "-fno-stack-protector"
fi

if is_plat "mingw" "msys" "cygwin"; then
    add_syslinks "ws2_32" "pthread" "m"
else
    add_syslinks "pthread" "dl" "m" "c"
fi

# enable backtrace symbols for linux
if is_plat "linux" && is_mode "debug"; then
    add_ldflags "-rdynamic"
fi

# options
option "demo" "Enable or disable the demo module." true
option "small" "Enable the small compile mode and disable all modules." true

option "micro"
    set_default false
    set_description "Compile micro core library for the embed system."
    set_configvar "TB_CONFIG_MICRO_ENABLE" 1

option "wchar"
    add_ctypes "wchar_t"
    set_configvar "TB_CONFIG_TYPE_HAVE_WCHAR" 1

option "float"
    set_default true
    set_description "Enable or disable the float type"
    set_configvar "TB_CONFIG_TYPE_HAVE_FLOAT" 1

option "info"
    set_default true
    set_description "Enable or disable to get some info, .e.g version .."
    set_configvar "TB_CONFIG_INFO_HAVE_VERSION" 1
    set_configvar "TB_CONFIG_INFO_TRACE_MORE" 1

option "exception"
    set_default false
    set_description "Enable or disable the exception."
    set_configvar "TB_CONFIG_EXCEPTION_ENABLE" 1

option "deprecated"
    set_default false
    set_description "Enable or disable the deprecated interfaces."
    set_configvar "TB_CONFIG_API_HAVE_DEPRECATED" 1

option "force_utf8"
    set_default false
    set_description "Forcely regard all tb_char* as utf-8."
    set_configvar "TB_CONFIG_FORCE_UTF8" 1

module_options() {
    local modules="xml zip hash regex object charset database coroutine"
    for name in ${modules}; do
        local name_upper=$(string_toupper "${name}")
        option "${name}"
            set_default true
            set_description "The ${name} module"
            set_configvar "TB_CONFIG_MODULE_HAVE_${name_upper}" 1
    done
    option_end
}
module_options

includes "tbox"
if has_config "demo"; then
    includes "demo"
fi
