========================================================================
The Treasure Box Library

TBOX is a mutli-platform c library for unix, windows, mac, ios, android, etc.

It includes asio, stream, network, container, algorithm, object, memory, database, string, charset, math, libc, libm, utils and other library modules.

========================================================================
features:

the stream io library
	supports file, data, http and socket source
	supports the stream filter for gzip charset and ..
	implements the multi-stream transfer using asio	

the asynchronous io library
	supports reactor and proactor mode
	using epoll, poll, select ,kqueue and iocp os system api

the database library
	supports mysql and sqlite3 database and enumerates data using the iterator mode

the xml parser library. 
	supports DOM and SAX mode and supports xpath.

the serialization and deserialization library. 
	supports xml, json, bplist, xplist, binary formats

the memory library
	implements some memory pools for optimizating memory.
	supports fast memory error detecting. it can detect the following types of bugs for the debug mode:
	1. out-of-bounds accesses to heap and globals
	2. use-after-free
	3. double-free, invalid free
	4. memory leaks

the container library
	implements hash table, single list, double list, vector, stack, queue
	and min/max heap. supports iterator mode for algorithm.

the algorithm library
	using the iterator mode
	implements find, binary find and reverse find algorithm.
	implements sort, bubble sort, quick sort, heap sort and insert sort algorithm. 
	implements count, walk items, reverse walk items, for_all and rfor_all.

the network library
	implements dns(cached), ssl(openssl and polarssl), http and cookies
	supports asynchronous io mode for dns, ssl and http using the asio and stream library

the platform library
	implements timer, faster and lower precision timer
	implements atomic and atomic64 operation
	implements spinlock, mutex, event, semaphore, thread and thread pool 
	implements file, socket operation

the charset library
	supports utf8, utf16, gbk, gb2312, uc2 and uc4
	supports big endian and little endian mode

the zip library
	supports gzip, zlibraw, zlib formats using the zlib library if exists
	implements lzsw, lz77 and rlc algorithm

the utils library
	implements base32, base64, md5 and sha algorithm
	implements assert and trace output for the debug mode

the math library
	implements random generator
	implements fast fixed-point calculation, supports 6-bits, 16-bits, 30-bits fixed-point number

the libc library
	implements lightweight libc library interfaces, the interface name contains tb_xxx prefix for avoiding conflict.
	implements strixxx strrxxx wcsixxx wcsrxxx interface extension.
	optimizates some frequently-used interface, .e.g. memset, memcpy, strcpy ... 
	implements memset_u16, memset_u32, memset_u64 extension interfaces.

the libm library
	implements lightweight libm library interfaces, the interface name contains tb_xxx prefix for avoiding conflict.
	supports float and double type.

========================================================================
contact:

email:   	waruqi@gmail.com
source:  	git@github.com:waruqi/tbox.git
website: 	http://www.tboox.org or http://www.tboox.net
download: 	https://github.com/waruqi/tbox/archive/master.zip
qq(group): 	343118190

