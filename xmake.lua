
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
 
-- add defines for c files
add_defines("_GNU_SOURCE=1", "_REENTRANT")

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations")
add_mxflags("-Wno-error=deprecated-declarations")

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
    add_ldflags("-fsanitize=address")
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

-- for the windows platform (msvc)
if plats("windows") then 

    -- force to compile all c-files as c++ files 
    add_cxflags("-TP") 

    -- enable _cdecl
    add_cxflags("-Gd") 

    -- enable multi-processors for compiling
    add_cxflags("-MP4") 
    
    -- the warnings less than all
    set_warnings("more", "error")

    -- add some defines only for windows
    add_defines("_MBCS", "_CRT_SECURE_NO_WARNINGS", "NOCRYPT", "NOGDI")

    -- the release mode
    if modes("release") then

        -- link libcmt.lib
        add_cxflags("-MT") 

    -- the debug mode
    elseif modes("debug") then

        -- enable some checkers
        add_cxflags("-Gs", "-RTC1") 

        -- link libcmtd.lib
        add_cxflags("-MTd") 
    end

    -- no msvcrt.lib
    add_ldflags("-nodefaultlib:\"msvcrt.lib\"")

    -- add uac
    add_ldflags("-manifest", "-manifestuac:\"level='asInvoker' uiAccess='false'\"")
end

-- add type: wchar_t
add_option("wchar_t")
    set_option_category("type")
    add_option_ctypes("wchar_t")
    add_option_defines_h_if_ok("TB_CONFIG_TYPE_HAVE_WCHAR")

-- add option: float
add_option("float")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("option")
    set_option_description("Enable or disable the float type")
    add_option_defines_h_if_ok("TB_CONFIG_TYPE_HAVE_FLOAT")

-- add option: demo
add_option("demo")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("option")
    set_option_description("Enable or disable the demo module")

-- add modules
for _, module in ipairs({"xml", "zip", "asio", "object", "charset", "database"}) do
    add_option(module)
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("module")
    set_option_description(string.format("The %s module", module))
    add_option_defines_h_if_ok(string.format("TB_CONFIG_MODULE_HAVE_%s", module:upper()))
end

-- add module interfaces
function add_option_module_interfaces(module, links, includes, ...)

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
        if links then add_option_links(links) end
        if includes then add_option_cincludes(includes) end
        add_option_defines_h_if_ok(string.format("TB_CONFIG_%s_HAVE_%s", module:upper(), interface:upper()))

        -- add options 
        table.insert(_G[module], name)
    end
end

-- add module interfaces for libc.string
add_option_module_interfaces(   "libc"
                            ,   nil
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

-- add module interfaces for libc.wstring
add_option_module_interfaces(   "libc"
                            ,   nil
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

-- add module interfaces for libc
add_option_module_interfaces("libc", nil, "time.h", "gmtime", "mktime", "localtime")
add_option_module_interfaces("libc", nil, "sys/time.h", "gettimeofday")
add_option_module_interfaces("libc", nil, {"signal.h", "setjmp.h"}, "signal", "setjmp", "sigsetjmp")
add_option_module_interfaces("libc", nil, "execinfo.h", "backtrace")
add_option_module_interfaces("libc", nil, "stdlib.h", "system")

-- add module interfaces for libm
add_option_module_interfaces("libm", nil, "math.h", "sincos", "sincosf", "log2", "log2f")

-- add module interfaces for posix
add_option_module_interfaces("posix", nil, {"sys/poll.h", "sys/socket.h"}, "poll")
add_option_module_interfaces("posix", "pthread", "pthread.h", "pthread_mutex_init", "pthread_create")
add_option_module_interfaces("posix", nil, {"sys/socket.h", "fcntl.h"}, "socket")
add_option_module_interfaces("posix", nil, "dirent.h", "opendir")
add_option_module_interfaces("posix", nil, "dlfcn.h", "dlopen")
add_option_module_interfaces("posix", nil, {"sys/stat.h", "fcntl.h"}, "open")
add_option_module_interfaces("posix", nil, "unistd.h", "gethostname")
add_option_module_interfaces("posix", nil, "ifaddrs.h", "getifaddrs")
add_option_module_interfaces("posix", "pthread", "semaphore.h", "sem_init")
add_option_module_interfaces("posix", nil, "unistd.h", "getpagesize", "sysconf")
add_option_module_interfaces("posix", nil, "sched.h", "sched_yield")

-- add module interfaces for systemv
add_option_module_interfaces("systemv", nil, {"sys/sem.h", "sys/ipc.h"}, "semget", "semtimedop")

-- add packages
add_subdirs("pkg/*.pkg") 

-- add projects
add_subdirs("src/tbox") 
if options("demo") then add_subdirs("src/demo") end
