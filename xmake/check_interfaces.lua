-- imports
import("core.base.option")
import("async.runjobs", {try = true})
if not runjobs then
    import("private.async.runjobs") -- it has been moved to public above 2.8.3
end

local _check_tasks = {}

-- get function name
--
-- sigsetjmp
-- sigsetjmp((void*)0, 0)
-- sigsetjmp{sigsetjmp((void*)0, 0);}
-- sigsetjmp{int a = 0; sigsetjmp((void*)a, a);}
--
function _get_function_name(func)
    local name, code = string.match(func, "(.+){(.+)}")
    if code == nil then
        local pos = func:find("%(")
        if pos then
            name = func:sub(1, pos - 1)
        else
            name = func
        end
    end
    return name:trim()
end

-- get the c flags for suppressing warnings
function _get_cflags_for_suppress_warnings(target)
    local cflags = target:data("_check_interfaces.cflags")
    if cflags == nil then
        if target:has_tool("cc", "cl") then
            cflags = "-D_CRT_SECURE_NO_WARNINGS"
        elseif target:has_tool("cc", "gcc", "gxx", "clang", "clangxx") then
            cflags = "-Wno-error=unused-variable"
        end
        target:data_set("_check_interfaces.cflags", cflags or false)
    end
    return cflags or nil
end

-- check c functions in the given module
function _check_module_cfuncs(target, module, includes, ...)
    local cflags = _get_cflags_for_suppress_warnings(target)
    for _, func in ipairs({...}) do
        table.insert(_check_tasks, function ()
            local funcname = _get_function_name(func)
            local checkname = module .. "_" .. funcname
            local ok = false
            if target:has_cfuncs(func, {name = checkname, includes = includes, configs = {cflags = cflags}}) then
                target:set("configvar", ("TB_CONFIG_%s_HAVE_%s"):format(module:upper(), funcname:upper()), 1)
                ok = true
            end
            cprint("checking for %s ... %s", checkname,
                ok and "${color.success}${text.success}" or "${color.nothing}${text.nothing}")
        end)
    end
end

-- check c snippet in the given module
function _check_module_csnippet(target, module, includes, name, snippet)
    local cflags = _get_cflags_for_suppress_warnings(target)
    table.insert(_check_tasks, function ()
        local checkname = module .. "_" .. name
        local ok = false
        if target:check_csnippets({[checkname] = snippet}, {includes = includes, configs = {cflags = cflags}}) then
            target:set("configvar", ("TB_CONFIG_%s_HAVE_%s"):format(module:upper(), name:upper()), 1)
            ok = true
        end
        cprint("checking for %s ... %s", checkname,
            ok and "${color.success}${text.success}" or "${color.nothing}${text.nothing}")
    end)
end

-- check c keyword
function _check_keyword_csnippet(target, name, varname, snippet, configs)
    local cflags = _get_cflags_for_suppress_warnings(target)
    table.insert(_check_tasks, function ()
        local checkname = name
        local ok = false
        if target:check_csnippets({[checkname] = snippet}, {configs = table.join({cflags = cflags}, configs)}) then
            target:set("configvar", varname, 1)
            ok = true
        end
        cprint("checking for %s ... %s", checkname,
            ok and "${color.success}${text.success}" or "${color.nothing}${text.nothing}")
    end)
end

function _check_interfaces(target)

    -- check the interfaces for libc
    _check_module_cfuncs(target, "libc", {"string.h", "stdlib.h"},
        "memcpy",
        "memset",
        "memmove",
        "memcmp",
        "memmem",
        "strcat",
        "strncat",
        "strcpy",
        "strncpy",
        "strlcpy",
        "strlen",
        "strnlen",
        "strstr",
        "strchr",
        "strrchr",
        "strcasestr",
        "strcmp",
        "strcasecmp",
        "strncmp",
        "strncasecmp")
    _check_module_cfuncs(target, "libc", {"wchar.h", "stdlib.h"},
        "wcscat",
        "wcsncat",
        "wcscpy",
        "wcsncpy",
        "wcslcpy",
        "wcslen",
        "wcsnlen",
        "wcsstr",
        "wcscasestr",
        "wcscmp",
        "wcscasecmp",
        "wcsncmp",
        "wcsncasecmp",
        "wcstombs",
        "mbstowcs")
    _check_module_cfuncs(target, "libc", "time.h",                           "gmtime", "mktime", "localtime")
    _check_module_cfuncs(target, "libc", "sys/time.h",                       "gettimeofday")
    _check_module_cfuncs(target, "libc", {"signal.h", "setjmp.h"},           "signal", "setjmp", "sigsetjmp{sigjmp_buf buf; sigsetjmp(buf, 0);}", "kill")
    _check_module_cfuncs(target, "libc", "execinfo.h",                       "backtrace")
    _check_module_cfuncs(target, "libc", "locale.h",                         "setlocale")
    _check_module_cfuncs(target, "libc", "stdio.h",                          "fputs", "fgets", "fgetc", "ungetc", "fputc", "fread", "fwrite")
    _check_module_cfuncs(target, "libc", "stdlib.h",                         "srandom", "random")

    -- add the interfaces for libm
    _check_module_cfuncs(target, "libm", "math.h",
        "sincos",
        "sincosf",
        "log2",
        "log2f",
        "sqrt",
        "sqrtf",
        "acos",
        "acosf",
        "asin",
        "asinf",
        "pow",
        "powf",
        "fmod",
        "fmodf",
        "tan",
        "tanf",
        "atan",
        "atanf",
        "atan2",
        "atan2f",
        "cos",
        "cosf",
        "sin",
        "sinf",
        "exp",
        "expf")

    -- add the interfaces for posix
    if not target:is_plat("windows") then
        _check_module_cfuncs(target, "posix", {"poll.h", "sys/socket.h"},         "poll")
        _check_module_cfuncs(target, "posix", {"sys/select.h"},                   "select")
        _check_module_cfuncs(target, "posix", "pthread.h",
            "pthread_mutex_init",
            "pthread_create",
            "pthread_setspecific",
            "pthread_getspecific",
            "pthread_key_create",
            "pthread_key_delete",
            "pthread_setaffinity_np") -- need _GNU_SOURCE
        _check_module_cfuncs(target, "posix", {"sys/socket.h", "fcntl.h"},        "socket")
        _check_module_cfuncs(target, "posix", "dirent.h",                         "opendir")
        _check_module_cfuncs(target, "posix", "dlfcn.h",                          "dlopen")
        _check_module_cfuncs(target, "posix", {"sys/stat.h", "fcntl.h"},          "open", "stat64", "lstat64")
        _check_module_cfuncs(target, "posix", "unistd.h",                         "gethostname")
        _check_module_cfuncs(target, "posix", "ifaddrs.h",                        "getifaddrs")
        _check_module_cfuncs(target, "posix", "unistd.h",                         "getpagesize", "sysconf")
        _check_module_cfuncs(target, "posix", "sched.h",                          "sched_yield", "sched_setaffinity") -- need _GNU_SOURCE
        _check_module_cfuncs(target, "posix", "regex.h",                          "regcomp", "regexec")
        _check_module_cfuncs(target, "posix", "sys/uio.h",                        "readv", "writev", "preadv", "pwritev")
        _check_module_cfuncs(target, "posix", "unistd.h",                         "pread64", "pwrite64")
        _check_module_cfuncs(target, "posix", "unistd.h",                         "fdatasync")
        _check_module_cfuncs(target, "posix", "copyfile.h",                       "copyfile")
        _check_module_cfuncs(target, "posix", "sys/sendfile.h",                   "sendfile")
        _check_module_cfuncs(target, "posix", "sys/epoll.h",                      "epoll_create", "epoll_wait")
        _check_module_cfuncs(target, "posix", "unistd.h",                         "execvp", "execvpe", "fork", "vfork")
        _check_module_cfuncs(target, "posix", "sys/wait.h",                       "waitpid")
        _check_module_cfuncs(target, "posix", "unistd.h",                         "getdtablesize")
        _check_module_cfuncs(target, "posix", "sys/resource.h",                   "getrlimit")
        _check_module_cfuncs(target, "posix", "netdb.h",                          "getaddrinfo", "getnameinfo", "gethostbyname", "gethostbyaddr")
        _check_module_cfuncs(target, "posix", "fcntl.h",                          "fcntl")
        _check_module_cfuncs(target, "posix", "unistd.h",                         "pipe", "pipe2")
        _check_module_cfuncs(target, "posix", "sys/stat.h",                       "mkfifo")
        _check_module_cfuncs(target, "posix", "sys/mman.h",                       "mmap")
        _check_module_cfuncs(target, "posix", "sys/stat.h",                       "futimens", "utimensat")
    elseif not target:is_plat("windows", "wasm") then
        _check_module_cfuncs(target, "posix", "spawn.h",                          "posix_spawnp", "posix_spawn_file_actions_addchdir_np")
        _check_module_cfuncs(target, "posix", "semaphore.h",                      "sem_init")
    end

    -- add the interfaces for windows/msvc
    if target:is_plat("windows") then
        for _, mo in ipairs({"", "_nf", "_acq", "_rel"}) do
            _check_module_csnippet(target, "windows", "windows.h", "_InterlockedExchange" .. mo, format([[
                LONG _InterlockedExchange%s(LONG volatile* Destination, LONG Value);
                #pragma intrinsic(_InterlockedExchange%s)
                void test() {
                    _InterlockedExchange%s(0, 0);
                }]], mo, mo, mo))
            _check_module_csnippet(target, "windows", "windows.h", "_InterlockedExchange8" .. mo, format([[
                CHAR _InterlockedExchange8%s(CHAR volatile* Destination, CHAR Value);
                #pragma intrinsic(_InterlockedExchange8%s)
                void test() {
                    _InterlockedExchange8%s(0, 0);
                }]], mo, mo, mo))
            _check_module_csnippet(target, "windows", "windows.h", "_InterlockedOr8" .. mo, format([[
                CHAR _InterlockedOr8%s(CHAR volatile* Destination, CHAR Value);
                #pragma intrinsic(_InterlockedOr8%s)
                void test() {
                    _InterlockedOr8%s(0, 0);
                }]], mo, mo, mo))
            _check_module_csnippet(target, "windows", "windows.h", "_InterlockedExchangeAdd" .. mo, format([[
                LONG _InterlockedExchangeAdd%s(LONG volatile* Destination, LONG Value);
                #pragma intrinsic(_InterlockedExchangeAdd%s)
                void test() {
                    _InterlockedExchangeAdd%s(0, 0);
                }]], mo, mo, mo))
            _check_module_csnippet(target, "windows", "windows.h", "_InterlockedExchangeAdd64" .. mo, format([[
                __int64 _InterlockedExchangeAdd64%s(__int64 volatile* Destination, __int64 Value);
                #pragma intrinsic(_InterlockedExchangeAdd64%s)
                void test() {
                    _InterlockedExchangeAdd64%s(0, 0);
                }]], mo, mo, mo))
            _check_module_csnippet(target, "windows", "windows.h", "_InterlockedCompareExchange" .. mo, format([[
                LONG _InterlockedCompareExchange%s(LONG volatile* Destination, LONG Exchange, LONG Comperand);
                #pragma intrinsic(_InterlockedCompareExchange%s)
                void test() {
                    _InterlockedCompareExchange%s(0, 0, 0);
                }]], mo, mo, mo))
            _check_module_csnippet(target, "windows", "windows.h", "_InterlockedCompareExchange64" .. mo, format([[
                __int64 _InterlockedCompareExchange64%s(__int64 volatile* Destination, __int64 Exchange, __int64 Comperand);
                #pragma intrinsic(_InterlockedCompareExchange64%s)
                void test() {
                    _InterlockedCompareExchange64%s(0, 0, 0);
                }]], mo, mo, mo))
        end
    end

    -- add the interfaces for bsd
    if not target:is_plat("windows") then
        _check_module_cfuncs(target, "bsd", {"sys/file.h", "fcntl.h"}, "flock")
    end

    -- add the interfaces for systemv
    if not target:is_plat("windows", "wasm") then
        _check_module_cfuncs(target, "systemv", {"sys/sem.h", "sys/ipc.h"}, "semget", "semtimedop")
    end

    -- add the interfaces for linux
    if target:is_plat("linux", "android") then
        _check_module_cfuncs(target, "linux", {"sys/inotify.h"}, "inotify_init")
    end

    -- add the interfaces for valgrind
    _check_module_cfuncs(target, "valgrind", "valgrind/valgrind.h",  "VALGRIND_STACK_REGISTER(0, 0)")

    -- check __thread keyword
    _check_keyword_csnippet(target, "keyword_thread", "TB_CONFIG_KEYWORD_HAVE__thread", "__thread int a = 0;", {links = "pthread"})
    _check_keyword_csnippet(target, "keyword_thread_local", "TB_CONFIG_KEYWORD_HAVE_Thread_local", "_Thread_local int a = 0;", {links = "pthread"})

    -- check anonymous union feature
    _check_keyword_csnippet(target, "feature_anonymous_union", "TB_CONFIG_FEATURE_HAVE_ANONYMOUS_UNION", "void test() { struct __st { union {int dummy;};} a; a.dummy = 1; }")

    -- do check
    local jobs = option.get("jobs") or os.default_njob()
    runjobs("check_interfaces", function (index)
        _check_tasks[index]()
    end, {total = #_check_tasks, comax = jobs})
end

function main(target, opt)
    if not opt or (opt and opt.recheck) then
        _check_interfaces(target)
    end
end
