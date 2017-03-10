-- option: demo
option("demo")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the demo module")

-- option: wchar
option("wchar")
    add_ctypes("wchar_t")
    add_defines_h_if_ok("$(prefix)_TYPE_HAVE_WCHAR")

-- option: float
option("float")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the float type")
    add_defines_h_if_ok("$(prefix)_TYPE_HAVE_FLOAT")

-- option: info
option("info")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable to get some info, .e.g version ..")
    add_defines_h_if_ok("$(prefix)_INFO_HAVE_VERSION")
    add_defines_h_if_ok("$(prefix)_INFO_TRACE_MORE")

-- option: exception
option("exception")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the exception.")
    add_defines_h_if_ok("$(prefix)_EXCEPTION_ENABLE")

-- option: deprecated
option("deprecated")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the deprecated interfaces.")
    add_defines_h_if_ok("$(prefix)_API_HAVE_DEPRECATED")

-- option: micro
option("micro")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Compile micro core library for the embed system.")
    add_defines_h_if_ok("$(prefix)_MICRO_ENABLE")
    add_rbindings("info", "deprecated", "float")
    add_rbindings("xml", "zip", "asio", "hash", "regex", "object", "charset", "database", "coroutine")
    add_rbindings("zlib", "mysql", "sqlite3", "openssl", "polarssl", "mbedtls", "pcre2", "pcre")

-- option: small
option("small")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable the small compile mode and disable all modules.")
    add_rbindings("info", "deprecated")
    add_rbindings("xml", "zip", "asio", "hash", "regex", "object", "charset", "database", "coroutine")
    add_rbindings("zlib", "mysql", "sqlite3", "openssl", "polarssl", "mbedtls", "pcre2", "pcre")

-- add modules
for _, module in ipairs({"xml", "zip", "hash", "regex", "object", "charset", "database", "coroutine"}) do
    option(module)
        set_default(true)
        set_showmenu(true)
        set_category("module")
        set_description(format("The %s module", module))
        add_defines_h_if_ok(format("$(prefix)_MODULE_HAVE_%s", module:upper()))
end

-- check interfaces
function check_interfaces()

    -- add the interfaces for libc
    add_cfuncs("libc", nil,         {"string.h", "stdlib.h"},           "memcpy",
                                                                        "memset",
                                                                        "memmove",
                                                                        "memcmp",
                                                                        "memmem",
                                                                        "strcat",
                                                                        "strncat",
                                                                        "strcpy",
                                                                        "strncpy",
                                                                        "strlcpy",
                                                                        "strlen",
                                                                        "strnlen",
                                                                        "strstr",
                                                                        "strcasestr",
                                                                        "strcmp",
                                                                        "strcasecmp",
                                                                        "strncmp",
                                                                        "strncasecmp")
    add_cfuncs("libc", nil,         {"wchar.h", "stdlib.h"},            "wcscat",
                                                                        "wcsncat",
                                                                        "wcscpy",
                                                                        "wcsncpy",
                                                                        "wcslcpy",
                                                                        "wcslen",
                                                                        "wcsnlen",
                                                                        "wcsstr",
                                                                        "wcscasestr",
                                                                        "wcscmp",
                                                                        "wcscasecmp",
                                                                        "wcsncmp",
                                                                        "wcsncasecmp",
                                                                        "wcstombs",
                                                                        "mbstowcs")
    add_cfuncs("libc", nil,         "time.h",                           "gmtime", "mktime", "localtime")
    add_cfuncs("libc", nil,         "sys/time.h",                       "gettimeofday")
    add_cfuncs("libc", nil,         {"signal.h", "setjmp.h"},           "signal", "setjmp", "sigsetjmp{sigjmp_buf buf; sigsetjmp(buf, 0);}", "kill")
    add_cfuncs("libc", nil,         "execinfo.h",                       "backtrace")
    add_cfuncs("libc", nil,         "locale.h",                         "setlocale")
    add_cfuncs("libc", nil,         "stdio.h",                          "fputs")
    add_cfuncs("libc", nil,         "stdlib.h",                         "srandom", "random")

    -- add the interfaces for libm
    add_cfuncs("libm", nil,         "math.h",                           "sincos", 
                                                                        "sincosf", 
                                                                        "log2", 
                                                                        "log2f",
                                                                        "sqrt",
                                                                        "sqrtf",
                                                                        "acos", 
                                                                        "acosf",
                                                                        "asin",
                                                                        "asinf",
                                                                        "pow",
                                                                        "powf",
                                                                        "fmod",
                                                                        "fmodf",
                                                                        "tan",
                                                                        "tanf",
                                                                        "atan",
                                                                        "atanf",
                                                                        "atan2",
                                                                        "atan2f",
                                                                        "cos",
                                                                        "cosf",
                                                                        "sin",
                                                                        "sinf",
                                                                        "exp",
                                                                        "expf")

    -- add the interfaces for posix
    add_cfuncs("posix", nil,        {"sys/poll.h", "sys/socket.h"},     "poll")
    add_cfuncs("posix", nil,        {"sys/select.h"},                   "select")
    add_cfuncs("posix", nil,        "pthread.h",                        "pthread_mutex_init",
                                                                        "pthread_create", 
                                                                        "pthread_setspecific", 
                                                                        "pthread_getspecific",
                                                                        "pthread_key_create",
                                                                        "pthread_key_delete")
    add_cfuncs("posix", nil,        {"sys/socket.h", "fcntl.h"},        "socket")
    add_cfuncs("posix", nil,        "dirent.h",                         "opendir")
    add_cfuncs("posix", nil,        "dlfcn.h",                          "dlopen")
    add_cfuncs("posix", nil,        {"sys/stat.h", "fcntl.h"},          "open", "stat64")
    add_cfuncs("posix", nil,        "unistd.h",                         "gethostname")
    add_cfuncs("posix", nil,        "ifaddrs.h",                        "getifaddrs")
    add_cfuncs("posix", nil,        "semaphore.h",                      "sem_init")
    add_cfuncs("posix", nil,        "unistd.h",                         "getpagesize", "sysconf")
    add_cfuncs("posix", nil,        "sched.h",                          "sched_yield")
    add_cfuncs("posix", nil,        "regex.h",                          "regcomp", "regexec")
    add_cfuncs("posix", nil,        "sys/uio.h",                        "readv", "writev", "preadv", "pwritev")
    add_cfuncs("posix", nil,        "unistd.h",                         "pread64", "pwrite64")
    add_cfuncs("posix", nil,        "unistd.h",                         "fdatasync")
    add_cfuncs("posix", nil,        "sys/sendfile.h",                   "sendfile")
    add_cfuncs("posix", nil,        "sys/epoll.h",                      "epoll_create", "epoll_wait")
    add_cfuncs("posix", nil,        "spawn.h",                          "posix_spawnp")
    add_cfuncs("posix", nil,        "unistd.h",                         "execvp", "execvpe", "fork", "vfork")
    add_cfuncs("posix", nil,        "sys/wait.h",                       "waitpid")
    add_cfuncs("posix", nil,        "unistd.h",                         "getdtablesize")
    add_cfuncs("posix", nil,        "sys/resource.h",                   "getrlimit")
    add_cfuncs("posix", nil,        "netdb.h",                          "getaddrinfo", "getnameinfo", "gethostbyname", "gethostbyaddr")

    -- add the interfaces for systemv
    add_cfuncs("systemv", nil,      {"sys/sem.h", "sys/ipc.h"},         "semget", "semtimedop")
end

-- add project directories
add_subfiles(format("tbox/%s.lua", ifelse(is_option("micro"), "micro", "xmake"))) 
if is_option("demo") then 
    add_subfiles(format("demo/%s.lua", ifelse(is_option("micro"), "micro", "xmake"))) 
end
