-- add zlib package
add_option("zlib")

    -- show menu
    set_option_showmenu(true)

    -- set category
    set_option_category("package")

    -- set description
    set_option_description("The zlib package")
    
    -- add defines to config.h if checking ok
    add_option_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_ZLIB")

    -- add links for checking
    add_option_links("z")

    -- add link directories
    add_option_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_option_cincludes("zlib/zlib.h")

    -- add include directories
    add_option_includedirs("inc/$(plat)", "inc")
