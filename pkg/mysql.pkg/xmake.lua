-- the mysql package
add_option("mysql")

    -- show menu
    set_option_showmenu(true)

    -- set category
    set_option_category("package")

    -- set description
    set_option_description("The mysql package")

    -- add defines to config.h if checking ok
    add_option_defines_h_if_ok("CONFIG_PACKAGE_HAVE_MYSQL")

    -- add links for checking
    add_option_links("mysqlclient")

    -- add link directories
    add_option_linkdirs("lib/$(plat)/$(arch)")

    -- add c includes for checking
    add_option_cincludes("mysql/mysql.h")

    -- add include directories
    add_option_includedirs("inc", "inc/$(plat)/$(arch)")


