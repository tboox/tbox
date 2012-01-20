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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		url.h
 *
 */
#ifndef TB_NETWORK_URL_H
#define TB_NETWORK_URL_H

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "../string/string.h"

/* /////////////////////////////////////////////////////////
 * the types
 */

// the url protocol type
typedef enum __tb_url_proto_t
{
	TB_URL_PROTO_NULL 	= 0
,	TB_URL_PROTO_FILE 	= 1 	//!< file://... or files://...
,	TB_URL_PROTO_SOCK 	= 2 	//!< sock://... or socks://...
,	TB_URL_PROTO_HTTP 	= 3 	//!< http://... or https://...

}tb_url_proto_t;

// the url type
typedef struct __tb_url_t
{
	// the protocol 
	tb_uint16_t 	poto: 	15;

	// is ssl?
	tb_uint16_t 	bssl: 	1;

	// the port
	tb_uint16_t 	port;

	// the host
	tb_pstring_t 	host;

	// the path
	tb_pstring_t 	path;

	// the path
	tb_pstring_t 	data;

}tb_url_t;


/* /////////////////////////////////////////////////////////
 * the interfaces
 */

// init & exit
tb_bool_t 			tb_url_init(tb_url_t* url);
tb_void_t 			tb_url_exit(tb_url_t* url);

// accessors
tb_char_t const* 	tb_url_cstr(tb_url_t const* url);
tb_size_t 			tb_url_size(tb_url_t const* url);

// modifiors
tb_void_t 			tb_url_clear(tb_url_t* url);

// url
tb_char_t const* 	tb_url_get(tb_url_t const* url);
tb_bool_t 			tb_url_set(tb_url_t* url, tb_char_t const* u);

// ssl
tb_bool_t 			tb_url_ssl_get(tb_url_t const* url);
tb_void_t 			tb_url_ssl_set(tb_url_t* url, tb_bool_t bssl);

// poto
tb_size_t 			tb_url_poto_get(tb_url_t const* url);
tb_void_t 			tb_url_poto_set(tb_url_t* url, tb_size_t poto);

// port
tb_size_t 			tb_url_port_get(tb_url_t const* url);
tb_void_t 			tb_url_port_set(tb_url_t* url, tb_size_t port);

// host
tb_char_t const* 	tb_url_host_get(tb_url_t const* url);
tb_void_t 			tb_url_host_set(tb_url_t* url, tb_char_t const* host);

// path
tb_char_t const* 	tb_url_path_get(tb_url_t const* url);
tb_void_t 			tb_url_path_set(tb_url_t* url, tb_char_t const* path);



#endif
