
-- add target
add_target("demo")

    -- add the dependent target
    add_deps("tbox")

    -- make as a binary
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"demo\"")

    -- set the object files directory
    set_objectdir("$(buildir)/.objs")

    -- add links directory
    add_linkdirs("$(buildir)/tbox.pkg/lib/$(plat)/$(arch)")

    -- add includes directory
    add_includedirs("$(buildir)/tbox.pkg/inc")
    add_includedirs("$(buildir)/tbox.pkg/inc/$(plat)/$(arch)")

    -- add links
    add_links("tbox")

    -- add packages
    add_options("zlib", "mysql", "sqlite3", "openssl", "polarssl", "base")

    -- add the source files
    add_files("*.c") 
    add_files("libc/*.c") 
    add_files("libm/integer.c") 
    add_files("math/random.c") 
    add_files("utils/*.c|option.c") 
    add_files("other/*.c|charset.c") 
    add_files("string/*.c") 
    add_files("memory/**.c") 
    add_files("network/**.c") 
    add_files("platform/*.c") 
    add_files("container/*.c") 
    add_files("algorithm/*.c") 
    add_files("stream/stream.c") 
    add_files("stream/stream/*.c") 

    -- add the source files for the float type
    if options("float") then
        add_files("math/fixed.c")
        add_files("libm/float.c")
        add_files("libm/double.c")
    end

    -- add the source files for the xml module
    if options("xml") then
        add_files("xml/*.c")
    end

    -- add the source files for the asio module
    if options("asio") then
        add_files("asio/*.c")
        add_files("stream/async_stream.c")
        add_files("stream/transfer_pool.c")
        add_files("stream/async_transfer.c")
        add_files("stream/async_stream/*.c")
    end

    -- add the source files for the object module
    if options("object") then
        add_files("utils/option.c")
        add_files("object/*.c")
    end

    -- add the source files for the charset module
    if options("charset") then add_files("other/charset.c") end

    -- add the source files for the database module
    if options("database") then add_files("database/sql.c") end
    
