-- add sqlite3 package
add_option("sqlite3")

    -- show menu
    set_option_showmenu(true)

    -- set category
    set_option_category("package")

    -- set description
    set_option_description("The sqlite3 package")

    -- add defines to config.h if checking ok
    add_option_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_SQLITE3")

    -- add links for checking
    add_option_links("sqlite3")

    -- add link directories
    add_option_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_option_cincludes("sqlite3/sqlite3.h")

    -- add include directories
    add_option_includedirs("inc/$(plat)", "inc")


