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
 * @file        state.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "state.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

tb_char_t const* tb_state_cstr(tb_size_t state)
{
    // done
    switch (state)
    {
    case TB_STATE_OK:                           return "ok";
    case TB_STATE_FAILED:                       return "failed";
    case TB_STATE_KILLED:                       return "killed";
    case TB_STATE_PAUSED:                       return "paused";
    case TB_STATE_CLOSED:                       return "closed";
    case TB_STATE_OPENED:                       return "opened";
    case TB_STATE_OPENING:                      return "opening";
    case TB_STATE_PENDING:                      return "pending";
    case TB_STATE_WAITING:                      return "waiting";
    case TB_STATE_WORKING:                      return "working";
    case TB_STATE_KILLING:                      return "killing";
    case TB_STATE_EXITING:                      return "exiting";
    case TB_STATE_PAUSING:                      return "pausing";
    case TB_STATE_TIMEOUT:                      return "timeout";
    case TB_STATE_FINISHED:                     return "finished";
    case TB_STATE_WAIT_FAILED:                  return "wait failed";
    case TB_STATE_POST_FAILED:                  return "post failed";
    case TB_STATE_NOT_SUPPORTED:                return "not supported";
    case TB_STATE_UNKNOWN_ERROR:                return "unknown error";

    case TB_STATE_FILE_NOT_EXISTS:              return "file: not exists";
    case TB_STATE_FILE_OPEN_FAILED:             return "file: open failed";
    case TB_STATE_FILE_UNKNOWN_ERROR:           return "file: unknown error";

    case TB_STATE_SOCK_DNS_FAILED:              return "sock: dns: failed";
    case TB_STATE_SOCK_CONNECT_FAILED:          return "sock: connect: failed";
    case TB_STATE_SOCK_CONNECT_TIMEOUT:         return "sock: connect: timeout";
    case TB_STATE_SOCK_RECV_TIMEOUT:            return "sock: recv: timeout";
    case TB_STATE_SOCK_SEND_TIMEOUT:            return "sock: send: timeout";
    case TB_STATE_SOCK_RECV_FAILED:             return "sock: recv: failed";
    case TB_STATE_SOCK_SEND_FAILED:             return "sock: send: failed";
    case TB_STATE_SOCK_OPEN_FAILED:             return "sock: open: failed";
    case TB_STATE_SOCK_UNKNOWN_ERROR:           return "sock: unknown error";
    case TB_STATE_SOCK_SSL_FAILED:              return "sock: ssl: failed";
    case TB_STATE_SOCK_SSL_TIMEOUT:             return "sock: ssl: timeout";
    case TB_STATE_SOCK_SSL_WANT_READ:           return "sock: ssl: want read";
    case TB_STATE_SOCK_SSL_WANT_WRIT:           return "sock: ssl: want writ";
    case TB_STATE_SOCK_SSL_WAIT_FAILED:         return "sock: ssl: wait failed";
    case TB_STATE_SOCK_SSL_READ_FAILED:         return "sock: ssl: read: failed";
    case TB_STATE_SOCK_SSL_WRIT_FAILED:         return "sock: ssl: writ: failed";
    case TB_STATE_SOCK_SSL_NOT_SUPPORTED:       return "sock: ssl: not supported";
    case TB_STATE_SOCK_SSL_UNKNOWN_ERROR:       return "sock: ssl: unknown error";

    case TB_STATE_HTTP_RESPONSE_204:            return "http: response: 204";
    case TB_STATE_HTTP_RESPONSE_300:            return "http: response: 300";
    case TB_STATE_HTTP_RESPONSE_301:            return "http: response: 301";
    case TB_STATE_HTTP_RESPONSE_302:            return "http: response: 302";
    case TB_STATE_HTTP_RESPONSE_303:            return "http: response: 303";
    case TB_STATE_HTTP_RESPONSE_304:            return "http: response: 304";
    case TB_STATE_HTTP_RESPONSE_305:            return "http: response: 305";
    case TB_STATE_HTTP_RESPONSE_306:            return "http: response: 306";
    case TB_STATE_HTTP_RESPONSE_307:            return "http: response: 307";
    case TB_STATE_HTTP_RESPONSE_400:            return "http: response: 400";
    case TB_STATE_HTTP_RESPONSE_401:            return "http: response: 401";
    case TB_STATE_HTTP_RESPONSE_402:            return "http: response: 402";
    case TB_STATE_HTTP_RESPONSE_403:            return "http: response: 403";
    case TB_STATE_HTTP_RESPONSE_404:            return "http: response: 404";
    case TB_STATE_HTTP_RESPONSE_405:            return "http: response: 405";
    case TB_STATE_HTTP_RESPONSE_406:            return "http: response: 406";
    case TB_STATE_HTTP_RESPONSE_407:            return "http: response: 407";
    case TB_STATE_HTTP_RESPONSE_408:            return "http: response: 408";
    case TB_STATE_HTTP_RESPONSE_409:            return "http: response: 409";
    case TB_STATE_HTTP_RESPONSE_410:            return "http: response: 410";
    case TB_STATE_HTTP_RESPONSE_411:            return "http: response: 411";
    case TB_STATE_HTTP_RESPONSE_412:            return "http: response: 412";
    case TB_STATE_HTTP_RESPONSE_413:            return "http: response: 413";
    case TB_STATE_HTTP_RESPONSE_414:            return "http: response: 414";
    case TB_STATE_HTTP_RESPONSE_415:            return "http: response: 415";
    case TB_STATE_HTTP_RESPONSE_416:            return "http: response: 416";
    case TB_STATE_HTTP_RESPONSE_500:            return "http: response: 500";
    case TB_STATE_HTTP_RESPONSE_501:            return "http: response: 501";
    case TB_STATE_HTTP_RESPONSE_502:            return "http: response: 502";
    case TB_STATE_HTTP_RESPONSE_503:            return "http: response: 503";
    case TB_STATE_HTTP_RESPONSE_504:            return "http: response: 504";
    case TB_STATE_HTTP_RESPONSE_505:            return "http: response: 505";
    case TB_STATE_HTTP_RESPONSE_506:            return "http: response: 506";
    case TB_STATE_HTTP_RESPONSE_507:            return "http: response: 507";
    case TB_STATE_HTTP_RESPONSE_UNK:            return "http: response: unknown code";
    case TB_STATE_HTTP_RESPONSE_NUL:            return "http: response: no";
    case TB_STATE_HTTP_REQUEST_FAILED:          return "http: request: failed";
    case TB_STATE_HTTP_REDIRECT_FAILED:         return "http: redirect: failed";
    case TB_STATE_HTTP_POST_FAILED:             return "http: post: failed";
    case TB_STATE_HTTP_RANGE_INVALID:           return "http: range: invalid";
    case TB_STATE_HTTP_GZIP_NOT_SUPPORTED:      return "http: gzip: not supported";
    case TB_STATE_HTTP_UNKNOWN_ERROR:           return "http: unknown error";

    case TB_STATE_DATABASE_NO_SUCH_TABLE:       return "database: no such table";
    case TB_STATE_DATABASE_NO_SUCH_FIELD:       return "database: no such field";
    case TB_STATE_DATABASE_NO_SUCH_DATABASE:    return "database: no such database";
    case TB_STATE_DATABASE_ACCESS_DENIED:       return "database: access denied";
    case TB_STATE_DATABASE_PARSE_ERROR:         return "database: parse error";
    case TB_STATE_DATABASE_VALUE_COUNT_ERROR:   return "database: value count error";
    case TB_STATE_DATABASE_UNKNOWN_HOST:        return "database: unknown host";
    case TB_STATE_DATABASE_UNKNOWN_ERROR:       return "database: unknown error";
    default:                                    return "unknown";
    }

    return tb_null;
}
