The Treasure Box Library
========================

TBOX is a mutli-platform c library for unix, windows, mac, ios, android, etc.

It includes asio, stream, network, container, algorithm, object, memory, database, string, charset, math, libc, libm, utils and other library modules.


features
--------

### the stream io library
-	supports file, data, http and socket source
-	supports the stream filter for gzip charset and ..
-	implements the multi-stream transfer using asio	

### the asynchronous io library
-	supports reactor and proactor mode
-	using epoll, poll, select ,kqueue and iocp os system api

### the database library
-	supports mysql and sqlite3 database and enumerates data using the iterator mode

### the xml parser library
-	supports DOM and SAX mode and supports xpath.

### the serialization and deserialization library. 
-	supports xml, json, bplist, xplist, binary formats

### the memory library
-	implements some memory pools for optimizating memory.
-	supports fast memory error detecting. it can detect the following types of bugs for the debug mode:
 - out-of-bounds accesses to heap and globals
 - use-after-free
 -	double-free, invalid free
 -	memory leaks

### the container library
-	implements hash table, single list, double list, vector, stack, queue
	and min/max heap. supports iterator mode for algorithm.

### the algorithm library
-	using the iterator mode
-	implements find, binary find and reverse find algorithm.
-	implements sort, bubble sort, quick sort, heap sort and insert sort algorithm. 
-	implements count, walk items, reverse walk items, for_all and rfor_all.

### the network library
-	implements dns(cached), ssl(openssl and polarssl), http and cookies
-	supports asynchronous io mode for dns, ssl and http using the asio and stream library

### the platform library
-	implements timer, faster and lower precision timer
-	implements atomic and atomic64 operation
-	implements spinlock, mutex, event, semaphore, thread and thread pool 
-	implements file, socket operation

### the charset library
-	supports utf8, utf16, gbk, gb2312, uc2 and uc4
-	supports big endian and little endian mode

### the zip library
-	supports gzip, zlibraw, zlib formats using the zlib library if exists
-	implements lzsw, lz77 and rlc algorithm

### the utils library
-	implements base32, base64, md5 and sha algorithm
-	implements assert and trace output for the debug mode

### the math library
-	implements random generator
-	implements fast fixed-point calculation, supports 6-bits, 16-bits, 30-bits fixed-point number

### the libc library
-	implements lightweight libc library interfaces, the interface name contains tb_xxx prefix for avoiding conflict.
-	implements strixxx strrxxx wcsixxx wcsrxxx interface extension.
-	optimizates some frequently-used interface, .e.g. memset, memcpy, strcpy ... 
-	implements memset_u16, memset_u32, memset_u64 extension interfaces.

### the libm library
-	implements lightweight libm library interfaces, the interface name contains tb_xxx prefix for avoiding conflict.
-	supports float and double type.

contact
-------

- email:   	    waruqi@gmail.com
- source:  	    https://github.com/waruqi/tbox
- website: 	    http://www.tboox.org or http://www.tboox.net
- download:     https://github.com/waruqi/tbox/archive/master.zip
- qq(group):    343118190

usage
-----

```bash
	// configure
    make config|f [PLAT=linux|mac|mingw|cygwin] [ARCH=x86|x64] [DEBUG=y|n] [SMALL=y|n] [DEMO=y|n] [PROF=y|n] [PREFIX=..] [BIN=] [PRE=] [SDK=]
    make config|f [PLAT=ios] [ARCH=armv6|armv7|armv7s] [DEBUG=y|n] [SMALL=y|n] [DEMO=y|n] [PROF=y|n] [PREFIX=..] [BIN=] [PRE=] [SDK=5.0|5.1|6.0|..]
    make config|f [PLAT=android] [ARCH=armv5te|armv6] [DEBUG=y|n] [SMALL=y|n] [DEMO=y|n] [PROF=y|n] [PREFIX=..] [BIN=] [PRE=] [NDK=..] [SDK=..]

	// build
    make [all]
    make rebuild|r
    make lipo ARCH1=armv6 ARCH2=armv7 DEBUG=[y|n] SDK=[5.0|5.1|6.0|..] // for ios 

	// update build
    make update|u

	// clean build
    make clean|c

	// install
    make prefix|p
    make install|i

	// make info
    make error|e
    make output|o
    make warning|w

	// make document
    make doc|d

	// help
    make help|h
```

build
-----

```bash
	// build for linux-x86 debug
    cd ./tbox
    [make f PLAT=linux ARCH=x86 DEBUG=y] => optional
    make r

	// build for linux-x64 release
    cd ./tbox
    [make f PLAT=linux ARCH=x64 DEBUG=n] => optional
    make r

	// build for linux and add cflags and ldflags
    cd ./tbox
    make f PLAT=linux CFLAG="-I." LDFLAG="-L. -lxxx"
    make r

	// build for mac
    cd ./tbox
    [make f PLAT=mac ARCH=x64] => optional
    make r

	// build for ios-armv7, using sdk7.1 framework
    cd ./tbox
    make f PLAT=ios ARCH=armv7 SDK=7.1
    make r
    
	// build for ios armv7 and arm64 universal version
    cd ./tbox
    make lipo ARCH1=armv7 ARCH2=arm64 SDK=7.1

	// build for android-armv5te, need set the ndk and toolchains directory
    cd ./tbox
    make f PLAT=android ARCH=armv5te NDK=/home/xxxx/android-ndk-r9d BIN=/home/xxxx/android-ndk-r9d/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64/bin
    make r
    
	// build for android-armv6, if ndk and toolchains have been setted
    cd ./tbox
    make f PLAT=android ARCH=armv6
    make r

	// build for windows using msvc
    cd ./tbox
    run ./msys.bat
    [make f PLAT=msvc ARCH=x86] => optional
    make r

	// build for windows using mingw and need link libgcc.a from mingw
    run msys
    cd ./tbox
    [make f PLAT=mingw ARCH=x86] => optional
    make r

	// build for windows using cygwin 
    run cygwin
    cd ./tbox
    [make f PLAT=cygwin ARCH=x86] => optional
    make r

	// build for windows and custom complier path and prefix and need link libgcc.a from mingw
    run cygwin
    cd ./tbox
    make f PLAT=mingw ARCH=x86 BIN="/home/xxx/bin" PRE="i386-mingw32-"
    make r
```

development
-----------

- the installed prefix header and library directory after runing "make r":
 -  inc: ./tbox/pre/inc/$(PLAT)/$(ARCH)/
 -  lib: ./tbox/pre/lib/$(PLAT)/$(ARCH)/

- for linux x64:
 -  inc:                ./tbox/pre/inc/linux/x64/tbox
 -  lib[debug]:         ./tbox/pre/lib/linux/x64/libtboxd.a
 -  lib[release]:       ./tbox/pre/lib/linux/x64/libtboxr.a
 -  CFLAGS[debug]:      -D\_\_tb\_debug\_\_
 -  CFLAGS[release]:    
 -  CFLAGS[optional]:   -D\_\_tb\_prefix\_\_=\"tag\"
 -  LDFLAGS[debug]:     -L./tbox/pre/lib/linux/x64/ -ltboxd
 -  LDFLAGS[release]:   -L./tbox/pre/lib/linux/x64/ -ltboxr

- for msvc x86:
 -  inc:                ./tbox/pre/inc/msvc/x64/tbox
 -  lib[debug]:         ./tbox/pre/lib/msvc/x64/tboxd.lib
 -  lib[release]:       ./tbox/pre/lib/msvc/x64/tboxr.lib
 -  CFLAGS[debug]:      -D\_\_tb\_debug\_\_
 -  CFLAGS[release]:    
 -  CFLAGS[optional]:   -D\_\_tb\_prefix\_\_=\"tag\"
 -  LDFLAGS[debug]:     -LIBPATH:.\\tbox\\pre\\lib/msvc\\x86 tboxd.lib
 -  LDFLAGS[release]:   -LIBPATH:.\\tbox\\pre\\lib/msvc\\x86 tboxr.lib

example
-----------
```c
    #include "tbox/tbox.h"

    int main(int argc, char** argv)
    {
        /* init tbox
         *
         * @param priv      the platform private data
         *                  pass JNIEnv* env for android
         *                  pass tb_null for other platform
         * @param data      the memory data for the memory pool, uses the native memory if be tb_null
         * @param size      the memory size for the memory pool, uses the native memory if be zero
         *
         * for android:     tb_init(jenv, tb_null, 0)
         * for memory pool: tb_init(tb_null, malloc(10 * 1024 * 1024), 10 * 1024 * 1024);
         */
        if (!tb_init(tb_null, tb_null, 0)) return 0;

        // print info with tag
        tb_trace_i("hello tbox");

        // print info only for debug
        tb_trace_d("hello tbox"); 

        // print error info
        tb_trace_e("hello tbox");

        // init stream
        tb_stream_ref_t stream = tb_stream_init_from_url("http://www.xxxx.com/index.html");
        if (stream)
        {
            // save stream data to file
            tb_transfer_done_to_url(stream, "/home/file/index.html", 0, tb_null, tb_null);

            // exit stream
            tb_stream_exit(stream);
        }

        // block: save http to file
        tb_transfer_done_url("http://www.xxxx.com/index.html", "/home/file/index.html", 0, tb_null, tb_null);

        // async: save http to file
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index0.html", "/home/file/index0.html", 0, 0, tb_null, tb_null);
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index1.html", "/home/file/index2.html", 0, 0, tb_null, tb_null);
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index2.html", "/home/file/index3.html", 0, 0, tb_null, tb_null);
        tb_transfer_pool_done(tb_transfer_pool(), "http://www.xxxx.com/index3.html", "/home/file/index4.html", 0, 0, tb_null, tb_null);

        // ...

        // exit tbox
        tb_exit();
        return 0;
    }
```


