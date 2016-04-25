-- add pcre package
add_option("pcre")

    -- show menu
    set_option_showmenu(true)

    -- set category
    set_option_category("package")

    -- set description
    set_option_description("The pcre package")
    
    -- add defines to config.h if checking ok
    add_option_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_PCRE")

    -- add links for checking
    add_option_links("pcre")

    -- add link directories
    add_option_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_option_cincludes("pcre/pcre.h")

    -- add include directories
    add_option_includedirs("inc/$(plat)", "inc")

    -- add c functions
    add_option_cfuncs("pcre_compile")
