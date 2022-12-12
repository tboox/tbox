#!/bin/sh

target "demo"
    add_deps "tbox"
    set_kind "binary"
    add_defines "__tb_prefix__=\"demo\""

    # add the source files
    add_files "demo.c"
    add_files "libc/*.c"
    add_files "libm/integer.c"
    add_files "math/random.c"
    add_files "utils/base32.c"
    add_files "utils/base64.c"
    add_files "utils/bits.c"
    add_files "utils/dump.c"
    add_files "utils/url.c"
    add_files "other/test.c"
    add_files "other/test.cpp"
    add_files "string/*.c"
    add_files "memory/**.c"
    add_files "platform/addrinfo.c"
    add_files "platform/atomic.c"
    add_files "platform/atomic32.c"
    add_files "platform/atomic64.c"
    add_files "platform/backtrace.c"
    add_files "platform/cache_time.c"
    add_files "platform/directory.c"
    add_files "platform/environment.c"
    add_files "platform/event.c"
    add_files "platform/file.c"
    add_files "platform/filelock.c"
    add_files "platform/fwatcher.c"
    add_files "platform/hostname.c"
    add_files "platform/ifaddrs.c"
    add_files "platform/lock.c"
    add_files "platform/ltimer.c"
    add_files "platform/named_pipe.c"
    add_files "platform/path.c"
    add_files "platform/pipe_pair.c"
    add_files "platform/poller_client.c"
    add_files "platform/poller_fwatcher.c"
    add_files "platform/poller_pipe.c"
    add_files "platform/poller_process.c"
    add_files "platform/poller_server.c"
    add_files "platform/process.c"
    add_files "platform/sched.c"
    add_files "platform/semaphore.c"
    add_files "platform/stdfile.c"
    add_files "platform/thread.c"
    add_files "platform/thread_local.c"
    add_files "platform/thread_pool.c"
    add_files "platform/timer.c"
    add_files "platform/utils.c"
    add_files "container/*.c"
    add_files "algorithm/*.c"
    add_files "stream/stream.c"
    add_files "stream/stream/*.c"
    add_files "network/**.c"

    # add the source files for the hash module
    if has_config "hash"; then
        add_files "hash/*.c"
    fi

    # add the source files for the float type
    if has_config "float"; then
        add_files "math/fixed.c"
        add_files "libm/float.c"
        add_files "libm/double.c"
    fi

    # add the source files for the coroutine module
    if has_config "coroutine"; then
        add_files "coroutine/**.c"
        add_files "platform/context.c"
    fi

    # add the source files for the exception module
    if has_config "exception"; then
        add_files "platform/exception.c"
    fi

    # add the source files for the xml module
    if has_config "xml"; then
        add_files "xml/*.c"
    fi

    # add the source files for the regex module
    if has_config "regex"; then
        add_files "regex/*.c"
    fi

    # add the source files for the object module
    if has_config "object"; then
        add_files "utils/option.c"
        add_files "object/*.c"
    fi

    # add the source files for the charset module
    if has_config "charset"; then
        add_files "other/charset.c"
    fi

    # add the source files for the database module
    if has_config "database"; then
        add_files "database/sql.c"
    fi

    # link mingw/libgcc
    if is_plat "mingw" "msys" "cygwin"; then
        add_ldflags "-static-libgcc"
    fi

    # link backtrace/execinfo for bsd
    if is_plat "freebsd" "bsd"; then
        add_syslinks "execinfo"
    fi



