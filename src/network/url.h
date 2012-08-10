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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		url.h
 * @ingroup 	network
 *
 */
#ifndef TB_NETWORK_URL_H
#define TB_NETWORK_URL_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "ipv4.h"
#include "../string/string.h"

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */
#ifdef TB_CONFIG_MEMORY_MODE_SMALL
# 	define TB_URL_HOST_MAX 		(32)
# 	define TB_URL_PATH_MAX 		(1024)
#else
# 	define TB_URL_HOST_MAX 		(128)
# 	define TB_URL_PATH_MAX 		(4096)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the url protocol type
typedef enum __tb_url_proto_t
{
	TB_URL_PROTO_NULL 	= 0
,	TB_URL_PROTO_FILE 	= 1 	//!< file://... or files://...
,	TB_URL_PROTO_SOCK 	= 2 	//!< sock://... or socks://...
,	TB_URL_PROTO_HTTP 	= 3 	//!< http://... or https://...
,	TB_URL_PROTO_RTSP 	= 4 	//!< rtsp://... or rtsps://...

}tb_url_proto_t;

// the url type
typedef struct __tb_url_t
{
	// the protocol 
	tb_uint16_t 	poto: 	6;

	// is ssl?
	tb_uint16_t 	bssl: 	1;

	// is win?
	tb_uint16_t 	bwin: 	1;

	// the windows prefix 
	tb_uint16_t 	pwin: 	8;

	// the port
	tb_uint16_t 	port;

	// the ipv4
	tb_ipv4_t 		ipv4;

	// the host
	tb_sstring_t 	host;

	// the path
	tb_sstring_t 	path;

	// the urls
	tb_pstring_t 	urls;

	// the args
	tb_pstring_t 	args;

	// the data
	tb_char_t 		data[TB_URL_HOST_MAX + TB_URL_PATH_MAX];

}tb_url_t;


/* ///////////////////////////////////////////////////////////////////////
 * the interfaces
 */

// init & exit
tb_bool_t 			tb_url_init(tb_url_t* url);
tb_void_t 			tb_url_exit(tb_url_t* url);

// modifiors
tb_void_t 			tb_url_clear(tb_url_t* url);

// url
tb_char_t const* 	tb_url_get(tb_url_t* url);
tb_bool_t 			tb_url_set(tb_url_t* url, tb_char_t const* u);
tb_void_t 			tb_url_cpy(tb_url_t* url, tb_url_t const* u);

// ssl
tb_bool_t 			tb_url_ssl_get(tb_url_t const* url);
tb_void_t 			tb_url_ssl_set(tb_url_t* url, tb_bool_t bssl);

// poto
tb_size_t 			tb_url_poto_get(tb_url_t const* url);
tb_void_t 			tb_url_poto_set(tb_url_t* url, tb_size_t poto);

// port
tb_size_t 			tb_url_port_get(tb_url_t const* url);
tb_void_t 			tb_url_port_set(tb_url_t* url, tb_size_t port);

// ipv4
tb_ipv4_t const* 	tb_url_ipv4_get(tb_url_t const* url);
tb_void_t 			tb_url_ipv4_set(tb_url_t* url, tb_ipv4_t const* ipv4);

// host
tb_char_t const* 	tb_url_host_get(tb_url_t const* url);
tb_void_t 			tb_url_host_set(tb_url_t* url, tb_char_t const* host);

// path
tb_char_t const* 	tb_url_path_get(tb_url_t const* url);
tb_void_t 			tb_url_path_set(tb_url_t* url, tb_char_t const* path);

// args
tb_char_t const* 	tb_url_args_get(tb_url_t const* url);
tb_void_t 			tb_url_args_set(tb_url_t* url, tb_char_t const* args);




#endif
