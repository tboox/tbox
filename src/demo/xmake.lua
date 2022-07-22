
-- add target
target("demo")

    -- add the dependent target
    add_deps("tbox")

    -- make as a binary
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"demo\"")

    -- add the source files
    add_files("demo.c")
    add_files("libc/*.c")
    add_files("libm/integer.c")
    add_files("math/random.c")
    add_files("utils/*.c|option.c")
    add_files("other/*.c|charset.c", "other/*.cpp")
    add_files("string/*.c")
    add_files("memory/**.c")
    add_files("platform/*.c|exception.c|context.c")
    add_files("container/*.c")
    add_files("algorithm/*.c")
    add_files("stream/stream.c")
    add_files("stream/stream/*.c")
    add_files("network/**.c")

    -- add the source files for the hash module
    if has_config("hash") then
        add_files("hash/*.c")
    end

    -- add the source files for the float type
    if has_config("float") then
        add_files("math/fixed.c")
        add_files("libm/float.c")
        add_files("libm/double.c")
    end

    -- add the source files for the coroutine module
    if has_config("coroutine") then
        add_files("coroutine/**.c")
        add_files("platform/context.c")
    end

    -- add the source files for the exception module
    if has_config("exception") then
        add_files("platform/exception.c")
    end

    -- add the source files for the xml module
    if has_config("xml") then
        add_files("xml/*.c")
    end

    -- add the source files for the regex module
    if has_config("regex") then
        add_files("regex/*.c")
    end

    -- add the source files for the object module
    if has_config("object") then
        add_files("utils/option.c")
        add_files("object/*.c")
    end

    -- enable utf8?
    add_options("force-utf8")

    -- add the source files for the charset module
    if has_config("charset") then add_files("other/charset.c") end

    -- add the source files for the database module
    if has_config("database") then add_files("database/sql.c") end

    -- enable xp compatibility mode
    if is_plat("windows") then
        if is_arch("x86") then
            add_ldflags("/subsystem:console,5.01")
        else
            add_ldflags("/subsystem:console,5.02")
        end
    end

    -- link mingw/libgcc
    if is_plat("mingw", "msys", "cygwin") then
        add_ldflags("-static-libgcc", {force = true})
    end

    -- link backtrace/execinfo for bsd
    if is_plat("bsd") then
        add_syslinks("execinfo")
    end

