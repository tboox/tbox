========================================================================
usage:

1. configure
    make config|f [PLAT=linux|mac|mingw|cygwin] [ARCH=x86|x64] [DEBUG=y|n] [SMALL=y|n] [DEMO=y|n] [PROF=y|n] [PREFIX=..] [BIN=] [PRE=] [SDK=]
    make config|f [PLAT=ios] [ARCH=armv6|armv7|armv7s] [DEBUG=y|n] [SMALL=y|n] [DEMO=y|n] [PROF=y|n] [PREFIX=..] [BIN=] [PRE=] [SDK=5.0|5.1|6.0|..]
    make config|f [PLAT=android] [ARCH=armv5te|armv6] [DEBUG=y|n] [SMALL=y|n] [DEMO=y|n] [PROF=y|n] [PREFIX=..] [BIN=] [PRE=] [NDK=..] [SDK=..]

2. build
    make [all]
    make rebuild|r
    make lipo ARCH1=armv6 ARCH2=armv7 DEBUG=[y|n] SDK=[5.0|5.1|6.0|..] // for ios 

3. update build
    make update|u

4. clean build
    make clean|c

5. install
    make prefix|p
    make install|i

6. make info
    make error|e
    make output|o
    make warning|w

7. make document
    make doc|d

8. help
    make help|h

========================================================================
build:

build for linux-x86 debug
    cd ./tbox
    [make f PLAT=linux ARCH=x86 DEBUG=y] => optional
    make r

build for linux-x64 release
    cd ./tbox
    [make f PLAT=linux ARCH=x64 DEBUG=n] => optional
    make r

build for linux and add cflags and ldflags
    cd ./tbox
    make f PLAT=linux CFLAG="-I." LDFLAG="-L. -lxxx"
    make r

build for mac
    cd ./tbox
    [make f PLAT=mac ARCH=x64] => optional
    make r

build for ios-armv7, using sdk7.1 framework
    cd ./tbox
    make f PLAT=ios ARCH=armv7 SDK=7.1
    make r
    
build for ios armv7 and arm64 universal version
    cd ./tbox
    make lipo ARCH1=armv7 ARCH2=arm64 SDK=7.1

build for android-armv5te, need set the ndk and toolchains directory
    cd ./tbox
    make f PLAT=android ARCH=armv5te NDK=/home/xxxx/android-ndk-r9d BIN=/home/xxxx/android-ndk-r9d/toolchains/arm-linux-androideabi-4.8/prebuilt/darwin-x86_64/bin
    make r
    
build for android-armv6, if ndk and toolchains have been setted
    cd ./tbox
    make f PLAT=android ARCH=armv6
    make r

build for windows using msvc
    cd ./tbox
    run ./msys.bat
    [make f PLAT=msvc ARCH=x86] => optional
    make r

build for windows using mingw and need link libgcc.a from mingw
    run msys
    cd ./tbox
    [make f PLAT=mingw ARCH=x86] => optional
    make r

build for windows using cygwin and need link libgcc.a from mingw
    run cygwin
    cd ./tbox
    [make f PLAT=cygwin ARCH=x86] => optional
    make r

build for windows and custom complier path and prefix and need link libgcc.a from mingw
    run cygwin
    cd ./tbox
    make f PLAT=mingw ARCH=x86 BIN="/home/xxx/bin" PRE="i386-mingw32-"
    make r

========================================================================
usage:

the installed prefix header and library directory after runing "make r":
    inc: ./tbox/pre/inc/$(PLAT)/$(ARCH)/
    lib: ./tbox/pre/lib/$(PLAT)/$(ARCH)/

for linux x64:
    inc:                ./tbox/pre/inc/linux/x64/tbox
    lib[debug]:         ./tbox/pre/lib/linux/x64/libtboxd.a
    lib[release]:       ./tbox/pre/lib/linux/x64/libtboxr.a

    CFLAGS[debug]:      -D__tb_debug__
    CFLAGS[release]:    
    CFLAGS[optional]:   -D__tb_prefix__=\"tag\"
    LDFLAGS[debug]:     -L./tbox/pre/lib/linux/x64/ -ltboxd
    LDFLAGS[release]:   -L./tbox/pre/lib/linux/x64/ -ltboxr

for msvc x86:
    inc:                ./tbox/pre/inc/msvc/x64/tbox
    lib[debug]:         ./tbox/pre/lib/msvc/x64/tboxd.lib
    lib[release]:       ./tbox/pre/lib/msvc/x64/tboxr.lib

    CFLAGS[debug]:      -D__tb_debug__
    CFLAGS[release]:    
    CFLAGS[optional]:   -D__tb_prefix__=\"tag\"
    LDFLAGS[debug]:     -LIBPATH:.\\tbox\\pre\\lib/msvc\\x86 tboxd.lib
    LDFLAGS[release]:   -LIBPATH:.\\tbox\\pre\\lib/msvc\\x86 tboxr.lib

code:

    #include "tbox/tbox.h"

    int main(int argc, char** argv)
    {
        /* init tbox
         *
         * @param priv      the platform private data
         *                  pass JNIEnv* env for android
         *                  pass tb_null for other platform
         * @param data      the memory data for the memory pool, using the native memory if be tb_null
         * @param size      the memory size for the memory pool, using the native memory if be zero
         *
         * for android:     tb_init(jenv, tb_null, 0)
         * for memory pool: tb_init(tb_null, malloc(10 * 1024 * 1024), 10 * 1024 * 1024);
         */
#ifdef __tb_debug__
        // for checking memory leaks and overflow
        if (!tb_init(tb_null, malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;
#else
        if (!tb_init(tb_null, tb_null, 0)) return 0;
#endif

        // print info with tag
        // output: [tag]: hello tbox
        tb_trace_i("hello tbox");

        // print info only for debug
        // output: [tag]: hello tbox
        tb_trace_d("hello tbox"); 

        // print error info
        // output: [tag]: [error]: hello tbox
        tb_trace_e("hello tbox");

        // print warning info
        // output: [tag]: [warning]: hello tbox
        tb_trace_w("hello tbox");

        // print info without prefix tag
        // output: hello tbox
        tb_printf("hello tbox\n");

        // ..

        // exit tbox
        tb_exit();
        return 0;
    }


