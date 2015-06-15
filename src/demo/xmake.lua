
-- add target
add_target("demo")

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

    -- add packages
    add_options("zlib", "mysql", "sqlite3", "openssl", "polarssl")

    -- add links
    add_links("tbox")

    -- add the source files
    add_files("src/demo/**.c") 
