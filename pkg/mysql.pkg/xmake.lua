add_option("mysql")
    set_option_showmenu(true)
    set_option_category("package")
    set_option_description("The mysql package")
    add_option_defines_h_if_ok("CONFIG_PACKAGE_HAVE_MYSQL")
    add_option_links("mysqlclient")
    add_option_linkdirs("lib/$(plat)/$(arch)")
    add_option_cincludes("mysql/mysql.h")
    add_option_includedirs("inc", "inc/$(plat)/$(arch)")


