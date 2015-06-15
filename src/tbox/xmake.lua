
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
    add_files("src/tbox/libc/*.c") 
    add_files("src/tbox/libc/misc/**.c") 
    add_files("src/tbox/libc/stdio/**.c") 
    add_files("src/tbox/libc/stdlib/**.c") 
    add_files("src/tbox/libc/string/*.c") 
    add_files("src/tbox/utils/bits.c") 
    add_files("src/tbox/utils/md5.c") 
    add_files("src/tbox/utils/sha.c") 
    add_files("src/tbox/utils/crc.c") 
    add_files("src/tbox/utils/url.c") 
    add_files("src/tbox/utils/dump.c") 
    add_files("src/tbox/utils/used.c") 
    add_files("src/tbox/utils/trace.c") 
    add_files("src/tbox/utils/base*.c") 
    add_files("src/tbox/utils/singleton.c") 
    add_files("src/tbox/utils/lock_profiler.c") 
    add_files("src/tbox/prefix/**.c") 
    add_files("src/tbox/memory/**.c") 
    add_files("src/tbox/string/**.c") 
    add_files("src/tbox/stream/stream.c") 
    add_files("src/tbox/stream/filter.c") 
    add_files("src/tbox/stream/transfer.c") 
    add_files("src/tbox/stream/static_stream.c") 
    add_files("src/tbox/stream/impl/stream/*.c") 
    add_files("src/tbox/stream/impl/filter/*.c") 
    add_files("src/tbox/network/*.c") 
    add_files("src/tbox/network/dns/*.c") 
    add_files("src/tbox/network/impl/http/*.c") 
    add_files("src/tbox/algorithm/**.c") 
    add_files("src/tbox/container/**.c") 
    add_files("src/tbox/libm/libm.c") 
    add_files("src/tbox/libm/idivi8.c") 
    add_files("src/tbox/libm/ilog2i.c") 
    add_files("src/tbox/libm/isqrti.c") 
    add_files("src/tbox/libm/isqrti64.c") 
    add_files("src/tbox/libm/idivi8.c") 
    add_files("src/tbox/platform/dns.c")
    add_files("src/tbox/platform/page.c")
    add_files("src/tbox/platform/path.c")
    add_files("src/tbox/platform/file.c")
    add_files("src/tbox/platform/time.c")
    add_files("src/tbox/platform/sched.c")
    add_files("src/tbox/platform/timer.c")
    add_files("src/tbox/platform/ltimer.c")
    add_files("src/tbox/platform/print.c")
    add_files("src/tbox/platform/mutex.c")
    add_files("src/tbox/platform/event.c")
    add_files("src/tbox/platform/socket.c")
    add_files("src/tbox/platform/thread.c")
    add_files("src/tbox/platform/memory.c")
    add_files("src/tbox/platform/process.c")
    add_files("src/tbox/platform/dynamic.c")
    add_files("src/tbox/platform/atomic64.c")
    add_files("src/tbox/platform/ifaddrs.c")
    add_files("src/tbox/platform/platform.c")
    add_files("src/tbox/platform/hostname.c")
    add_files("src/tbox/platform/processor.c")
    add_files("src/tbox/platform/directory.c")
    add_files("src/tbox/platform/semaphore.c")
    add_files("src/tbox/platform/backtrace.c")
    add_files("src/tbox/platform/cache_time.c")
    add_files("src/tbox/platform/thread_pool.c")
    add_files("src/tbox/platform/environment.c")
    add_files("src/tbox/platform/thread_store.c")
    add_files("src/tbox/platform/aioo.c")
    add_files("src/tbox/platform/aiop.c")

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
        add_files("src/tbox/stream/async_stream.c")
        add_files("src/tbox/stream/impl/async_stream/*.c")
        add_files("src/tbox/stream/async_transfer.c")
        add_files("src/tbox/stream/transfer_pool.c")
        add_files("src/tbox/platform/aicp.c")
        if options("openssl", "polarssl") then add_files("src/tbox/asio/ssl.c") end
    end

    -- add the source files for the object module
    if options("object") then 
        add_files("src/tbox/object/*.c")
        add_files("src/tbox/object/impl/reader/bin.c")
        add_files("src/tbox/object/impl/reader/json.c")
        add_files("src/tbox/object/impl/reader/bplist.c")
        add_files("src/tbox/object/impl/reader/reader.c")
        add_files("src/tbox/object/impl/writer/bin.c")
        add_files("src/tbox/object/impl/writer/json.c")
        add_files("src/tbox/object/impl/writer/bplist.c")
        add_files("src/tbox/object/impl/writer/writer.c")
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
        add_files("src/tbox/zip/zip.c")
        add_files("src/tbox/zip/vlc/*.c")
        add_files("src/tbox/zip/rlc.c")
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

    -- add the source for the windows platform
    if plats("windows", "mingw") then
        add_files("src/tbox/windows/socket_pool.c")
        add_files("src/tbox/windows/interface/*.c")
    end

    -- add the source for the ios platform
    if plats("ios") then
        add_files("src/tbox/platform/mach/ios/directory.m")
    end

    -- add the source for the android platform
    if plats("android") then
        add_files("src/tbox/platform/linux/android/*.c")
    end
