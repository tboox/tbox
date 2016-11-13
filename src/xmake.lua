-- add option: demo
option("demo")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the demo module")

-- add type: wchar
option("wchar")
    add_ctypes("wchar_t")
    add_defines_h_if_ok("$(prefix)_TYPE_HAVE_WCHAR")

-- add option: float
option("float")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the float type")
    add_defines_h_if_ok("$(prefix)_TYPE_HAVE_FLOAT")

-- add option: info
option("info")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable to get some info, .e.g version ..")
    add_defines_h_if_ok("$(prefix)_INFO_HAVE_VERSION")

-- add option: exception
option("exception")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the exception.")
    add_defines_h_if_ok("$(prefix)_EXCEPTION_ENABLE")

-- add option: deprecated
option("deprecated")
    set_default(true)
    set_showmenu(true)
    set_category("option")
    set_description("Enable or disable the deprecated interfaces.")
    add_defines_h_if_ok("$(prefix)_API_HAVE_DEPRECATED")

-- add option: smallest
option("smallest")
    set_default(false)
    set_showmenu(true)
    set_category("option")
    set_description("Enable the smallest compile mode and disable all modules.")
    add_rbindings("info", "deprecated")
    add_rbindings("xml", "zip", "asio", "hash", "regex", "object", "charset", "database", "coroutine")
    add_rbindings("zlib", "mysql", "sqlite3", "openssl", "polarssl", "mbedtls", "pcre2", "pcre")

-- add modules
for _, module in ipairs({"xml", "zip", "hash", "regex", "object", "charset", "database", "coroutine"}) do
    option(module)
        set_default(true)
        set_showmenu(true)
        set_category("module")
        set_description(format("The %s module", module))
        add_defines_h_if_ok(format("$(prefix)_MODULE_HAVE_%s", module:upper()))
end

-- add project directories
add_subdirs("tbox") 
if is_option("demo") then 
    add_subdirs("demo") 
end
