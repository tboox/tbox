#!/bin/sh

target "tbox"
    set_kind "${kind}"

    # add defines
    add_defines "__tb_prefix__=\"tbox\""

    # set the auto-generated config.h
    if $check_interfaces_enabled; then
        set_configdir "${buildir}/${plat}/${arch}/${mode}"
        add_configfiles "tbox.config.h.in"
        add_headerfiles "${buildir}/${plat}/${arch}/${mode}/tbox.config.h" "tbox"
    fi

    # add include directories
    add_includedirs ".." "{public}"
    add_includedirs "${buildir}/${plat}/${arch}/${mode}" "{public}"

    # add the header files for installing
    add_headerfiles "../(tbox/**.h)"
    add_headerfiles "../(tbox/**.S)"

    # add frameworks
    if is_plat "macosx"; then
        add_frameworks "CoreFoundation" "CoreServices" "{public}"
    fi

    # add the common source files
    add_files "*.c"
    add_files "hash/bkdr.c"
    add_files "hash/fnv32.c"
    add_files "hash/adler32.c"
    add_files "math/**.c"
    add_files "libc/misc/*.c"
    add_files "libc/misc/time/*.c"
    add_files "libc/stdio/*.c"
    add_files "libc/stdlib/*.c"
    add_files "libc/string/*.c"
    add_files "libc/impl/*.c"
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
    add_files "network/impl/*.c"
    add_files "network/impl/http/*.c"
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
    add_files "platform/impl/platform.c"
    add_files "platform/impl/pollerdata.c"
    add_files "platform/impl/dns.c"

    # add the source files for the windows
    if is_plat "mingw"; then
        add_files "platform/windows/windows.c"
        add_files "platform/windows/iocp_object.c"
        add_files "platform/windows/socket_pool.c"
        add_files "platform/windows/interface/*.c"
    fi

    # add the source files for the float type
    if has_config "float"; then
        add_files "libm/acos.c"
        add_files "libm/acosf.c"
        add_files "libm/asin.c"
        add_files "libm/asinf.c"
        add_files "libm/atan.c"
        add_files "libm/atan2.c"
        add_files "libm/atan2f.c"
        add_files "libm/atanf.c"
        add_files "libm/cos.c"
        add_files "libm/cosf.c"
        add_files "libm/exp.c"
        add_files "libm/exp1.c"
        add_files "libm/exp1f.c"
        add_files "libm/expf.c"
        add_files "libm/expi.c"
        add_files "libm/expif.c"
        add_files "libm/fmod.c"
        add_files "libm/fmodf.c"
        add_files "libm/isfin.c"
        add_files "libm/isfinf.c"
        add_files "libm/isinf.c"
        add_files "libm/isinff.c"
        add_files "libm/isnan.c"
        add_files "libm/isnanf.c"
        add_files "libm/log2.c"
        add_files "libm/log2f.c"
        add_files "libm/pow.c"
        add_files "libm/powf.c"
        add_files "libm/sin.c"
        add_files "libm/sincos.c"
        add_files "libm/sincosf.c"
        add_files "libm/sinf.c"
        add_files "libm/sqrt.c"
        add_files "libm/sqrtf.c"
        add_files "libm/tan.c"
        add_files "libm/tanf.c"
    fi

    # add the source files for the xml module
    if has_config "xml"; then
        add_files "xml/**.c"
    fi

    # add the source files for the regex module
    if has_config "regex"; then
        add_files "regex/*.c"
    fi

    # add the source files for the hash module
    if has_config "hash"; then
        add_files "hash/ap.c"
        add_files "hash/blizzard.c"
        add_files "hash/crc16.c"
        add_files "hash/crc32.c"
        add_files "hash/crc8.c"
        add_files "hash/djb2.c"
        add_files "hash/fnv64.c"
        add_files "hash/md5.c"
        add_files "hash/murmur.c"
        add_files "hash/sdbm.c"
        add_files "hash/rs.c"
        add_files "hash/sha.c"
        add_files "hash/uuid.c"
        add_files "hash/arch/crc32.S"
    fi

    # add the source files for the coroutine module
    if has_config "coroutine"; then
        add_files "platform/context.c"
        add_files "platform/arch/context.S"
        add_files "coroutine/**.c"
    fi

    # add the source files for the exception module
    if has_config "exception"; then
        add_files "platform/exception.c"
    fi

    # add the source files for the object module
    if has_config "object"; then
        add_files "object/*.c"
        add_files "object/impl/*.c"
        add_files "object/impl/reader/bin.c"
        add_files "object/impl/reader/json.c"
        add_files "object/impl/reader/bplist.c"
        add_files "object/impl/reader/reader.c"
        add_files "object/impl/writer/bin.c"
        add_files "object/impl/writer/json.c"
        add_files "object/impl/writer/bplist.c"
        add_files "object/impl/writer/writer.c"
        add_files "utils/option.c"
        add_files "container/element/obj.c"
        if has_config "xml"; then
            add_files "object/impl/reader/xml.c"
            add_files "object/impl/reader/xplist.c"
            add_files "object/impl/writer/xml.c"
            add_files "object/impl/writer/xplist.c"
        fi
    fi

    # add the source files for the charset module
    if has_config "charset"; then
        add_files "charset/**.c"
        add_files "platform/impl/charset.c"
        add_files "stream/impl/filter/charset.c"
    fi

    # add the source files for the zip module
    if has_config "zip"; then
        add_files "zip/zip.c"
        add_files "stream/impl/filter/zip.c"
    fi

    # add the source files for the database module
    if has_config "database"; then
        add_files "database/*.c"
    fi

    # check interfaces
    check_interfaces

