
-- add target
target("demo")

    -- add the dependent target
    add_deps("tbox")

    -- make as a binary
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"demo\"")

    -- set the object files directory
    set_objectdir("$(buildir)/.objs")

    -- add links directory
    add_linkdirs("$(buildir)")

    -- add includes directory
    add_includedirs("$(buildir)")
    add_includedirs("$(buildir)/tbox")

    -- add links
    add_links("tbox")

    -- add packages
    add_packages("zlib", "mysql", "sqlite3", "pcre", "pcre2", "openssl", "polarssl", "base")

    -- add the source files
    add_files("*.c") 
    add_files("libc/*.c") 
    add_files("libm/integer.c") 
    add_files("math/random.c") 
    add_files("utils/*.c|option.c") 
    add_files("other/*.c|charset.c") 
    add_files("string/*.c") 
    add_files("memory/**.c") 
    add_files("platform/*.c|thread*.c|semaphore.c|event.c|lock.c|timer.c|ltimer.c|exception.c") 
    add_files("container/*.c") 
    add_files("algorithm/*.c") 
    add_files("stream/stream.c") 
    add_files("stream/stream/*.c") 

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

    -- add the source files for the thread type
    if is_option("thread") then
        add_files("platform/thread*.c") 
        add_files("platform/event.c") 
        add_files("platform/lock.c") 
        add_files("platform/timer.c") 
        add_files("platform/ltimer.c") 
        add_files("platform/exception.c") 
        add_files("platform/semaphore.c") 
    end

    -- add the source files for the xml module
    if is_option("xml") then
        add_files("xml/*.c")
    end

    -- add the source files for the regex module
    if is_option("regex") then
        add_files("regex/*.c")
    end

    -- add the source files for the network type
    if is_option("network") then
        add_files("network/**.c") 
    end

    -- add the source files for the asio module
    if is_option("asio") then
        add_files("asio/*.c")
        add_files("stream/async_stream.c")
        add_files("stream/transfer_pool.c")
        add_files("stream/async_transfer.c")
        add_files("stream/async_stream/*.c")
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
    
