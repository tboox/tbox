-- add sqlite3 package
option("sqlite3")

    -- show menu
    set_showmenu(true)

    -- set category
    set_category("package")

    -- set description
    set_description("The sqlite3 package")

    -- add defines to config.h if checking ok
    add_defines_h_if_ok("$(prefix)_PACKAGE_HAVE_SQLITE3")

    -- add links for checking
    add_links("sqlite3")

    -- add link directories
    add_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_cincludes("sqlite3/sqlite3.h")

    -- add include directories
    add_includedirs("inc/$(plat)", "inc")

    -- add c functions
    add_cfuncs("sqlite3_open_v2")
