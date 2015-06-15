
-- version
set_version("1.5.0")

-- set warning all as error
set_warnings("all", "error")

-- set language: c99, c++11
set_languages("c99", "cxx11")

-- add defines to config.h
add_defines_h("TB_CONFIG_OS_$(OS)")

-- add undefines to config.h 
add_undefines_h("TB_CONFIG_TRACE_INFO_ONLY")
add_undefines_h("TB_CONFIG_EXCEPTION_ENABLE")
add_undefines_h("TB_CONFIG_MEMORY_UNALIGNED_ACCESS_ENABLE")

-- the debug mode
if modes("debug") then
    
    -- enable the debug symbols
    set_symbols("debug")

    -- disable optimization
    set_optimize("none")

    -- add defines for debug
    add_defines("__tb_debug__")

    -- attempt to enable some checkers
    add_cxflags("-fsanitize=address", "-ftrapv")
    add_mxflags("-fsanitize=address", "-ftrapv")
end

-- the release or profile modes
if modes("release", "profile") then

    -- the release mode
    if modes("release") then
        
        -- set the symbols visibility: hidden
        set_symbols("hidden")

        -- strip all symbols
        set_strip("all")

        -- fomit the frame pointer
        add_cxflags("-fomit-frame-pointer")
        add_mxflags("-fomit-frame-pointer")

    -- the profile mode
    else
    
        -- enable the debug symbols
        set_symbols("debug")

    end

    -- for pc
    if archs("i386", "x86_64") then
 
        -- enable fastest optimization
        set_optimize("fastest")

    -- for embed
    else
        -- enable smallest optimization
        set_optimize("smallest")

        -- add defines for small
        add_defines("__tb_small__")

        -- add defines to config.h
        add_defines_h("TB_CONFIG_SMALL")
    end

    -- attempt to add vector extensions 
    add_vectorexts("sse2", "sse3", "ssse3", "mmx")
end

-- add module: demo
add_option("demo")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the demo module")

-- add module: float
add_option("float")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the float type")
    add_option_defines_h_if_ok("TB_CONFIG_TYPE_FLOAT")

-- add module: xml
add_option("xml")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the xml module")
    add_option_defines_h_if_ok("TB_CONFIG_MODULE_HAVE_XML")

-- add module: xml
add_option("zip")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the zip module")
    add_option_defines_h_if_ok("TB_CONFIG_MODULE_HAVE_ZIP")

-- add module: asio
add_option("asio")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the asio module")
    add_option_defines_h_if_ok("TB_CONFIG_MODULE_HAVE_ASIO")

-- add module: object
add_option("object")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the object module")
    add_option_defines_h_if_ok("TB_CONFIG_MODULE_HAVE_OBJECT")

-- add module: charset
add_option("charset")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the charset module")
    add_option_defines_h_if_ok("TB_CONFIG_MODULE_HAVE_CHARSET")

-- add module: database
add_option("database")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description("Enable or disable the database module")
    add_option_defines_h_if_ok("TB_CONFIG_MODULE_HAVE_DATABASE")

-- add package: zlib
add_option("zlib")
    set_option_showmenu(true)
    set_option_category("package")
    set_option_description("Enable or disable the zlib package")
    add_option_defines_h_if_ok("TB_CONFIG_PACKAGE_HAVE_ZLIB")
    add_option_links("z")
    add_option_linkdirs("pkg/zlib.pkg/lib/$(plat)/$(arch)")
    add_option_cincludes("zlib/zlib.h")
    add_option_includedirs("pkg/zlib.pkg/inc", "pkg/zlib.pkg/inc/$(plat)/$(arch)")

-- add package: mysql
add_option("mysql")
    set_option_showmenu(true)
    set_option_category("package")
    set_option_description("Enable or disable the mysql package")
    add_option_defines_h_if_ok("TB_CONFIG_PACKAGE_HAVE_MYSQL")
    add_option_links("mysqlclient")
    add_option_linkdirs("pkg/mysql.pkg/lib/$(plat)/$(arch)")
    add_option_cincludes("mysql/mysql.h")
    add_option_includedirs("pkg/mysql.pkg/inc", "pkg/mysql.pkg/inc/$(plat)/$(arch)")

-- add package: sqlite3
add_option("sqlite3")
    set_option_showmenu(true)
    set_option_category("package")
    set_option_description("Enable or disable the sqlite3 package")
    add_option_defines_h_if_ok("TB_CONFIG_PACKAGE_HAVE_SQLITE3")
    add_option_links("sqlite3")
    add_option_linkdirs("pkg/sqlite3.pkg/lib/$(plat)/$(arch)")
    add_option_cincludes("sqlite3/sqlite3.h")
    add_option_includedirs("pkg/sqlite3.pkg/inc", "pkg/sqlite3.pkg/inc/$(plat)/$(arch)")

-- add package: openssl
add_option("openssl")
    set_option_showmenu(true)
    set_option_category("package")
    set_option_description("Enable or disable the openssl package")
    add_option_defines_h_if_ok("TB_CONFIG_PACKAGE_HAVE_OPENSSL")
    add_option_links("ssl", "crypto")
    add_option_linkdirs("pkg/openssl.pkg/lib/$(plat)/$(arch)")
    add_option_cincludes("openssl/openssl.h")
    add_option_includedirs("pkg/openssl.pkg/inc", "pkg/openssl.pkg/inc/$(plat)/$(arch)")

    -- disable some compiler errors for macosx
    if plats("macosx") then
        add_option_cxflags("-Wno-error=deprecated-declarations")
        add_option_mxflags("-Wno-error=deprecated-declarations")
    end

-- add package: polarssl
add_option("polarssl")
    set_option_showmenu(true)
    set_option_category("package")
    set_option_description("Enable or disable the polarssl package")
    add_option_defines_h_if_ok("TB_CONFIG_PACKAGE_HAVE_POLARSSL")
    add_option_links("polarssl")
    add_option_linkdirs("pkg/polarssl.pkg/lib/$(plat)/$(arch)")
    add_option_cincludes("polarssl/polarssl.h")
    add_option_includedirs("pkg/polarssl.pkg/inc", "pkg/polarssl.pkg/inc/$(plat)/$(arch)")

-- add module interfaces
function add_option_module_interfaces(module, includes, ...)

    -- make module
    _G[module] = _G[module] or {}

    -- init interfaces
    local options = {}
    for _, interface in ipairs({...}) do

        -- the options name
        local name = string.format("__%s_%s", module, interface)

        -- add options
        add_option(name)
        set_option_category("interface")
        add_option_cfuncs(interface)
        add_option_cincludes(includes)
        add_option_defines_h_if_ok(string.format("TB_CONFIG_%s_HAVE_%s", module:upper(), interface:upper()))

        -- add options to the libc
        table.insert(_G[module], name)
    end
end

-- add module interfaces for libc.string
add_option_module_interfaces(   "libc"
                            ,   {"string.h", "stdlib.h"}
                            ,   "memcpy"
                            ,   "memset"
                            ,   "memmove"
                            ,   "memcmp"
                            ,   "strcat"
                            ,   "strncat"
                            ,   "strcpy"
                            ,   "strncpy"
                            ,   "strlcpy"
                            ,   "strlen"
                            ,   "strnlen"
                            ,   "strstr"
                            ,   "strcasestr"
                            ,   "strcmp"
                            ,   "strcasecmp"
                            ,   "strncmp"
                            ,   "strncasecmp")

-- add module interfaces for libc.wchar
add_option_module_interfaces(   "libc"
                            ,   {"wchar.h", "stdlib.h"}
                            ,   "wcscat"
                            ,   "wcsncat"
                            ,   "wcscpy"
                            ,   "wcsncpy"
                            ,   "wcslcpy"
                            ,   "wcslen"
                            ,   "wcsnlen"
                            ,   "wcsstr"
                            ,   "wcscasestr"
                            ,   "wcscmp"
                            ,   "wcscasecmp"
                            ,   "wcsncmp"
                            ,   "wcsncasecmp"
                            ,   "wcstombs"
                            ,   "mbstowcs")

-- add module interfaces for libc.time
add_option_module_interfaces(   "libc"
                            ,   "time.h"
                            ,   "gmtime"
                            ,   "mktime"
                            ,   "localtime")

-- add module interfaces for libc.time
add_option_module_interfaces(   "libc"
                            ,   "sys/time.h"
                            ,   "gettimeofday")

-- add module interfaces for libc.signal
add_option_module_interfaces(   "libc"
                            ,   {"signal.h", "setjmp.h"}
                            ,   "signal"
                            ,   "setjmp"
                            ,   "sigsetjmp")

-- add module interfaces for libc.execinfo
add_option_module_interfaces(   "libc"
                            ,   "execinfo.h"
                            ,   "backtrace")

-- add module interfaces for libc.system
add_option_module_interfaces(   "libc"
                            ,   "stdlib.h"
                            ,   "system")

-- add module interfaces for libm
add_option_module_interfaces(   "libm"
                            ,   "math.h"
                            ,   "sincos"
                            ,   "sincosf"
                            ,   "log2"
                            ,   "log2f")

-- add module interfaces for posix.poll
add_option_module_interfaces(   "posix"
                            ,   {"sys/poll.h", "sys/socket.h"}
                            ,   "poll")

-- add module interfaces for posix.pthread
add_option_module_interfaces(   "posix"
                            ,   "pthread.h"
                            ,   "pthread_mutex_init"
                            ,   "pthread_create")

-- add module interfaces for posix.socket
add_option_module_interfaces(   "posix"
                            ,   {"sys/socket.h", "fcntl.h"}
                            ,   "socket")

-- add module interfaces for posix.dirent
add_option_module_interfaces(   "posix"
                            ,   "dirent.h"
                            ,   "opendir")

-- add module interfaces for posix.dlopen
add_option_module_interfaces(   "posix"
                            ,   "dlfcn.h"
                            ,   "dlopen")

-- add module interfaces for posix.file
add_option_module_interfaces(   "posix"
                            ,   {"sys/stat.h", "fcntl.h"}
                            ,   "open")

-- add module interfaces for posix.gethostname
add_option_module_interfaces(   "posix"
                            ,   "unistd.h"
                            ,   "gethostname")

-- add module interfaces for posix.ifaddrs
add_option_module_interfaces(   "posix"
                            ,   "ifaddrs.h"
                            ,   "getifaddrs")

-- add module interfaces for posix.semaphore
add_option_module_interfaces(   "posix"
                            ,   "semaphore.h"
                            ,   "sem_init")

-- add module interfaces for posix.page
add_option_module_interfaces(   "posix"
                            ,   "unistd.h"
                            ,   "getpagesize"
                            ,   "sysconf")

-- add module interfaces for posix.sched
add_option_module_interfaces(   "posix"
                            ,   "sched.h"
                            ,   "sched_yield")

-- add module interfaces for systemv.semaphore
add_option_module_interfaces(   "systemv"
                            ,   {"sys/sem.h", "sys/ipc.h"}
                            ,   "semget"
                            ,   "semtimedop")

-- projects
add_subdirs("src/tbox") 
if options("demo") then add_subdirs("src/demo") end
