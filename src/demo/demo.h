/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        demo.h
 *
 */
#ifndef TB_DEMO_H
#define TB_DEMO_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../tbox/tbox.h"
#include <stdio.h>
#include <stdlib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the main decl
#define TB_DEMO_MAIN_DECL(name)         tb_int_t tb_demo_##name##_main(tb_int_t argc, tb_char_t** argv)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

// database
TB_DEMO_MAIN_DECL(database_sql);

// regex
TB_DEMO_MAIN_DECL(regex);

// xml
TB_DEMO_MAIN_DECL(xml_reader);
TB_DEMO_MAIN_DECL(xml_writer);
TB_DEMO_MAIN_DECL(xml_document);

// libc
TB_DEMO_MAIN_DECL(libc_time);
TB_DEMO_MAIN_DECL(libc_wchar);
TB_DEMO_MAIN_DECL(libc_string);
TB_DEMO_MAIN_DECL(libc_stdlib);

// libm
TB_DEMO_MAIN_DECL(libm_float);
TB_DEMO_MAIN_DECL(libm_double);
TB_DEMO_MAIN_DECL(libm_integer);

// asio
TB_DEMO_MAIN_DECL(asio_dns);
TB_DEMO_MAIN_DECL(asio_http);
TB_DEMO_MAIN_DECL(asio_httpd);
TB_DEMO_MAIN_DECL(asio_aiopc);
TB_DEMO_MAIN_DECL(asio_aiopd);
TB_DEMO_MAIN_DECL(asio_aicpc);
TB_DEMO_MAIN_DECL(asio_aicpd);

// math
TB_DEMO_MAIN_DECL(math_fixed);
TB_DEMO_MAIN_DECL(math_random);

// utils
TB_DEMO_MAIN_DECL(utils_url);
TB_DEMO_MAIN_DECL(utils_crc);
TB_DEMO_MAIN_DECL(utils_md5);
TB_DEMO_MAIN_DECL(utils_sha);
TB_DEMO_MAIN_DECL(utils_bits);
TB_DEMO_MAIN_DECL(utils_dump);
TB_DEMO_MAIN_DECL(utils_option);
TB_DEMO_MAIN_DECL(utils_base32);
TB_DEMO_MAIN_DECL(utils_base64);
TB_DEMO_MAIN_DECL(utils_adler32);

// other
TB_DEMO_MAIN_DECL(other_test);
TB_DEMO_MAIN_DECL(other_charset);

// object
TB_DEMO_MAIN_DECL(object_jcat);
TB_DEMO_MAIN_DECL(object_json);
TB_DEMO_MAIN_DECL(object_bin);
TB_DEMO_MAIN_DECL(object_xml);
TB_DEMO_MAIN_DECL(object_xplist);
TB_DEMO_MAIN_DECL(object_bplist);
TB_DEMO_MAIN_DECL(object_dump);

// stream
TB_DEMO_MAIN_DECL(stream_transfer_pool);
TB_DEMO_MAIN_DECL(stream_async_transfer);
TB_DEMO_MAIN_DECL(stream_async_stream);
TB_DEMO_MAIN_DECL(stream);
TB_DEMO_MAIN_DECL(stream_zip);
TB_DEMO_MAIN_DECL(stream_null);
TB_DEMO_MAIN_DECL(stream_cache);
TB_DEMO_MAIN_DECL(stream_charset);
TB_DEMO_MAIN_DECL(stream_async_stream_zip);
TB_DEMO_MAIN_DECL(stream_async_stream_null);
TB_DEMO_MAIN_DECL(stream_async_stream_cache);
TB_DEMO_MAIN_DECL(stream_async_stream_charset);

// string
TB_DEMO_MAIN_DECL(string_string);
TB_DEMO_MAIN_DECL(string_static_string);

// memory
TB_DEMO_MAIN_DECL(memory_check);
TB_DEMO_MAIN_DECL(memory_fixed_pool);
TB_DEMO_MAIN_DECL(memory_string_pool);
TB_DEMO_MAIN_DECL(memory_large_allocator);
TB_DEMO_MAIN_DECL(memory_small_allocator);
TB_DEMO_MAIN_DECL(memory_default_allocator);
TB_DEMO_MAIN_DECL(memory_memops);
TB_DEMO_MAIN_DECL(memory_buffer);
TB_DEMO_MAIN_DECL(memory_queue_buffer);
TB_DEMO_MAIN_DECL(memory_static_buffer);
TB_DEMO_MAIN_DECL(memory_impl_static_fixed_pool);

// network
#ifdef TB_CONFIG_MODULE_HAVE_NETWORK
TB_DEMO_MAIN_DECL(network_dns);
TB_DEMO_MAIN_DECL(network_url);
TB_DEMO_MAIN_DECL(network_ipv4);
TB_DEMO_MAIN_DECL(network_ipv6);
TB_DEMO_MAIN_DECL(network_ipaddr);
TB_DEMO_MAIN_DECL(network_hwaddr);
TB_DEMO_MAIN_DECL(network_http);
TB_DEMO_MAIN_DECL(network_whois);
TB_DEMO_MAIN_DECL(network_spider);
TB_DEMO_MAIN_DECL(network_cookies);
TB_DEMO_MAIN_DECL(network_impl_date);
#endif

// platform
TB_DEMO_MAIN_DECL(platform_file);
TB_DEMO_MAIN_DECL(platform_lock);
TB_DEMO_MAIN_DECL(platform_path);
TB_DEMO_MAIN_DECL(platform_event);
TB_DEMO_MAIN_DECL(platform_utils);
TB_DEMO_MAIN_DECL(platform_timer);
TB_DEMO_MAIN_DECL(platform_ltimer);
TB_DEMO_MAIN_DECL(platform_atomic);
TB_DEMO_MAIN_DECL(platform_process);
TB_DEMO_MAIN_DECL(platform_barrier);
TB_DEMO_MAIN_DECL(platform_atomic64);
TB_DEMO_MAIN_DECL(platform_ifaddrs);
TB_DEMO_MAIN_DECL(platform_hostname);
TB_DEMO_MAIN_DECL(platform_processor);
TB_DEMO_MAIN_DECL(platform_backtrace);
TB_DEMO_MAIN_DECL(platform_directory);
TB_DEMO_MAIN_DECL(platform_exception);
TB_DEMO_MAIN_DECL(platform_semaphore);
TB_DEMO_MAIN_DECL(platform_cache_time);
TB_DEMO_MAIN_DECL(platform_environment);
TB_DEMO_MAIN_DECL(platform_thread_pool);
TB_DEMO_MAIN_DECL(platform_thread_store);

// container
TB_DEMO_MAIN_DECL(container_heap);
TB_DEMO_MAIN_DECL(container_stack);
TB_DEMO_MAIN_DECL(container_vector);
TB_DEMO_MAIN_DECL(container_hash_map);
TB_DEMO_MAIN_DECL(container_hash_set);
TB_DEMO_MAIN_DECL(container_queue);
TB_DEMO_MAIN_DECL(container_circle_queue);
TB_DEMO_MAIN_DECL(container_list);
TB_DEMO_MAIN_DECL(container_list_entry);
TB_DEMO_MAIN_DECL(container_single_list);
TB_DEMO_MAIN_DECL(container_single_list_entry);
TB_DEMO_MAIN_DECL(container_bloom_filter);

// algorithm
TB_DEMO_MAIN_DECL(algorithm_find);
TB_DEMO_MAIN_DECL(algorithm_sort);

#endif


