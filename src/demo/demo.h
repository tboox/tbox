/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
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
#include <stdlib.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the main decl
#define TB_DEMO_MAIN_DECL(name)         tb_int_t tb_demo_##name##_main(tb_int_t argc, tb_char_t** argv)

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
__tb_extern_c_enter__

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
TB_DEMO_MAIN_DECL(libc_mbstowcs);
TB_DEMO_MAIN_DECL(libc_wcstombs);

// libm
TB_DEMO_MAIN_DECL(libm_float);
TB_DEMO_MAIN_DECL(libm_double);
TB_DEMO_MAIN_DECL(libm_integer);

// math
TB_DEMO_MAIN_DECL(math_fixed);
TB_DEMO_MAIN_DECL(math_random);

// utils
TB_DEMO_MAIN_DECL(utils_url);
TB_DEMO_MAIN_DECL(utils_bits);
TB_DEMO_MAIN_DECL(utils_dump);
TB_DEMO_MAIN_DECL(utils_option);
TB_DEMO_MAIN_DECL(utils_base32);
TB_DEMO_MAIN_DECL(utils_base64);

// hash
TB_DEMO_MAIN_DECL(hash_md5);
TB_DEMO_MAIN_DECL(hash_sha);
TB_DEMO_MAIN_DECL(hash_uuid);
TB_DEMO_MAIN_DECL(hash_djb2);
TB_DEMO_MAIN_DECL(hash_sdbm);
TB_DEMO_MAIN_DECL(hash_crc8);
TB_DEMO_MAIN_DECL(hash_crc16);
TB_DEMO_MAIN_DECL(hash_crc32);
TB_DEMO_MAIN_DECL(hash_fnv32);
TB_DEMO_MAIN_DECL(hash_fnv64);
TB_DEMO_MAIN_DECL(hash_adler32);
TB_DEMO_MAIN_DECL(hash_benchmark);

// other
TB_DEMO_MAIN_DECL(other_test);
TB_DEMO_MAIN_DECL(other_test_cpp);
TB_DEMO_MAIN_DECL(other_charset);

// object
TB_DEMO_MAIN_DECL(object_jcat);
TB_DEMO_MAIN_DECL(object_json);
TB_DEMO_MAIN_DECL(object_bin);
TB_DEMO_MAIN_DECL(object_xml);
TB_DEMO_MAIN_DECL(object_xplist);
TB_DEMO_MAIN_DECL(object_bplist);
TB_DEMO_MAIN_DECL(object_dump);
TB_DEMO_MAIN_DECL(object_writer);

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
TB_DEMO_MAIN_DECL(network_dns);
TB_DEMO_MAIN_DECL(network_url);
TB_DEMO_MAIN_DECL(network_ipv4);
TB_DEMO_MAIN_DECL(network_ipv6);
TB_DEMO_MAIN_DECL(network_unixaddr);
TB_DEMO_MAIN_DECL(network_unix_echo_server);
TB_DEMO_MAIN_DECL(network_unix_echo_client);
TB_DEMO_MAIN_DECL(network_ping);
TB_DEMO_MAIN_DECL(network_ipaddr);
TB_DEMO_MAIN_DECL(network_hwaddr);
TB_DEMO_MAIN_DECL(network_http);
TB_DEMO_MAIN_DECL(network_whois);
TB_DEMO_MAIN_DECL(network_cookies);
TB_DEMO_MAIN_DECL(network_impl_date);

// platform
TB_DEMO_MAIN_DECL(platform_file);
TB_DEMO_MAIN_DECL(platform_lock);
TB_DEMO_MAIN_DECL(platform_path);
TB_DEMO_MAIN_DECL(platform_sched);
TB_DEMO_MAIN_DECL(platform_event);
TB_DEMO_MAIN_DECL(platform_utils);
TB_DEMO_MAIN_DECL(platform_timer);
TB_DEMO_MAIN_DECL(platform_ltimer);
TB_DEMO_MAIN_DECL(platform_atomic);
TB_DEMO_MAIN_DECL(platform_atomic32);
TB_DEMO_MAIN_DECL(platform_atomic64);
TB_DEMO_MAIN_DECL(platform_stdfile);
TB_DEMO_MAIN_DECL(platform_process);
TB_DEMO_MAIN_DECL(platform_ifaddrs);
TB_DEMO_MAIN_DECL(platform_filelock);
TB_DEMO_MAIN_DECL(platform_addrinfo);
TB_DEMO_MAIN_DECL(platform_hostname);
TB_DEMO_MAIN_DECL(platform_pipe_pair);
TB_DEMO_MAIN_DECL(platform_named_pipe);
TB_DEMO_MAIN_DECL(platform_backtrace);
TB_DEMO_MAIN_DECL(platform_directory);
TB_DEMO_MAIN_DECL(platform_exception);
TB_DEMO_MAIN_DECL(platform_semaphore);
TB_DEMO_MAIN_DECL(platform_cache_time);
TB_DEMO_MAIN_DECL(platform_environment);
TB_DEMO_MAIN_DECL(platform_thread);
TB_DEMO_MAIN_DECL(platform_thread_pool);
TB_DEMO_MAIN_DECL(platform_thread_local);
TB_DEMO_MAIN_DECL(platform_poller_pipe);
TB_DEMO_MAIN_DECL(platform_poller_client);
TB_DEMO_MAIN_DECL(platform_poller_server);
TB_DEMO_MAIN_DECL(platform_poller_process);
TB_DEMO_MAIN_DECL(platform_poller_fwatcher);
TB_DEMO_MAIN_DECL(platform_context);
TB_DEMO_MAIN_DECL(platform_fwatcher);

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

// coroutine
TB_DEMO_MAIN_DECL(coroutine_dns);
TB_DEMO_MAIN_DECL(coroutine_nest);
TB_DEMO_MAIN_DECL(coroutine_lock);
TB_DEMO_MAIN_DECL(coroutine_ping);
TB_DEMO_MAIN_DECL(coroutine_sleep);
TB_DEMO_MAIN_DECL(coroutine_spider);
TB_DEMO_MAIN_DECL(coroutine_stream);
TB_DEMO_MAIN_DECL(coroutine_switch);
TB_DEMO_MAIN_DECL(coroutine_channel);
TB_DEMO_MAIN_DECL(coroutine_semaphore);
TB_DEMO_MAIN_DECL(coroutine_thread);
TB_DEMO_MAIN_DECL(coroutine_pipe);
TB_DEMO_MAIN_DECL(coroutine_process);
TB_DEMO_MAIN_DECL(coroutine_process_pipe);
TB_DEMO_MAIN_DECL(coroutine_fwatcher);
TB_DEMO_MAIN_DECL(coroutine_echo_client);
TB_DEMO_MAIN_DECL(coroutine_echo_server);
TB_DEMO_MAIN_DECL(coroutine_unix_echo_client);
TB_DEMO_MAIN_DECL(coroutine_unix_echo_server);
TB_DEMO_MAIN_DECL(coroutine_file_client);
TB_DEMO_MAIN_DECL(coroutine_file_server);
TB_DEMO_MAIN_DECL(coroutine_http_server);

// stackless coroutine
TB_DEMO_MAIN_DECL(lo_coroutine_nest);
TB_DEMO_MAIN_DECL(lo_coroutine_lock);
TB_DEMO_MAIN_DECL(lo_coroutine_sleep);
TB_DEMO_MAIN_DECL(lo_coroutine_switch);
TB_DEMO_MAIN_DECL(lo_coroutine_process);
TB_DEMO_MAIN_DECL(lo_coroutine_process_pipe);
TB_DEMO_MAIN_DECL(lo_coroutine_echo_server);
TB_DEMO_MAIN_DECL(lo_coroutine_echo_client);
TB_DEMO_MAIN_DECL(lo_coroutine_file_server);
TB_DEMO_MAIN_DECL(lo_coroutine_file_client);
TB_DEMO_MAIN_DECL(lo_coroutine_http_server);

__tb_extern_c_leave__

#endif


