---
search: english
---

# The Treasure Box Library 

[![Build Status](https://api.travis-ci.org/tboox/tbox.svg)](https://travis-ci.org/tboox/tbox) [![Build status](https://ci.appveyor.com/api/projects/status/0pbnea6d059odbe8/branch/master?svg=true)](https://ci.appveyor.com/project/waruqi/tbox/branch/master) [![codecov](https://codecov.io/gh/tboox/tbox/branch/master/graph/badge.svg)](https://codecov.io/gh/tboox/tbox) [![Join the chat at https://gitter.im/waruqi/tboox](https://badges.gitter.im/waruqi/tboox.svg)](https://gitter.im/waruqi/tboox?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge) [![donate](http://tboox.org/static/img/donate.svg)](http://tboox.org/donation/)

## Introduction

TBOX is a glib-like cross-platform C library that is simple to use yet powerful in nature.

The project focuses on making C development easier and provides many modules (.e.g stream, asio, regex, container, algorithm ...), 
so that any developer can quickly pick it up and enjoy the productivity boost when developing in C language.

It supports the following platforms:

- Windows
- Macosx
- Linux
- Android
- iOS

And it provides many compiling options using [xmake](http://www.xmake.io):

* Release: Disable debug information, assertion, memory checking and enable optimization.
* Debug: Enable debug information, assertion, memory checking and disable optimization.
* Small: Disable all extensional modules and enable space optimization.
* Micro: compiling micro library (~64K) for the embed system.

If you want to know more, please refer to:

* [Documents](https://github.com/waruqi/tbox/wiki/documents)
* [Github](https://github.com/waruqi/tbox)
* [HomePage](http://tboox.org)

## Features

#### Stream

- Supports file, data, http and socket source
- Supports the stream filter for gzip, charset and...
- Implements stream transfer
- Implements the static buffer stream for parsing data
- Supports coroutine and implements asynchronous operation

#### Coroutine

- Provides high-performance coroutine switch(refer to [reports](http://tboox.org/2016/10/28/benchbox-coroutine/))
- Supports arm, arm64, x86, x86_64 ..
- Provides channel interfaces
- Provides semaphore and lock interfaces
- Supports io socket and stream operation in coroutine
- Provides some io servers (http ..) using coroutine
- Provides stackfull and stackless coroutines

#### Database

- Supports mysql and sqlite3 database and enumerates data using the iterator mode

#### Xml 

- Supports DOM and SAX mode and Supports xpath

#### Serialization 

- Supports xml, json, bplist, xplist, binary formats

#### Memory

- Implements some memory pools for optimizing memory
- Supports fast memory error detecting. it can detect the following types of bugs for the debug mode:
  - out-of-bounds accesses to heap and globals
  - use-after-free
  - double-free, invalid free
  - memory leaks

#### Container

- Implements hash table, single list, double list, vector, stack, queue
  and min/max heap. Supports iterator mode for algorithm

#### Algorithm

- Uses the iterator mode
- Implements find, binary find and reverse find algorithm
- Implements sort, bubble sort, quick sort, heap sort and insert sort algorithm
- Implements count, walk items, reverse walk items, for_all and rfor_all

#### Network

- Implements dns(cached)
- Implements ssl(openssl, polarssl, mbedtls)
- Implements http
- Implements cookies
- Supports ipv4, ipv6
- Supports coroutine

#### Platform

- Implements timer, fast and low precision timer
- Implements atomic and atomic64 operation
- Implements spinlock, mutex, event, semaphore, thread and thread pool 
- Implements file, socket operation
- Implements poller using epoll, poll, select, kqueue ...
- Implements switch context interfaces for coroutine

#### Charset

- Supports utf8, utf16, gbk, gb2312, uc2 and uc4
- Supports big endian and little endian mode

#### Zip

- Supports gzip, zlibraw, zlib formats using the zlib library if exists
- Implements lzsw, lz77 and rlc algorithm

#### Utils

- Implements base32, base64 encoder and decoder
- Implements assert and trace output for the debug mode
- Implements bits operation for parsing u8, u16, u32, u64 data

#### Math

- Implements random generator
- Implements fast fixed-point calculation, Supports 6-bits, 16-bits, 30-bits fixed-point number

#### Libc

- Implements lightweight libc library interfaces, the interface name contains `tb_xxx` prefix for avoiding conflict
- Implements strixxx strrxxx wcsixxx wcsrxxx interface extension
- Optimizes some frequently-used interface, .e.g. memset, memcpy, strcpy ... 
- Implements `memset_u16`, `memset_u32`, `memset_u64` extension interfaces

#### Libm

- Implements lightweight libm library interfaces, the interface name contains `tb_xxx` prefix for avoiding conflict
- Supports float and double type

#### Regex

- Supports match and replace
- Supports global/multiline/caseless mode
- Uses pcre, pcre2 and posix regex modules

#### Hash

- Implements crc32, adler32, md5 and sha1 hash algorithm
- Implements some string hash algorithms (.e.g bkdr, fnv32, fnv64, sdbm, djb2, rshash, aphash ...)
- Implements uuid generator


