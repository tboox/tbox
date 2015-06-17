
-- add target
add_target("tbox")

    -- make as a static library
    set_kind("static")

    -- add defines
    add_defines("__tb_prefix__=\"tbox\"")

    -- set the auto-generated config.h
    set_config_h("$(buildir)/tbox.pkg/inc/$(plat)/$(arch)/tbox.config.h")
    set_config_h_prefix("TB")

    -- set the target directory
    set_targetdir("$(buildir)/tbox.pkg/lib/$(plat)/$(arch)")

    -- set the header files directory
    set_headerdir("$(buildir)/tbox.pkg/inc")

    -- set the object files directory
    set_objectdir("$(buildir)/.objs")

    -- add includes directory
    add_includedirs("$(buildir)/tbox.pkg/inc/$(plat)/$(arch)")

    -- add the header files for installing
    add_headers("src/(tbox/**.h)")
    add_headers("src/(tbox/prefix/**/prefix.S)")

    -- add modules
    add_options("float", "xml", "zip", "asio", "object", "charset", "database")

    -- add packages
    add_options("zlib", "mysql", "sqlite3", "openssl", "polarssl")

    -- add interfaces
    add_options(libc, libm, posix, systemv)

    -- add the common source files
    add_files("src/tbox/*.c") 
    add_files("src/tbox/asio/aioo.c") 
    add_files("src/tbox/asio/aiop.c") 
    add_files("src/tbox/math/**.c") 
    add_files("src/tbox/libc/**.c|string/impl/**.c") 
    add_files("src/tbox/utils/*.c|option.c") 
    add_files("src/tbox/prefix/**.c") 
    add_files("src/tbox/memory/**.c") 
    add_files("src/tbox/string/**.c") 
    add_files("src/tbox/stream/**.c|**/charset.c|**/zip.c|**async_**.c|transfer_pool.c") 
    add_files("src/tbox/network/**.c|impl/ssl/*.c") 
    add_files("src/tbox/algorithm/**.c") 
    add_files("src/tbox/container/**.c") 
    add_files("src/tbox/libm/libm.c") 
    add_files("src/tbox/libm/idivi8.c") 
    add_files("src/tbox/libm/ilog2i.c") 
    add_files("src/tbox/libm/isqrti.c") 
    add_files("src/tbox/libm/isqrti64.c") 
    add_files("src/tbox/libm/idivi8.c") 
    add_files("src/tbox/platform/*.c|aicp.c")

    -- add the source files for arm
    if archs("arm.*") then
        add_files("src/tbox/utils/impl/crc_arm.S")
    end

    -- add the source files for the float type
    if options("float") then add_files("src/tbox/libm/*.c") end

    -- add the source files for the xml module
    if options("xml") then add_files("src/tbox/xml/**.c") end

    -- add the source files for the asio module
    if options("asio") then 
        add_files("src/tbox/asio/aico.c")
        add_files("src/tbox/asio/aicp.c")
        add_files("src/tbox/asio/http.c")
        add_files("src/tbox/asio/dns.c")
        add_files("src/tbox/stream/**async_**.c")
        add_files("src/tbox/stream/transfer_pool.c")
        add_files("src/tbox/platform/aicp.c")
        if options("openssl", "polarssl") then add_files("src/tbox/asio/ssl.c") end
    end

    -- add the source files for the object module
    if options("object") then 
        add_files("src/tbox/object/**.c|**/xml.c|**/xplist.c")
        add_files("src/tbox/utils/option.c")
        add_files("src/tbox/container/element/obj.c")
        if options("xml") then
            add_files("src/tbox/object/impl/reader/xml.c")
            add_files("src/tbox/object/impl/reader/xplist.c")
            add_files("src/tbox/object/impl/writer/xml.c")
            add_files("src/tbox/object/impl/writer/xplist.c")
        end
    end

    -- add the source files for the charset module
    if options("charset") then 
        add_files("src/tbox/charset/**.c")
        add_files("src/tbox/stream/impl/filter/charset.c")
    end

    -- add the source files for the zip module
    if options("zip") then 
        add_files("src/tbox/zip/**.c|gzip.c|zlib.c|zlibraw.c|lzsw.c")
        add_files("src/tbox/stream/impl/filter/zip.c")
        if options("zlib") then 
            add_files("src/tbox/zip/gzip.c") 
            add_files("src/tbox/zip/zlib.c") 
            add_files("src/tbox/zip/zlibraw.c") 
        end
    end

    -- add the source files for the database module
    if options("database") then 
        add_files("src/tbox/database/*.c")
        if options("mysql") then add_files("src/tbox/database/impl/mysql.c") end
        if options("sqlite3") then add_files("src/tbox/database/impl/sqlite3.c") end
    end

    -- add the source files for the ssl package
    if options("polarssl") then add_files("src/tbox/network/impl/ssl/polarssl.c") 
    elseif options("openssl") then add_files("src/tbox/network/impl/ssl/openssl.c") end

    -- add the source for the windows 
    if os("windows") then
        add_files("src/tbox/platform/windows/socket_pool.c")
        add_files("src/tbox/platform/windows/interface/*.c")
    end

    -- add the source for the ios 
    if os("ios") then
        add_files("src/tbox/platform/mach/ios/directory.m")
    end

    -- add the source for the android 
    if os("android") then
        add_files("src/tbox/platform/linux/android/*.c")
    end
