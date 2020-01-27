
-- add target
target("demo")

    -- add the dependent target
    add_deps("tbox")

    -- make as a binary
    set_kind("binary")

    -- add defines
    add_defines("__tb_prefix__=\"demo\"")

    -- add the source file
    add_files("micro.c") 
    add_files("libc/stdlib.c") 
    add_files("utils/bits.c") 
    add_files("other/test.c") 
    add_files("platform/addrinfo.c") 
    add_files("container/list_entry.c") 
    add_files("container/single_list_entry.c") 

    -- add the source files for coroutine
    if has_config("coroutine") then
        add_files("coroutine/stackless/*.c|process.c") 
    end

