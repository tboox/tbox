#!/bin/sh

set_project "tbox"
set_version "1.7.5" "%Y%m%d" "1"

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
    set_symbols "debug"
    set_optimizes "none"
else
    set_strip "all"
    if ! is_kind "shared"; then
        set_symbols "hidden"
    fi
    set_optimizes "smallest"
fi

# small or micro?
if has_config "small"; then
    add_defines "__tb_small__"
    set_configvar "TB_CONFIG_SMALL" 1
    add_cxflags "-fno-stack-protector"
fi

if is_plat "mingw" "msys" "cygwin"; then
    add_syslinks "ws2_32" "pthread" "m"
elif is_plat "haiku"; then
    add_syslinks "pthread" "network" "m" "c"
else
    add_syslinks "pthread" "dl" "m" "c"
fi

# enable backtrace symbols for linux
if is_plat "linux" && is_mode "debug"; then
    add_ldflags "-rdynamic"
fi

# options
option "demo"       "Enable or disable the demo module." true
option "small"      "Enable the small compile mode and disable all modules." true
option "micro"      "Compile micro core library for the embed system." false
option "float"      "Enable or disable the float type" true
option "info"       "Enable or disable to get some info, .e.g version .." true
option "exception"  "Enable or disable the exception." false
option "deprecated" "Enable or disable the deprecated interfaces." false
option "force_utf8" "Forcely regard all tb_char* as utf-8." false

option "wchar"
    add_ctypes "wchar_t"
option_end

basic_options() {
    if has_config "micro"; then
        set_configvar "TB_CONFIG_MICRO_ENABLE" 1
    fi

    if has_config "wchar"; then
        set_configvar "TB_CONFIG_TYPE_HAVE_WCHAR" 1
    fi

    if has_config "float"; then
        set_configvar "TB_CONFIG_TYPE_HAVE_FLOAT" 1
    fi

    if has_config "info"; then
        set_configvar "TB_CONFIG_INFO_HAVE_VERSION" 1
        set_configvar "TB_CONFIG_INFO_TRACE_MORE" 1
    fi

    if has_config "exception"; then
        set_configvar "TB_CONFIG_EXCEPTION_ENABLE" 1
    fi

    if has_config "deprecated"; then
        set_configvar "TB_CONFIG_API_HAVE_DEPRECATED" 1
    fi

    if has_config "force_utf8"; then
        set_configvar "TB_CONFIG_FORCE_UTF8" 1
    fi
}
basic_options

# we disable all modules by default, but we can use `./configure --hash=y` to enable it
module_options() {
    local modules="xml zip hash regex object charset database coroutine"
    for name in ${modules}; do
        string_toupper "${name}"; local name_upper="${_ret}"
        option "${name}" "The ${name} module" false
        if has_config "${name}"; then
            set_configvar "TB_CONFIG_MODULE_HAVE_${name_upper}" 1
        fi
        option_end
    done
}
module_options

# check c functions in the given module
check_module_cfuncs() {
    local module="${1}"
    local cincludes="${2}"
    shift
    shift
    for funcname in ${@}; do
        local optname="${module}_${funcname}"
        string_toupper "${module}"; local module_upper="${_ret}"
        string_toupper "${funcname}"; local funcname_upper="${_ret}"
        option "${optname}"
            add_cfuncs "${funcname}"
            add_cincludes "${cincludes}"
            add_defines "_GNU_SOURCE=1"
            set_warnings "error"
            set_languages "c99"
        option_end
        if has_config "${optname}"; then
            set_configvar "TB_CONFIG_${module_upper}_HAVE_${funcname_upper}" 1
        fi
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
        if test_nz "${links}"; then
            add_links "${links}"
        fi
    option_end
    if has_config "${optname}"; then
        set_configvar "${varname}" 1
    fi
}

# check interfaces
#check_interfaces_enabled=false
check_interfaces() {
    if ! $check_interfaces_enabled; then
        return
    fi

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
    check_module_cfuncs "libc" "signal.h setjmp.h"                "signal" "setjmp" "kill"
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

    # add the interfaces for sigsetjmp
    check_module_csnippets "libc_sigsetjmp" "TB_CONFIG_LIBC_HAVE_SIGSETJMP" \
        "#include <signal.h>\n
         #include <setjmp.h>\n
         void test() {sigjmp_buf buf; sigsetjmp(buf, 0);}"

    # add the interfaces for valgrind
    check_module_csnippets "valgrind" "TB_CONFIG_VALGRIND_HAVE_VALGRIND_STACK_REGISTER" \
        "#include \"valgrind/valgrind.h\"\n
         void test() {VALGRIND_STACK_REGISTER(0,0);}"

    # check __thread keyword
    check_module_csnippets "keyword_thread" "TB_CONFIG_KEYWORD_HAVE__thread" "__thread int a = 0;" "pthread"
    check_module_csnippets "keyword_thread_local" "TB_CONFIG_KEYWORD_HAVE_Thread_local" "_Thread_local int a = 0;" "pthread"

    # check anonymous union feature
    check_module_csnippets "feature_anonymous_union" "TB_CONFIG_FEATURE_HAVE_ANONYMOUS_UNION" \
        "void test() { struct __st { union {int dummy;};} a; a.dummy = 1; }"
}

includes "tbox"
if has_config "demo"; then
    includes "demo"
fi
