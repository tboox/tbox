-- add polarssl package
add_option("polarssl")

    -- show menu
    set_option_showmenu(true)

    -- set category
    set_option_category("package")

    -- set description
    set_option_description("The polarssl package")

    -- add defines to config.h if checking ok
    add_option_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_POLARSSL")

    -- add links for checking
    add_option_links("polarssl")

    -- add link directories
    add_option_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_option_cincludes("polarssl/polarssl.h")

    -- add include directories
    add_option_includedirs("inc", "inc/$(plat)/$(arch)")

