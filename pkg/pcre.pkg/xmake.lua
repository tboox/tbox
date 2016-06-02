-- add pcre package
option("pcre")

    -- show menu
    set_showmenu(true)

    -- set category
    set_category("package")

    -- set description
    set_description("The pcre package")
    
    -- add defines to config.h if checking ok
    add_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_PCRE")

    -- add links for checking
    add_links("pcre")

    -- add link directories
    add_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_cincludes("pcre/pcre.h")

    -- add include directories
    add_includedirs("inc/$(plat)", "inc")

    -- add c functions
    add_cfuncs("pcre_compile")
