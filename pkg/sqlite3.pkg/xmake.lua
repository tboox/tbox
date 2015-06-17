add_option("sqlite3")
    set_option_showmenu(true)
    set_option_category("package")
    set_option_description("The sqlite3 package")
    add_option_defines_h_if_ok("CONFIG_PACKAGE_HAVE_SQLITE3")
    add_option_links("sqlite3")
    add_option_linkdirs("lib/$(plat)/$(arch)")
    add_option_cincludes("sqlite3/sqlite3.h")
    add_option_includedirs("inc", "inc/$(plat)/$(arch)")


