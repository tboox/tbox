/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_platform_process_main(tb_int_t argc, tb_char_t** argv)
{
    // init environments
    tb_char_t const* envp[] = 
    {
        "path=/tmp;z:\\"
    ,   tb_null
    };

    // run bash
    tb_long_t ok = tb_process_run(argv[1], (tb_char_t const**)(argv + 1), envp);
//    tb_long_t ok = tb_process_run_cmd("ccache xcrun -sdk macosx clang -c -Wno-error=deprecated-declarations -fno-strict-aliasing -fomit-frame-pointer -fvisibility=hidden -Wall -Werror -O3 -msse2 -msse3 -mssse3 -mmmx -std=c99 -Ibuild -Ibuild/tbox -D_GNU_SOURCE=1 -D_REENTRANT -D__tb_prefix__=\"tbox\" -Ipkg/zlib.pkg/inc/macosx -Ipkg/zlib.pkg/inc -Ipkg/sqlite3.pkg/inc/macosx -Ipkg/sqlite3.pkg/inc -Ipkg/openssl.pkg/inc/macosx -Ipkg/openssl.pkg/inc -Ipkg/polarssl.pkg/inc/macosx -Ipkg/polarssl.pkg/inc -Ipkg/pcre2.pkg/inc/macosx -Ipkg/pcre2.pkg/inc -DPCRE2_CODE_UNIT_WIDTH=8 -Ipkg/pcre.pkg/inc/macosx -Ipkg/pcre.pkg/inc -arch x86_64 -fpascal-strings -fmessage-length=0 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.11.sdk -I/usr/include -I/usr/local/include -Qunused-arguments -o build/.objs/tbox/src/tbox/libc/string/memmem.o src/tbox/libc/string/memmem.c", envp);

    // trace
    tb_trace_i("run: %s: %ld", argv[1], ok);

    // ok
    return 0;
}
