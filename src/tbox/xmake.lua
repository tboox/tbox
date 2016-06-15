
-- add type: wchar
option("wchar")
    add_ctypes("wchar_t")
    add_defines_h_if_ok("$(prefix)_TYPE_HAVE_WCHAR")

-- add option: float
option("float")
    set_enable(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the float type")
    add_defines_h_if_ok("$(prefix)_TYPE_HAVE_FLOAT")

-- add option: info
option("info")
    set_enable(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable to get some info, .e.g version ..")
    add_defines_h_if_ok("$(prefix)_INFO_HAVE_VERSION")

-- add option: smallest
option("smallest")
    set_enable(false)
    set_showmenu(true)
    set_category("option")
    set_description("Enable the smallest compile mode and disable all modules.")
    add_rbindings("info")
    add_rbindings("xml", "zip", "asio", "regex", "object", "thread", "network", "charset", "database")
    add_rbindings("zlib", "mysql", "sqlite3", "openssl", "polarssl", "pcre2", "pcre", "base")

-- add modules
for _, module in ipairs({"xml", "zip", "asio", "regex", "object", "thread", "network", "charset", "database"}) do
    option(module)
        set_enable(true)
        set_showmenu(true)
        set_category("module")
        set_description(format("The %s module", module))
        add_defines_h_if_ok(format("$(prefix)_MODULE_HAVE_%s", module:upper()))
end

-- add target
target("tbox")

    -- make as a static library
    set_kind("static")

    -- add defines
    add_defines("__tb_prefix__=\"tbox\"")

    -- set the auto-generated config.h
    set_config_h("$(buildir)/tbox/tbox.config.h")
    set_config_h_prefix("TB_CONFIG")

    -- set the object files directory
    set_objectdir("$(buildir)/.objs")

    -- add includes directory
    add_includedirs("$(buildir)")
    add_includedirs("$(buildir)/tbox")

    -- add the header files for installing
    add_headers("../(tbox/**.h)|**/impl/**.h")
    add_headers("../(tbox/prefix/**/prefix.S)")
    add_headers("../(tbox/math/impl/*.h)")
    add_headers("../(tbox/utils/impl/*.h)")

    -- add options
    add_options("info", "float", "wchar")

    -- add modules
    add_options("xml", "zip", "asio", "regex", "object", "thread", "network", "charset", "database")

    -- add packages
    add_options("zlib", "mysql", "sqlite3", "openssl", "polarssl", "pcre2", "pcre", "base")

    -- add interfaces
    add_options(libc, libm, posix, systemv)

    -- add the common source files
    add_files("*.c") 
    add_files("math/**.c") 
    add_files("libc/**.c|string/impl/**.c") 
    add_files("utils/*.c|option.c") 
    add_files("prefix/**.c") 
    add_files("memory/**.c") 
    add_files("string/**.c") 
    add_files("stream/**.c|**/charset.c|**/zip.c|**async_**.c|transfer_pool.c|impl/stream/http.c|impl/stream/sock.c|impl/filter/chunked.c") 
    add_files("algorithm/**.c") 
    add_files("container/**.c|element/obj.c") 
    add_files("libm/libm.c") 
    add_files("libm/idivi8.c") 
    add_files("libm/ilog2i.c") 
    add_files("libm/isqrti.c") 
    add_files("libm/isqrti64.c") 
    add_files("libm/idivi8.c") 
    add_files("platform/*.c|aicp.c|aiop.c|aioo.c|socket.c|dns.c|thread*.c|event.c|semaphore.c|mutex.c|timer.c|ltimer.c")

    -- add the source files for arm
    if is_arch("arm.*") then
        add_files("utils/impl/crc_arm.S")
    end

    -- add the source files for the float type
    if is_option("float") then add_files("libm/*.c") end

    -- add the source files for the xml module
    if is_option("xml") then add_files("xml/**.c") end

    -- add the source files for the regex module
    if is_option("regex") then add_files("regex/*.c") end

    -- add the source files for the network module
    if is_option("network") then
        add_files("asio/aioo.c") 
        add_files("asio/aiop.c") 
        add_files("platform/dns.c") 
        add_files("platform/aioo.c") 
        add_files("platform/aiop.c") 
        add_files("platform/socket.c") 
        add_files("stream/impl/stream/http.c") 
        add_files("stream/impl/stream/sock.c") 
        add_files("stream/impl/filter/chunked.c") 
        add_files("network/**.c|impl/ssl/*.c") 
    else
        add_files("network/url.c") 
        add_files("network/ipv4.c") 
        add_files("network/ipv6.c") 
        add_files("network/ipaddr.c") 
        add_files("network/hwaddr.c") 
    end

    -- add the source files for the asio module
    if is_option("asio") then 
        add_files("asio/aico.c")
        add_files("asio/aicp.c")
        add_files("asio/http.c")
        add_files("asio/dns.c")
        add_files("stream/**async_**.c")
        add_files("stream/transfer_pool.c")
        add_files("platform/aicp.c")
        if is_option("openssl", "polarssl") then add_files("asio/ssl.c") end
    end

    -- add the source files for the thread module
    if is_option("thread") then
        add_files("platform/thread*.c") 
        add_files("platform/event.c") 
        add_files("platform/mutex.c") 
        add_files("platform/semaphore.c") 
        add_files("platform/timer.c") 
        add_files("platform/ltimer.c") 
    end

    -- add the source files for the object module
    if is_option("object") then 
        add_files("object/**.c|**/xml.c|**/xplist.c")
        add_files("utils/option.c")
        add_files("container/element/obj.c")
        if is_option("xml") then
            add_files("object/impl/reader/xml.c")
            add_files("object/impl/reader/xplist.c")
            add_files("object/impl/writer/xml.c")
            add_files("object/impl/writer/xplist.c")
        end
    end

    -- add the source files for the charset module
    if is_option("charset") then 
        add_files("charset/**.c")
        add_files("stream/impl/filter/charset.c")
    end

    -- add the source files for the zip module
    if is_option("zip") then 
        add_files("zip/**.c|gzip.c|zlib.c|zlibraw.c|lzsw.c")
        add_files("stream/impl/filter/zip.c")
        if is_option("zlib") then 
            add_files("zip/gzip.c") 
            add_files("zip/zlib.c") 
            add_files("zip/zlibraw.c") 
        end
    end

    -- add the source files for the database module
    if is_option("database") then 
        add_files("database/*.c")
        if is_option("mysql") then add_files("database/impl/mysql.c") end
        if is_option("sqlite3") then add_files("database/impl/sqlite3.c") end
    end

    -- add the source files for the ssl package
    if is_option("polarssl") then add_files("network/impl/ssl/polarssl.c") 
    elseif is_option("openssl") then add_files("network/impl/ssl/openssl.c") end

    -- add the source for the windows 
    if is_os("windows") then
        add_files("platform/windows/socket_pool.c")
        add_files("platform/windows/interface/*.c")
    end

    -- add the source for the ios 
    if is_os("ios") then
        add_files("platform/mach/ios/directory.m")
    end

    -- add the source for the android 
    if is_os("android") then
        add_files("platform/android/*.c|dns.c")
        if is_option("network") then
            add_files("platform/android/dns.c")
        end
    end

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
    add_cfuncs("libc", nil,         {"signal.h", "setjmp.h"},           "signal", "setjmp", "sigsetjmp", "kill")
    add_cfuncs("libc", nil,         "execinfo.h",                       "backtrace")

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
    add_cfuncs("posix", nil,        "pthread.h",                        "pthread_mutex_init", "pthread_create")
    add_cfuncs("posix", nil,        {"sys/socket.h", "fcntl.h"},        "socket")
    add_cfuncs("posix", nil,        "dirent.h",                         "opendir")
    add_cfuncs("posix", nil,        "dlfcn.h",                          "dlopen")
    add_cfuncs("posix", nil,        {"sys/stat.h", "fcntl.h"},          "open")
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

    -- add the interfaces for systemv
    add_cfuncs("systemv", nil,      {"sys/sem.h", "sys/ipc.h"},         "semget", "semtimedop")
