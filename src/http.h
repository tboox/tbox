/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		http.h
 *
 */
#ifndef TB_HTTP_H
#define TB_HTTP_H

/* ////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "string.h"

/* ////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_HTTP_PATH_MAX 						(8192)
#define TB_HTTP_LINE_MAX 						(8192)
#define TB_HTTP_HEAD_MAX 						(4096)
#define TB_HTTP_REDIRECT_MAX 					(3)

/* ////////////////////////////////////////////////////////////////////////
 * types
 */

// the http method type
typedef enum __tb_http_method_t
{
	TB_HTTP_METHOD_GET
, 	TB_HTTP_METHOD_POST

}tb_http_method_t;

// the http type
typedef struct __tb_http_t
{
	// the socket
	tplat_handle_t 		socket;

	// the http code
	tb_int_t 			code;

	// the data size
	tb_int_t 			size;

	// is redirect
	tb_int_t 			redirect;
	tb_int_t 			redirect_n;

	// is transfer: chunked
	tb_int_t 			chunked;

	// is stream
	tb_int_t 			stream;
	
	// the url
	tb_char_t 			url[TB_HTTP_PATH_MAX];

	// the process line
	tb_char_t 			line[TB_HTTP_LINE_MAX];

}tb_http_t;

/* ////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_http_t* 			tb_http_create();
void 				tb_http_destroy(tb_http_t* http);

tb_bool_t 			tb_http_open(tb_http_t* http, tb_char_t const* url, tb_char_t const* args, tb_http_method_t method);
void 				tb_http_close(tb_http_t* http);

tb_size_t 			tb_http_size(tb_http_t* http);
tb_size_t 			tb_http_code(tb_http_t* http);
tb_char_t const* 	tb_http_url(tb_http_t* http);
tb_bool_t 			tb_http_stream(tb_http_t* http);

tb_int_t 			tb_http_send_data(tb_http_t* http, tb_byte_t* data, tb_size_t size, tb_bool_t block);
tb_int_t 			tb_http_recv_data(tb_http_t* http, tb_byte_t* data, tb_size_t size, tb_bool_t block);
tb_char_t const* 	tb_http_recv_string(tb_http_t* http, tb_string_t* string);



#endif

