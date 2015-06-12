
-- version
set_version("1.5.0")

-- set warning all as error
set_warnings("all", "error")

-- TODO for c++
-- set language: c99
set_language("c99")

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
    end

    -- attempt to add vector extensions 
    add_vectorexts("sse2", "sse3", "ssse3", "mmx")
end

-- the macosx platform
if plats("macosx") then

    -- disable some compiler errors
    add_cxflags("-Wno-error=deprecated-declarations")
    add_mxflags("-Wno-error=deprecated-declarations")

end

-- add module: xml
add_option("xml")
    set_option_default(true)
    add_option_defines_if_ok("TB_CONFIG_MODULE_HAVE_XML")
    set_option_description("The module: xml")

-- add module: xml
add_option("zip")
    set_option_default(true)
    add_option_defines_if_ok("TB_CONFIG_MODULE_HAVE_ZIP")
    set_option_description("The module: zip")

-- add module: asio
add_option("asio")
    set_option_default(true)
    add_option_defines_if_ok("TB_CONFIG_MODULE_HAVE_ASIO")
    set_option_description("The module: asio")

-- add module: object
add_option("object")
    set_option_default(true)
    add_option_defines_if_ok("TB_CONFIG_MODULE_HAVE_OBJECT")
    set_option_description("The module: object")

-- add module: charset
add_option("charset")
    set_option_default(true)
    add_option_defines_if_ok("TB_CONFIG_MODULE_HAVE_CHARSET")
    set_option_description("The module: charset")

-- add module: database
add_option("database")
    set_option_default(true)
    add_option_defines_if_ok("TB_CONFIG_MODULE_HAVE_DATABASE")
    set_option_description("The module: database")

-- projects
add_subdirs("src/tbox", "src/demo")
