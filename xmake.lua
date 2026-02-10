set_project("tbox")
set_xmakever("3.0.5")
set_policy("build.progress_style", "multirow")
set_version("1.8.0", {build = "%Y%m%d", soname = true})
set_warnings("all", "error")

-- set language: c99
stdc = "c99"
set_languages(stdc)

-- add defines to config.h
set_configvar("_GNU_SOURCE", 1)
set_configvar("_REENTRANT", 1)

-- ensure POSIX/XOPEN features are available on Solaris (for setenv, unsetenv, clock_gettime, etc.)
-- _XOPEN_SOURCE=600 implicitly sets _POSIX_C_SOURCE to 200112L
if is_plat("solaris") then
    add_defines("_XOPEN_SOURCE=600")
end

-- add module directories
add_moduledirs("xmake")

-- disable some compiler errors
add_cxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined", "-Wno-error=empty-body")
add_mxflags("-Wno-error=deprecated-declarations", "-fno-strict-aliasing", "-Wno-error=expansion-to-defined", "-Wno-error=empty-body")
if has_config("coroutine") then
    -- https://github.com/tboox/tbox/issues/218
    add_cxflags("gcc::-Wno-error=dangling-pointer")
end

-- set wasm toolchain
if is_plat("wasm") then
    add_requires("emscripten")
    set_toolchains("emcc@emscripten")
end

-- set cosmocc toolchain, e.g. xmake f -p linux --cosmocc=y
if has_config("cosmocc") then
    add_requires("cosmocc")
    set_toolchains("@cosmocc")
    set_policy("build.ccache", false)
end

-- add build modes
add_rules("mode.release", "mode.debug", "mode.profile", "mode.coverage", "mode.valgrind", "mode.asan", "mode.tsan", "mode.ubsan")
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
    if is_mode("release", "profile") and
        -- coroutine maybe crash if we enable lto on windows, we disable small mode.
        -- TODO we should fix it in context code later
        -- https://github.com/tboox/tbox/issues/175
        not has_config("coroutine") then
        if is_plat("windows") then
            -- we cannot use smallest(/O1), it maybe generates incorrect code for msvc2022
            -- @see https://github.com/tboox/tbox/issues/272
            set_optimize("fastest")
        else
            set_optimize("smallest")
        end
    end
    add_cxflags("-fno-stack-protector")
end

-- for the windows platform (msvc)
if is_plat("windows") then
    add_defines("NOCRYPT", "NOGDI")
    if is_mode("debug") then
        add_cxflags("-Gs", "-RTC1")
        set_runtimes("MTd")
    else
        set_runtimes("MT")
    end
    add_syslinks("ws2_32", "user32")
elseif is_plat("android") then
    add_syslinks("m", "c")
elseif is_plat("mingw", "msys", "cygwin") then
    add_syslinks("ws2_32", "user32", "pthread", "m")
elseif is_plat("haiku") then
    add_syslinks("pthread", "network", "m", "c")
else
    add_syslinks("pthread", "dl", "m", "c")
end

-- enable backtrace symbols for linux
if is_plat("linux") and is_mode("debug") then
    add_ldflags("-rdynamic")
end

-- include project sources
includes("src")
