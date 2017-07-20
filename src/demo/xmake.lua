
-- add target
target("demo")

    -- add the dependent target
    add_deps("tbox")

    -- make as a binary
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"demo\"")

    -- set the object files directory
    set_objectdir("$(buildir)/$(mode)/$(arch)/.objs")
    set_targetdir("$(buildir)/$(mode)/$(arch)")

    -- add packages
    add_options("zlib", "mysql", "sqlite3", "pcre", "pcre2", "openssl", "polarssl", "mbedtls", "base")

    -- add the source files
    add_files("demo.c") 
    add_files("libc/*.c") 
    add_files("libm/integer.c") 
    add_files("math/random.c") 
    add_files("utils/*.c|option.c") 
    add_files("other/*.c|charset.c") 
    add_files("string/*.c") 
    add_files("memory/**.c") 
    add_files("platform/*.c|exception.c|context.c") 
    add_files("container/*.c") 
    add_files("algorithm/*.c") 
    add_files("stream/stream.c") 
    add_files("stream/stream/*.c") 
    add_files("network/**.c") 

    -- add the source files for the hash module
    if is_option("hash") then
        add_files("hash/*.c") 
    end

    -- add the source files for the float type
    if is_option("float") then
        add_files("math/fixed.c")
        add_files("libm/float.c")
        add_files("libm/double.c")
    end

    -- add the source files for the coroutine module
    if is_option("coroutine") then
        add_files("coroutine/**.c|spider.c") 
        add_files("platform/context.c") 
        if is_option("xml") then
            add_files("coroutine/spider.c") 
        end
    end

    -- add the source files for the exception module
    if is_option("exception") then
        add_files("platform/exception.c") 
    end

    -- add the source files for the xml module
    if is_option("xml") then
        add_files("xml/*.c")
    end

    -- add the source files for the regex module
    if is_option("regex") then
        add_files("regex/*.c")
    end

    -- add the source files for the asio module
    if is_option("deprecated") then
        add_files("asio/deprecated/*.c")
        add_files("stream/deprecated/async_stream.c")
        add_files("stream/deprecated/transfer_pool.c")
        add_files("stream/deprecated/async_transfer.c")
        add_files("stream/deprecated/async_stream/*.c")
    end

    -- add the source files for the object module
    if is_option("object") then
        add_files("utils/option.c")
        add_files("object/*.c")
    end

    -- add the source files for the charset module
    if is_option("charset") then add_files("other/charset.c") end

    -- add the source files for the database module
    if is_option("database") then add_files("database/sql.c") end
    
