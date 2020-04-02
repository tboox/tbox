-- project
set_project("tbox")

-- set xmake minimum version
set_xmakever("2.2.5")

-- set project version
set_version("1.6.5", {build = "%Y%m%d%H%M"})

-- set warning all as error
set_warnings("all", "error")

-- set language: c99
stdc = "c99"
set_languages(stdc)

-- add defines to config.h
set_configvar("_GNU_SOURCE", 1)
set_configvar("_REENTRANT", 1)

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined")
add_mxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined")

-- add build modes
add_rules("mode.release", "mode.debug", "mode.coverage", "mode.valgrind", "mode.asan", "mode.tsan")
if is_mode("debug") then
    add_defines("__tb_debug__")
end
if is_mode("valgrind") then
    add_defines("__tb_valgrind__")
end
if is_mode("asan") then
    add_defines("__tb_sanitize_address__")
end
if is_mode("tsan") then
    add_defines("__tb_sanitize_thread__")
end

-- small or micro?
if has_config("small", "micro") then
    add_defines("__tb_small__")
    set_configvar("TB_CONFIG_SMALL", 1)
    if is_mode("release", "profile") then
        set_optimize("smallest")
    end
    add_cxflags("-fno-stack-protector")
end

-- for the windows platform (msvc)
if is_plat("windows") then 

    -- add some defines only for windows
    add_defines("NOCRYPT", "NOGDI")

    -- the release mode
    if is_mode("release") then

        -- link libcmt.lib
        add_cxflags("-MT") 

    -- the debug mode
    elseif is_mode("debug") then

        -- enable some checkers
        add_cxflags("-Gs", "-RTC1") 

        -- link libcmtd.lib
        add_cxflags("-MTd") 
    end

    -- no msvcrt.lib
    add_ldflags("-nodefaultlib:msvcrt.lib")
    add_syslinks("ws2_32") 

elseif is_plat("android") then
    add_syslinks("m", "c") 
elseif is_plat("mingw", "msys", "cygwin") then
    add_syslinks("ws2_32", "pthread", "m")
else 
    add_syslinks("pthread", "dl", "m", "c") 
end

-- enable backtrace symbols for linux 
if is_plat("linux") and is_mode("debug") then
    add_ldflags("-rdynamic")
end

-- include project sources
includes("src") 
