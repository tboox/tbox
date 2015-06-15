
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
    add_options("xml", "zip", "asio", "object", "charset", "database")

    -- add packages
    add_options("zlib", "mysql", "sqlite3", "openssl", "polarssl")

    -- add interfaces
    add_options(libc, libm, posix, systemv)

    -- add the common source files
    add_files("src/tbox/*.c") 
    add_files("src/tbox/asio/*.c") 
    add_files("src/tbox/math/**.c") 
    add_files("src/tbox/libc/misc/**.c") 
    add_files("src/tbox/libc/stdio/**.c") 
    add_files("src/tbox/libc/stdlib/**.c") 
    add_files("src/tbox/libc/string/*.c") 
    add_files("src/tbox/utils/**.c") 
    add_files("src/tbox/prefix/**.c") 
    add_files("src/tbox/memory/**.c") 
    add_files("src/tbox/string/**.c") 
    add_files("src/tbox/stream/**.c") 
    add_files("src/tbox/network/**.c") 
    add_files("src/tbox/platform/*.c") 
    add_files("src/tbox/algorithm/**.c") 
    add_files("src/tbox/container/**.c") 
    add_files("src/tbox/libm/libm.c") 
    add_files("src/tbox/libm/idivi8.c") 
    add_files("src/tbox/libm/ilog2i.c") 
    add_files("src/tbox/libm/isqrti.c") 
    add_files("src/tbox/libm/isqrti64.c") 
    add_files("src/tbox/libm/idivi8.c") 
