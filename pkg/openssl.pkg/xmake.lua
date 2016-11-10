-- the openssl package
option("openssl")

    -- show menu
    set_showmenu(true)

    -- set category
    set_category("package")

    -- set description
    set_description("The openssl package")

    -- add defines to config.h if checking ok
    add_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_OPENSSL")

    -- add links for checking
    add_links("ssl", "crypto")

    -- add link directories
    add_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_cincludes("openssl/openssl.h")

    -- add include directories
    add_includedirs("inc/$(plat)", "inc")
 
    -- add c functions
    add_cfuncs("SSL_new")
