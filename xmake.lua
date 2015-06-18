
-- version
set_version("1.5.0")

-- set warning all as error
set_warnings("all", "error")

-- set language: c99, c++11
set_languages("c99", "cxx11")

-- add defines to config.h
add_defines_h("$(prefix)_OS_$(OS)")

-- add undefines to config.h 
add_undefines_h("$(prefix)_TRACE_INFO_ONLY")
add_undefines_h("$(prefix)_EXCEPTION_ENABLE")
add_undefines_h("$(prefix)_MEMORY_UNALIGNED_ACCESS_ENABLE")
 
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
        add_defines_h("$(prefix)_SMALL")
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

-- add option: demo
add_option("demo")
    set_option_enable(true)
    set_option_showmenu(true)
    set_option_category("option")
    set_option_description("Enable or disable the demo module")

-- add packages
add_subdirs("pkg/*.pkg") 

-- add projects
add_subdirs("src/tbox") 
if options("demo") then add_subdirs("src/demo") end
