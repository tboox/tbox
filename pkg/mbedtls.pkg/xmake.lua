-- add mbedtls package
option("mbedtls")

    -- show menu
    set_showmenu(true)

    -- set category
    set_category("package")

    -- set description
    set_description("The mbedtls package")

    -- add defines to config.h if checking ok
    add_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_MBEDTLS")

    -- add links for checking
    add_links("mbedtls", "mbedcrypto", "mbedx509")

    -- add link directories
    add_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_cincludes("mbedtls/mbedtls.h")

    -- add include directories
    add_includedirs("inc/$(plat)", "inc")

    -- add c functions
    add_cfuncs("mbedtls_ssl_setup")
