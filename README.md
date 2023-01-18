<div align="center">

  <a href="https://docs.tboox.org">
    <img width="160" heigth="160" src="https://docs.tboox.org/assets/img/logo_text.png">
  </a>

  <div>
    <a href="https://github.com/tboox/tbox/actions?query=workflow%3AWindows">
      <img src="https://img.shields.io/github/actions/workflow/status/tboox/tbox/windows.yml?branch=dev&style=flat-square&logo=windows" alt="github-ci" />
    </a>
    <a href="https://github.com/tboox/tbox/actions?query=workflow%3ALinux">
      <img src="https://img.shields.io/github/actions/workflow/status/tboox/tbox/linux.yml?branch=dev&style=flat-square&logo=linux" alt="github-ci" />
    </a>
    <a href="https://github.com/tboox/tbox/actions?query=workflow%3AmacOS">
      <img src="https://img.shields.io/github/actions/workflow/status/tboox/tbox/macos.yml?branch=dev&style=flat-square&logo=apple" alt="github-ci" />
    </a>
    <a href="https://github.com/tboox/tbox/actions?query=workflow%3AAndroid">
      <img src="https://img.shields.io/github/actions/workflow/status/tboox/tbox/android.yml?branch=dev&style=flat-square&logo=android" alt="github-ci" />
    </a>
    <a href="https://github.com/tboox/tbox/releases">
      <img src="https://img.shields.io/github/release/tboox/tbox.svg?style=flat-square" alt="Github All Releases" />
    </a>
  </div>
  <div>
    <a href="https://github.com/tboox/tbox/blob/master/LICENSE.md">
      <img src="https://img.shields.io/github/license/tboox/tbox.svg?colorB=f48041&style=flat-square" alt="license" />
    </a>
    <a href="https://www.reddit.com/r/tboox/">
      <img src="https://img.shields.io/badge/chat-on%20reddit-ff3f34.svg?style=flat-square" alt="Reddit" />
    </a>
    <a href="https://gitter.im/tboox/tboox?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge">
      <img src="https://img.shields.io/gitter/room/tboox/tboox.svg?style=flat-square&colorB=96c312" alt="Gitter" />
    </a>
    <a href="https://t.me/tbooxorg">
      <img src="https://img.shields.io/badge/chat-on%20telegram-blue.svg?style=flat-square" alt="Telegram" />
    </a>
    <a href="https://jq.qq.com/?_wv=1027&k=5hpwWFv">
      <img src="https://img.shields.io/badge/chat-on%20QQ-ff69b4.svg?style=flat-square" alt="QQ" />
    </a>
    <a href="https://tboox.org/donation/">
      <img src="https://img.shields.io/badge/donate-us-orange.svg?style=flat-square" alt="Donate" />
    </a>
  </div>

  <p>A glib-like cross-platform C library</p>
</div>

## Supporting the project

Support this project by becoming a sponsor. Your logo will show up here with a link to your website. 🙏 [[Become a sponsor](https://docs.tboox.org/#/about/sponsor)]

<a href="https://opencollective.com/tbox#backers" target="_blank"><img src="https://opencollective.com/tbox/backers.svg?width=890"></a>

## Introduction ([中文](/README_zh.md))

TBOX is a glib-like cross-platform C library that is simple to use yet powerful in nature.

The project focuses on making C development easier and provides many modules (.e.g stream, coroutine, regex, container, algorithm ...),
so that any developer can quickly pick it up and enjoy the productivity boost when developing in C language.

It supports the following platforms: Windows, Macosx, Linux, Android, iOS, *BSD and etc.

And it provides many compiling options using [xmake](https://github.com/xmake-io/xmake):

* Release: Disable debug information, assertion, memory checking and enable optimization.
* Debug: Enable debug information, assertion, memory checking and disable optimization.
* Small: Disable all extensional modules and enable space optimization.
* Micro: compiling micro library (~64K) for the embed system.

If you want to know more, please refer to: [Documents](https://docs.tboox.org/#/getting_started), [Github](https://github.com/tboox/tbox) and [Gitee](https://gitee.com/tboox/tbox)

## Features

#### The stream library

- Supports file, data, http and socket source
- Supports the stream filter for gzip, charset and...
- Implements stream transfer
- Implements the static buffer stream for parsing data
- Supports coroutine and implements asynchronous operation

#### The coroutine library

- Provides high-performance coroutine switch
- Supports arm, arm64, x86, x86_64 ..
- Provides channel interfaces
- Provides semaphore and lock interfaces
- Supports io socket and stream operation in coroutine
- Provides some io servers (http ..) using coroutine
- Provides stackfull and stackless coroutines
- Support epoll, kqueue, poll, select and IOCP
- Support to wait pipe, socket and process in coroutine and poller at same time

#### The database library

- Supports mysql and sqlite3 database and enumerates data using the iterator mode

#### The xml parser library

- Supports DOM and SAX mode and Supports xpath

#### The serialization and deserialization library

- Supports xml, json, bplist, xplist, binary formats

#### The memory library

- Implements some memory pools for optimizing memory
- Supports fast memory error detecting. it can detect the following types of bugs for the debug mode:
  - out-of-bounds accesses to heap and globals
  - use-after-free
  - double-free, invalid free
  - memory leaks

#### The container library

- Implements hash table, single list, double list, vector, stack, queue
  and min/max heap. Supports iterator mode for algorithm

#### The algorithm library

- Uses the iterator mode
- Implements find, binary find and reverse find algorithm
- Implements sort, bubble sort, quick sort, heap sort and insert sort algorithm
- Implements count, walk items, reverse walk items, for_all and rfor_all

#### The network library

- Implements dns(cached)
- Implements ssl(openssl, polarssl, mbedtls)
- Implements http
- Implements cookies
- Supports ipv4, ipv6
- Supports coroutine

#### The platform library

- Implements timer, fast and low precision timer
- Implements atomic and atomic64 operation
- Implements spinlock, mutex, event, semaphore, thread and thread pool
- Implements file, socket operation
- Implements poller using epoll, poll, select, kqueue ...
- Implements switch context interfaces for coroutine

#### The charset library

- Supports utf8, utf16, gbk, gb2312, uc2 and uc4
- Supports big endian and little endian mode

#### The zip library

- Supports gzip, zlibraw, zlib formats using the zlib library if exists
- Implements lzsw, lz77 and rlc algorithm

#### The utils library

- Implements base32, base64 encoder and decoder
- Implements assert and trace output for the debug mode
- Implements bits operation for parsing u8, u16, u32, u64 data

#### The math library

- Implements random generator
- Implements fast fixed-point calculation, Supports 6-bits, 16-bits, 30-bits fixed-point number

#### The libc library

- Implements lightweight libc library interfaces, the interface name contains `tb_xxx` prefix for avoiding conflict
- Implements strixxx strrxxx wcsixxx wcsrxxx interface extension
- Optimizes some frequently-used interface, .e.g. memset, memcpy, strcpy ...
- Implements `memset_u16`, `memset_u32`, `memset_u64` extension interfaces

#### The libm library

- Implements lightweight libm library interfaces, the interface name contains `tb_xxx` prefix for avoiding conflict
- Supports float and double type

#### The regex library

- Supports match and replace
- Supports global/multiline/caseless mode
- Uses pcre, pcre2 and posix regex modules

#### The hash library

- Implements crc32, adler32, md5 and sha1 hash algorithm
- Implements some string hash algorithms (.e.g bkdr, fnv32, fnv64, sdbm, djb2, rshash, aphash ...)
- Implements uuid generator

## Projects

Some projects using tbox:

* [gbox](https://github.com/tboox/gbox)
* [vm86](https://github.com/tboox/vm86)
* [xmake](http://www.xmake.io)
* [itrace](https://github.com/tboox/itrace)
* [more](https://github.com/tboox/tbox/wiki/tbox-projects)

## Build (xmake)

Please install xmake first: [xmake](https://github.com/xmake-io/xmake)

```console
# build for the host platform
$ cd ./tbox
$ xmake

# build for the mingw platform
$ cd ./tbox
$ xmake f -p mingw --sdk=/home/mingwsdk
$ xmake

# build for the iphoneos platform
$ cd ./tbox
$ xmake f -p iphoneos
$ xmake

# build for the android platform
$ cd ./tbox
$ xmake f -p android --ndk=xxxxx
$ xmake

# build for the linux cross-platform
$ cd ./tbox
$ xmake f -p linux --sdk=/home/sdk # --bin=/home/sdk/bin
$ xmake
```

## Build (xmake.sh)

```console
$ ./configure
$ make
```

## Example

```c
#include "tbox/tbox.h"

int main(int argc, char** argv) {
    if (!tb_init(tb_null, tb_null)) return 0;

    tb_vector_ref_t vector = tb_vector_init(0, tb_element_str(tb_true));
    if (vector) {
        tb_vector_insert_tail(vector, "hello");
        tb_vector_insert_tail(vector, "tbox");

        tb_for_all (tb_char_t const*, cstr, vector) {
            tb_trace_i("%s", cstr);
        }
        tb_vector_exit(vector);
    }
    tb_exit();
    return 0;
}
```

## Technical Support

You can also consider sponsoring us to get technical support services, [[Become a sponsor](https://docs.tboox.org/#/about/sponsor)]

## Contacts

* Email：[waruqi@gmail.com](mailto:waruqi@gmail.com)
* Homepage：[tboox.org](https://tboox.org)
* Community：[/r/tboox on reddit](https://www.reddit.com/r/tboox/)
* ChatRoom：[Chat on telegram](https://t.me/tbooxorg), [Chat on gitter](https://gitter.im/tboox/tboox?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)
* QQ Group: 343118190(full), 662147501
* Wechat Public: tboox-os

