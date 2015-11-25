
-- add type: wchar_t
add_option("wchar_t")
    add_option_ctypes("wchar_t")
    add_option_defines_h_if_ok("$(prefix)_TYPE_HAVE_WCHAR")

-- add option: float
add_option("float")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("option")
    set_option_description("Enable or disable the float type")
    add_option_defines_h_if_ok("$(prefix)_TYPE_HAVE_FLOAT")

-- add option: info
add_option("info")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("option")
    set_option_description("Enable or disable to get some info, .e.g version ..")
    add_option_defines_h_if_ok("$(prefix)_INFO_HAVE_VERSION")

-- add modules
for _, module in ipairs({"xml", "zip", "asio", "object", "charset", "database"}) do
    add_option(module)
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description(string.format("The %s module", module))
    add_option_defines_h_if_ok(string.format("$(prefix)_MODULE_HAVE_%s", module:upper()))
end

-- add target
add_target("tbox")

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

    -- add modules
    add_options("info", "float", "xml", "zip", "asio", "object", "charset", "database")

    -- add packages
    add_options("zlib", "mysql", "sqlite3", "openssl", "polarssl", "base")

    -- add interfaces
    add_options(libc, libm, posix, systemv)

    -- add the common source files
    add_files("*.c") 
    add_files("asio/aioo.c") 
    add_files("asio/aiop.c") 
    add_files("math/**.c") 
    add_files("libc/**.c|string/impl/**.c") 
    add_files("utils/*.c|option.c") 
    add_files("prefix/**.c") 
    add_files("memory/**.c") 
    add_files("string/**.c") 
    add_files("stream/**.c|**/charset.c|**/zip.c|**async_**.c|transfer_pool.c") 
    add_files("network/**.c|impl/ssl/*.c") 
    add_files("algorithm/**.c") 
    add_files("container/**.c") 
    add_files("libm/libm.c") 
    add_files("libm/idivi8.c") 
    add_files("libm/ilog2i.c") 
    add_files("libm/isqrti.c") 
    add_files("libm/isqrti64.c") 
    add_files("libm/idivi8.c") 
    add_files("platform/*.c|aicp.c")

    -- add the source files for arm
    if archs("arm.*") then
        add_files("utils/impl/crc_arm.S")
    end

    -- add the source files for the float type
    if options("float") then add_files("libm/*.c") end

    -- add the source files for the xml module
    if options("xml") then add_files("xml/**.c") end

    -- add the source files for the asio module
    if options("asio") then 
        add_files("asio/aico.c")
        add_files("asio/aicp.c")
        add_files("asio/http.c")
        add_files("asio/dns.c")
        add_files("stream/**async_**.c")
        add_files("stream/transfer_pool.c")
        add_files("platform/aicp.c")
        if options("openssl", "polarssl") then add_files("asio/ssl.c") end
    end

    -- add the source files for the object module
    if options("object") then 
        add_files("object/**.c|**/xml.c|**/xplist.c")
        add_files("utils/option.c")
        add_files("container/element/obj.c")
        if options("xml") then
            add_files("object/impl/reader/xml.c")
            add_files("object/impl/reader/xplist.c")
            add_files("object/impl/writer/xml.c")
            add_files("object/impl/writer/xplist.c")
        end
    end

    -- add the source files for the charset module
    if options("charset") then 
        add_files("charset/**.c")
        add_files("stream/impl/filter/charset.c")
    end

    -- add the source files for the zip module
    if options("zip") then 
        add_files("zip/**.c|gzip.c|zlib.c|zlibraw.c|lzsw.c")
        add_files("stream/impl/filter/zip.c")
        if options("zlib") then 
            add_files("zip/gzip.c") 
            add_files("zip/zlib.c") 
            add_files("zip/zlibraw.c") 
        end
    end

    -- add the source files for the database module
    if options("database") then 
        add_files("database/*.c")
        if options("mysql") then add_files("database/impl/mysql.c") end
        if options("sqlite3") then add_files("database/impl/sqlite3.c") end
    end

    -- add the source files for the ssl package
    if options("polarssl") then add_files("network/impl/ssl/polarssl.c") 
    elseif options("openssl") then add_files("network/impl/ssl/openssl.c") end

    -- add the source for the windows 
    if os("windows") then
        add_files("platform/windows/socket_pool.c")
        add_files("platform/windows/interface/*.c")
    end

    -- add the source for the ios 
    if os("ios") then
        add_files("platform/mach/ios/directory.m")
    end

    -- add the source for the android 
    if os("android") then
        add_files("platform/android/*.c")
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
    add_cfuncs("libc", nil,         {"signal.h", "setjmp.h"},           "signal", "setjmp", "sigsetjmp")
    add_cfuncs("libc", nil,         "execinfo.h",                       "backtrace")
    add_cfuncs("libc", nil,         "stdlib.h",                         "system")

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

    -- add the interfaces for systemv
    add_cfuncs("systemv", nil,      {"sys/sem.h", "sys/ipc.h"},         "semget", "semtimedop")
