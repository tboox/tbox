#!/bin/sh

target "tbox"
    set_kind "static"

    # add defines
    add_defines "__tb_prefix__=\"tbox\""

    # set the auto-generated config.h
    set_configdir "${buildir}/${plat}/${arch}/${mode}"
    add_configfiles "tbox.config.h.in"

    # add include directories
    add_includedirs ".."
    add_includedirs "${buildir}/${plat}/${arch}/${mode}"

    # add the header files for installing
    add_headerfiles "../(tbox/**.h)"
    add_headerfiles "../(tbox/**.S)"
    add_headerfiles "${buildir}/${plat}/${arch}/${mode}/tbox.config.h" "tbox"

    # add frameworks
    if is_plat "macosx"; then
        add_frameworks "CoreFoundation" "CoreServices"
    fi

    # add options
    add_options "info" "float" "wchar" "exception" "force_utf8" "micro" "deprecated"

    # add modules
    add_options "xml" "zip" "hash" "regex" "coroutine" "object" "charset" "database"

    # add the common source files
    add_files "*.c"
    add_files "hash/bkdr.c"
    add_files "hash/fnv32.c"
    add_files "hash/adler32.c"
    add_files "math/**.c"
    add_files "libc/misc/*.c"
    add_files "libc/stdio/*.c"
    add_files "libc/stdlib/*.c"
    add_files "libc/string/*.c"
    add_files "utils/base32.c"
    add_files "utils/base64.c"
    add_files "utils/bits.c"
    add_files "utils/dump.c"
    add_files "utils/lock_profiler.c"
    add_files "utils/singleton.c"
    add_files "utils/trace.c"
    add_files "utils/url.c"
    add_files "utils/used.c"
    add_files "prefix/**.c"
    add_files "memory/**.c"
    add_files "string/**.c"
    add_files "stream/*.c"
    add_files "stream/impl/stream/*.c"
    add_files "stream/impl/filter/cache.c"
    add_files "stream/impl/filter/chunked.c"
    add_files "network/*.c"
    add_files "network/dns/*.c"
    add_files "algorithm/**.c"
    add_files "container/*.c"
    add_files "container/element/hash.c"
    add_files "container/element/long.c"
    add_files "container/element/null.c"
    add_files "container/element/size.c"
    add_files "container/element/true.c"
    add_files "container/element/uint8.c"
    add_files "container/element/uint16.c"
    add_files "container/element/uint32.c"
    add_files "container/element/mem.c"
    add_files "container/element/ptr.c"
    add_files "container/element/str.c"
    add_files "libm/impl/libm.c"
    add_files "libm/idivi8.c"
    add_files "libm/ilog2i.c"
    add_files "libm/isqrti.c"
    add_files "libm/isqrti64.c"
    add_files "libm/idivi8.c"
    add_files "platform/addrinfo.c"
    add_files "platform/atomic64.c"
    add_files "platform/backtrace.c"
    add_files "platform/cache_time.c"
    add_files "platform/cpu.c"
    add_files "platform/directory.c"
    add_files "platform/dynamic.c"
    add_files "platform/environment.c"
    add_files "platform/event.c"
    add_files "platform/file.c"
    add_files "platform/filelock.c"
    add_files "platform/fwatcher.c"
    add_files "platform/hostname.c"
    add_files "platform/ifaddrs.c"
    add_files "platform/ltimer.c"
    add_files "platform/mutex.c"
    add_files "platform/native_memory.c"
    add_files "platform/page.c"
    add_files "platform/path.c"
    add_files "platform/pipe.c"
    add_files "platform/poller.c"
    add_files "platform/print.c"
    add_files "platform/process.c"
    add_files "platform/sched.c"
    add_files "platform/semaphore.c"
    add_files "platform/socket.c"
    add_files "platform/stdfile.c"
    add_files "platform/syserror.c"
    add_files "platform/thread.c"
    add_files "platform/thread_local.c"
    add_files "platform/thread_pool.c"
    add_files "platform/time.c"
    add_files "platform/timer.c"
    add_files "platform/virtual_memory.c"
    add_files "platform/impl/dns.c"
    add_files "platform/impl/platform.c"
    add_files "platform/impl/pollerdata.c"
    add_files "platform/impl/dns.c"

    # check interfaces
    check_interfaces

