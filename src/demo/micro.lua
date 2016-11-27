
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
    add_packages("base")

    -- add the source files
    add_files("micro.c") 
    add_files("libc/stdlib.c") 
    add_files("utils/bits.c") 
    add_files("other/test.c") 
    add_files("platform/addrinfo.c") 
    add_files("container/list_entry.c") 
    add_files("container/single_list_entry.c") 
    add_files("coroutine/stackless/*.c") 

