-- the openssl package
add_option("openssl")

    -- show menu
    set_option_showmenu(true)

    -- set category
    set_option_category("package")

    -- set description
    set_option_description("The openssl package")

    -- add defines to config.h if checking ok
    add_option_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_OPENSSL")

    -- add links for checking
    add_option_links("ssl", "crypto")

    -- add link directories
    add_option_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_option_cincludes("openssl/openssl.h")

    -- add include directories
    add_option_includedirs("inc", "inc/$(plat)/$(arch)")
 
