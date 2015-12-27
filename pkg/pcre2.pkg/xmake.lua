-- add pcre2 package
add_option("pcre2")

    -- show menu
    set_option_showmenu(true)

    -- set category
    set_option_category("package")

    -- set description
    set_option_description("The pcre2 package")
    
    -- add defines to config.h if checking ok
    add_option_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_PCRE2")

    -- add defines for checking
    add_option_defines("PCRE2_CODE_UNIT_WIDTH=8")

    -- add links for checking
    add_option_links("pcre2")

    -- add link directories
    add_option_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_option_cincludes("pcre2/pcre2.h")

    -- add include directories
    add_option_includedirs("inc/$(plat)", "inc")
