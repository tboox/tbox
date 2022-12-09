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

# disable all packages now
package_options() {
    local packages="zlib sqlite3 mysql mbedtls openssl polarssl pcre2 pcre"
    for name in ${packages}; do
        local name_upper=$(string_toupper "${name}")
        set_configvar "TB_CONFIG_PACKAGE_HAVE_${name_upper}" 0
    done
}
package_options

# get function name
#
# sigsetjmp
# sigsetjmp((void*)0, 0)
#
get_function_name() {
    local name=$(string_split "${1}" "(" 1)
    echo "${name}"
}

# check c functions in the given module
check_module_cfuncs() {
    local module="${1}"
    local cincludes="${2}"
    shift
    shift
    for func in ${@}; do
        local funcname=$(get_function_name "${func}")
        local optname="${module}_${funcname}"
        local module_upper=$(string_toupper "${module}")
        local funcname_upper=$(string_toupper "${funcname}")
        option "${optname}"
            add_cfuncs "${func}"
            add_cincludes "${cincludes}"
            add_defines "_GNU_SOURCE=1"
            set_warnings "error"
            set_languages "c99"
            set_configvar "TB_CONFIG_${module_upper}_HAVE_${funcname_upper}" 1
        option_end
        add_options "${optname}"
    done
}

# check c snippets in the given module
check_module_csnippets() {
    local optname="${1}"
    local varname="${2}"
    local snippets="${3}"
    local links="${4}"
    option "${optname}"
        add_csnippets "${snippets}"
        add_defines "_GNU_SOURCE=1"
        set_warnings "error"
        set_languages "c99"
        set_configvar "${varname}" 1
        if test_nz "${links}"; then
            add_links "${links}"
        fi
    option_end
    add_options "${optname}"
}

# disable c functions in the given module
disable_module_cfuncs() {
    local module="${1}"
    local cincludes="${2}"
    shift
    shift
    for func in ${@}; do
        local funcname=$(get_function_name "${func}")
        local module_upper=$(string_toupper "${module}")
        local funcname_upper=$(string_toupper "${funcname}")
        set_configvar "TB_CONFIG_${module_upper}_HAVE_${funcname_upper}" 0
    done
}

# check interfaces
check_interfaces() {

    # check the interfaces for libc
    check_module_cfuncs "libc" "string.h stdlib.h" \
        "memcpy" \
        "memset" \
        "memmove" \
        "memcmp" \
        "memmem" \
        "strcat" \
        "strncat" \
        "strcpy" \
        "strncpy" \
        "strlcpy" \
        "strlen" \
        "strnlen" \
        "strstr" \
        "strchr" \
        "strrchr" \
        "strcasestr" \
        "strcmp" \
        "strcasecmp" \
        "strncmp" \
        "strncasecmp"

    check_module_cfuncs "libc" "wchar.h stdlib.h" \
        "wcscat" \
        "wcsncat" \
        "wcscpy" \
        "wcsncpy" \
        "wcslcpy" \
        "wcslen" \
        "wcsnlen" \
        "wcsstr" \
        "wcscasestr" \
        "wcscmp" \
        "wcscasecmp" \
        "wcsncmp" \
        "wcsncasecmp" \
        "wcstombs" \
        "mbstowcs"
    check_module_cfuncs "libc" "time.h"                           "gmtime" "mktime" "localtime"
    check_module_cfuncs "libc" "sys/time.h"                       "gettimeofday"
    check_module_cfuncs "libc" "signal.h setjmp.h"                "signal" "setjmp" "sigsetjmp(0,0)" "kill"
    check_module_cfuncs "libc" "execinfo.h"                       "backtrace"
    check_module_cfuncs "libc" "locale.h"                         "setlocale"
    check_module_cfuncs "libc" "stdio.h"                          "fputs" "fgets" "fgetc" "ungetc" "fputc" "fread" "fwrite"
    check_module_cfuncs "libc" "stdlib.h"                         "srandom" "random"

    # add the interfaces for libm
    check_module_cfuncs "libm" "math.h" \
        "sincos" \
        "sincosf" \
        "log2" \
        "log2f" \
        "sqrt" \
        "sqrtf" \
        "acos" \
        "acosf" \
        "asin" \
        "asinf" \
        "pow" \
        "powf" \
        "fmod" \
        "fmodf" \
        "tan" \
        "tanf" \
        "atan" \
        "atanf" \
        "atan2" \
        "atan2f" \
        "cos" \
        "cosf" \
        "sin" \
        "sinf" \
        "exp" \
        "expf"

    # add the interfaces for posix
    check_module_cfuncs "posix" "poll.h sys/socket.h"             "poll"
    check_module_cfuncs "posix" "sys/select.h"                    "select"
    check_module_cfuncs "posix" "pthread.h" \
        "pthread_mutex_init" \
        "pthread_create" \
        "pthread_setspecific" \
        "pthread_getspecific" \
        "pthread_key_create" \
        "pthread_key_delete" \
        "pthread_setaffinity_np" # need _GNU_SOURCE
    check_module_cfuncs "posix" "sys/socket.h fcntl.h"             "socket"
    check_module_cfuncs "posix" "dirent.h"                         "opendir"
    check_module_cfuncs "posix" "dlfcn.h"                          "dlopen"
    check_module_cfuncs "posix" "sys/stat.h fcntl.h"               "open" "stat64" "lstat64"
    check_module_cfuncs "posix" "unistd.h"                         "gethostname"
    check_module_cfuncs "posix" "ifaddrs.h"                        "getifaddrs"
    check_module_cfuncs "posix" "semaphore.h"                      "sem_init"
    check_module_cfuncs "posix" "unistd.h"                         "getpagesize" "sysconf"
    check_module_cfuncs "posix" "sched.h"                          "sched_yield" "sched_setaffinity" # need _GNU_SOURCE
    check_module_cfuncs "posix" "regex.h"                          "regcomp" "regexec"
    check_module_cfuncs "posix" "sys/uio.h"                        "readv" "writev" "preadv" "pwritev"
    check_module_cfuncs "posix" "unistd.h"                         "pread64" "pwrite64"
    check_module_cfuncs "posix" "unistd.h"                         "fdatasync"
    check_module_cfuncs "posix" "copyfile.h"                       "copyfile"
    check_module_cfuncs "posix" "sys/sendfile.h"                   "sendfile"
    check_module_cfuncs "posix" "sys/epoll.h"                      "epoll_create" "epoll_wait"
    check_module_cfuncs "posix" "spawn.h"                          "posix_spawnp" "posix_spawn_file_actions_addchdir_np"
    check_module_cfuncs "posix" "unistd.h"                         "execvp" "execvpe" "fork" "vfork"
    check_module_cfuncs "posix" "sys/wait.h"                       "waitpid"
    check_module_cfuncs "posix" "unistd.h"                         "getdtablesize"
    check_module_cfuncs "posix" "sys/resource.h"                   "getrlimit"
    check_module_cfuncs "posix" "netdb.h"                          "getaddrinfo" "getnameinfo" "gethostbyname" "gethostbyaddr"
    check_module_cfuncs "posix" "fcntl.h"                          "fcntl"
    check_module_cfuncs "posix" "unistd.h"                         "pipe" "pipe2"
    check_module_cfuncs "posix" "sys/stat.h"                       "mkfifo"
    check_module_cfuncs "posix" "sys/mman.h"                       "mmap"
    check_module_cfuncs "posix" "sys/stat.h"                       "futimens" "utimensat"

    # add the interfaces for bsd
    check_module_cfuncs "bsd" "sys/file.h fcntl.h" "flock"

    # add the interfaces for systemv
    check_module_cfuncs "systemv" "sys/sem.h sys/ipc.h" "semget" "semtimedop"

    # add the interfaces for linux
    check_module_cfuncs "linux" "sys/inotify.h" "inotify_init"

    # add the interfaces for valgrind
    check_module_cfuncs "valgrind" "valgrind/valgrind.h" "VALGRIND_STACK_REGISTER(0,0)"

    # add the interfaces for windows/msvc
    local mos="z _nf _acq _rel"
    for mo in ${mos}; do
        if test_eq "${mo}" "z"; then
            mo=""
        fi
        disable_module_cfuncs "windows" "windows.h" "_InterlockedExchange${mo}"
        disable_module_cfuncs "windows" "windows.h" "_InterlockedExchange8${mo}"
        disable_module_cfuncs "windows" "windows.h" "_InterlockedOr8${mo}"
        disable_module_cfuncs "windows" "windows.h" "_InterlockedExchangeAdd${mo}"
        disable_module_cfuncs "windows" "windows.h" "_InterlockedExchangeAdd64${mo}"
        disable_module_cfuncs "windows" "windows.h" "_InterlockedCompareExchange${mo}"
        disable_module_cfuncs "windows" "windows.h" "_InterlockedCompareExchange64${mo}"
    done

    # check __thread keyword
    check_module_csnippets "keyword_thread" "TB_CONFIG_KEYWORD_HAVE__thread" "__thread int a = 0;" "pthread"
    check_module_csnippets "keyword_thread_local" "TB_CONFIG_KEYWORD_HAVE_Thread_local" "_Thread_local int a = 0;" "pthread"

    # check anonymous union feature
    check_module_csnippets "feature_anonymous_union" "TB_CONFIG_FEATURE_HAVE_ANONYMOUS_UNION" "void test() { struct __st { union {int dummy;};} a; a.dummy = 1; }"
}

includes "tbox"
if has_config "demo"; then
    includes "demo"
fi
