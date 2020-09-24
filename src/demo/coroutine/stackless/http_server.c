/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the port
#define TB_DEMO_PORT        (8080)

// the timeout
#define TB_DEMO_TIMEOUT     (-1)

// the stack size
#define TB_DEMO_STACKSIZE   (8192 << 2)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the http session type
typedef struct __tb_demo_http_session_t
{
    // the socket
    tb_socket_ref_t     sock;

    // the http code
    tb_size_t           code;

    // the http method
    tb_size_t           method;

    // the content size
    tb_hize_t           content_size;

    // is keep-alive?
    tb_bool_t           keep_alive;

    // the file
    tb_file_ref_t       file;

    // the data buffer
    tb_byte_t           data[8192];

    // the resource path
    tb_char_t           path[1024];

    // the line buffer
    tb_char_t           line[1024];

    // the line size
    tb_size_t           line_size;

    // the line index
    tb_size_t           line_index;

    // the locals
    struct
    {
        // ok?
        tb_long_t       ok;

        // the wait state
        tb_long_t       wait;

        // the real size
        tb_long_t       real;

        // the send c-string
        tb_char_t*      cstr;

        // the send size
        tb_hize_t       send;

        // the data size
        tb_hize_t       size;

    }                   locals;

}tb_demo_http_session_t, *tb_demo_http_session_ref_t;

// the http listen type
typedef struct __tb_demo_http_listen_t
{
    // the listen socket
    tb_socket_ref_t     sock;

    // the address
    tb_ipaddr_t         addr;

    // the client socket
    tb_socket_ref_t     client;

}tb_demo_http_listen_t, *tb_demo_http_listen_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// the root directory
static tb_char_t        g_rootdir[TB_PATH_MAXN];

// only send data for testing?
static tb_bool_t        g_onlydata = tb_false;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_bool_t tb_demo_http_session_init(tb_demo_http_session_ref_t session)
{
    // init
    session->file           = tb_null;
    session->line_size      = 0;
    session->line_index     = 0;
    session->keep_alive     = tb_false;
    session->code           = TB_HTTP_CODE_OK;
    session->method         = TB_HTTP_METHOD_GET;
    session->content_size   = 0;

    // ok
    return tb_true;
}
static tb_void_t tb_demo_http_session_exit(tb_demo_http_session_ref_t session)
{
    // check
    tb_assert(session);

    // exit socket
    if (session->sock) tb_socket_exit(session->sock);
    session->sock = tb_null;

    // exit file
    if (session->file) tb_file_exit(session->file);
    session->file = tb_null;
}
static tb_char_t const* tb_demo_http_session_code_cstr(tb_size_t code)
{
    // done
    tb_char_t const* cstr = tb_null;
    switch (code)
    {
    case TB_HTTP_CODE_OK:                       cstr = "OK"; break;
    case TB_HTTP_CODE_BAD_REQUEST:              cstr = "Bad Request"; break;
    case TB_HTTP_CODE_NOT_FOUND:                cstr = "Not Found"; break;
    case TB_HTTP_CODE_NOT_IMPLEMENTED:          cstr = "Not Implemented"; break;
    case TB_HTTP_CODE_NOT_MODIFIED:             cstr = "Not Modified"; break;
    case TB_HTTP_CODE_INTERNAL_SERVER_ERROR:    cstr = "Internal Server Error"; break;
    default: break;
    }

    // check
    tb_assert(cstr);

    // ok?
    return cstr;
}
static tb_long_t tb_demo_http_session_data_send(tb_demo_http_session_ref_t session)
{
    // done
    while (session->locals.send < session->locals.size)
    {
        // send it
        tb_long_t real = tb_socket_send(session->sock, session->data + session->locals.send, (tb_size_t)(session->locals.size - session->locals.send));

        // has data?
        if (real > 0)
        {
            session->locals.send += real;
            session->locals.wait = 0;
        }
        // no data? wait it
        else if (!real && !session->locals.wait) return 0;
        // failed or end?
        else break;
    }

    // ok?
    return session->locals.send < session->locals.size? -1 : 1;
}
static tb_long_t tb_demo_http_session_file_send(tb_demo_http_session_ref_t session)
{
    // done
    while (session->locals.send < session->locals.size)
    {
        // send it
        tb_hong_t real = tb_socket_sendf(session->sock, session->file, session->locals.send, session->locals.size - session->locals.send);

        // has data?
        if (real > 0)
        {
            session->locals.send += real;
            session->locals.wait = 0;
        }
        // no data? wait it
        else if (!real && !session->locals.wait) return 0;
        // failed or end?
        else break;
    }

    // ok?
    return session->locals.send < session->locals.size? -1 : 1;
}
static tb_void_t tb_demo_http_session_head_parse(tb_demo_http_session_ref_t session)
{
    // check
    tb_assert_and_check_return(session);

    // the first line?
    tb_char_t const* p = session->line;
    if (!session->line_index)
    {
        // parse get
        if (!tb_strnicmp(p, "GET", 3))
        {
            session->code       = TB_HTTP_CODE_OK;
            session->method     = TB_HTTP_METHOD_GET;
            p += 3;
        }
        // parse post
        else if (!tb_strnicmp(p, "POST", 4))
        {
            session->code       = TB_HTTP_CODE_NOT_IMPLEMENTED;
            session->method     = TB_HTTP_METHOD_POST;
            p += 4;
        }
        // other method is not implemented
        else session->code = TB_HTTP_CODE_NOT_IMPLEMENTED;

        // get or post? parse the path
        if (    session->method == TB_HTTP_METHOD_GET
            ||  session->method == TB_HTTP_METHOD_POST)
        {
            // skip space
            while (*p && tb_isspace(*p)) p++;

            // append path
            tb_size_t i = 0;
            while (*p && !tb_isspace(*p) && i < sizeof(session->path) - 1) session->path[i++] = *p++;
            session->path[i] = '\0';
        }
    }
    // key: value?
    else
    {
        // seek to value
        while (*p && *p != ':') p++;
        tb_assert_and_check_return(*p);
        p++; while (*p && tb_isspace(*p)) p++;

        // no value
        tb_check_return(*p);

        // parse content-length
        if (!tb_strnicmp(session->line, "Content-Length", 14))
            session->content_size = tb_stou64(p);
        // parse connection
        else if (!tb_strnicmp(session->line, "Connection", 10))
            session->keep_alive = !tb_stricmp(p, "keep-alive");
        // parse range
        else if (!tb_strnicmp(session->line, "Range", 5))
            session->code = TB_HTTP_CODE_NOT_IMPLEMENTED;
    }
}
static tb_long_t tb_demo_http_session_head_line(tb_demo_http_session_ref_t session, tb_byte_t const* data, tb_size_t size)
{
    // check
    tb_assert_and_check_return_val(session && data && size, -1);

    // done
    tb_long_t ok = 0;
    do
    {
        // done
        tb_char_t           ch = '\0';
        tb_char_t const*    p = (tb_char_t const*)data;
        tb_char_t const*    e = p + size;
        while (p < e)
        {
            // the char
            ch = *p++;

            // error end?
            if (!ch)
            {
                ok = -1;
                break;
            }

            // append char to line
            if (ch != '\n') session->line[session->line_size++] = ch;
            // is line end?
            else
            {
                // line end? strip '\r\n'
                if (session->line_size && session->line[session->line_size - 1] == '\r')
                {
                    session->line_size--;
                    session->line[session->line_size] = '\0';
                }

                // trace
                tb_trace_d("head: %s", session->line);

                // end?
                if (!session->line_size)
                {
                    // ok
                    ok = 1;
                    break;
                }

                // parse the header line
                tb_demo_http_session_head_parse(session);

                // clear line
                session->line_size = 0;

                // update line index
                session->line_index++;
            }
        }

    } while (0);

    // ok?
    return ok;
}
static tb_long_t tb_demo_http_session_head_recv(tb_demo_http_session_ref_t session)
{
    // check
    tb_assert_and_check_return_val(session && session->sock, -1);

    // read data
    tb_long_t ok = 0;
    while (!ok)
    {
        // read it
        tb_long_t real = tb_socket_recv(session->sock, session->data, sizeof(session->data));

        // has data?
        if (real > 0)
        {
            // get the header line
            ok = tb_demo_http_session_head_line(session, session->data, real);

            // clear wait events
            session->locals.wait = 0;
        }
        // no data? wait it
        else if (!real && !session->locals.wait) break;
        // failed or end?
        else
        {
            ok = -1;
            break;
        }
    }

    // ok?
    return ok;
}
static tb_void_t tb_demo_lo_coroutine_client(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_demo_http_session_ref_t session = (tb_demo_http_session_ref_t)priv;
    tb_assert(session);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // done
        do
        {
            // init session
            if (!tb_demo_http_session_init(session)) break;

            // read data
            session->locals.wait = 0;
            while (!(session->locals.ok = tb_demo_http_session_head_recv(session)))
            {
                // wait it
                tb_lo_coroutine_wait_sock(session->sock, TB_SOCKET_EVENT_RECV, TB_DEMO_TIMEOUT);

                // wait ok
                session->locals.wait = tb_lo_coroutine_wait_result();
                tb_assert_and_check_break(session->locals.wait >= 0);
            }
            tb_check_break(session->locals.ok > 0);

            // trace
            tb_trace_d("path: %s", session->path);

            // get file or cstr?
            session->locals.cstr = tb_null;
            if (session->method == TB_HTTP_METHOD_GET)
            {
                // only send data?
                if (g_onlydata) session->locals.cstr = g_rootdir;
                else
                {
                    // make full path
                    session->locals.real = tb_snprintf((tb_char_t*)session->data, sizeof(session->data), "%s%s%s", g_rootdir, session->path[0] != '/'? "/" : "", session->path);
                    if (session->locals.real > 0) session->data[session->locals.real] = 0;

                    // init file
                    session->file = tb_file_init((tb_char_t*)session->data, TB_FILE_MODE_RO);

                    // not found?
                    if (!session->file) session->code = TB_HTTP_CODE_NOT_FOUND;
                }
            }

            // make the response header
            session->locals.size = tb_snprintf(     (tb_char_t*)session->data
                                                ,   sizeof(session->data)
                                                ,   "HTTP/1.1 %lu %s\r\n"
                                                    "Server: %s\r\n"
                                                    "Content-Type: text/html\r\n"
                                                    "Content-Length: %llu\r\n"
                                                    "Connection: %s\r\n"
                                                    "\r\n"
                                                    "%s"
                                                ,   session->code
                                                ,   tb_demo_http_session_code_cstr(session->code)
                                                ,   TB_VERSION_SHORT_STRING
                                                ,   session->file? tb_file_size(session->file) : (session->locals.cstr? tb_strlen(session->locals.cstr) : 0)
                                                ,   session->keep_alive? "keep-alive" : "close"
                                                ,   session->locals.cstr? session->locals.cstr : "");
            // end
            session->data[session->locals.size] = 0;

            // send data
            session->locals.send = 0;
            session->locals.wait = 0;
            while (!(session->locals.ok = tb_demo_http_session_data_send(session)))
            {
                // wait it
                tb_lo_coroutine_wait_sock(session->sock, TB_SOCKET_EVENT_SEND, TB_DEMO_TIMEOUT);

                // wait ok
                session->locals.wait = tb_lo_coroutine_wait_result();
                tb_assert_and_check_break(session->locals.wait >= 0);
            }
            tb_check_break(session->locals.ok > 0);

            // send file
            if (session->file)
            {
                // send data
                session->locals.send = 0;
                session->locals.wait = 0;
                session->locals.size = tb_file_size(session->file);
                while (!(session->locals.ok = tb_demo_http_session_file_send(session)))
                {
                    // wait it
                    tb_lo_coroutine_wait_sock(session->sock, TB_SOCKET_EVENT_SEND, TB_DEMO_TIMEOUT);

                    // wait ok
                    session->locals.wait = tb_lo_coroutine_wait_result();
                    tb_assert_and_check_break(session->locals.wait >= 0);
                }
                tb_check_break(session->locals.ok > 0);

                // exit file
                tb_file_exit(session->file);
                session->file = tb_null;
            }

            // trace
            tb_trace_d("ok!");

        } while (session->keep_alive);

        // exit session
        tb_demo_http_session_exit(session);
    }
}
static tb_void_t tb_demo_lo_coroutine_listen(tb_lo_coroutine_ref_t coroutine, tb_cpointer_t priv)
{
    // check
    tb_demo_http_listen_ref_t listen = (tb_demo_http_listen_ref_t)priv;
    tb_assert(listen);

    // enter coroutine
    tb_lo_coroutine_enter(coroutine)
    {
        // done
        do
        {
            // init socket
            listen->sock = tb_socket_init(TB_SOCKET_TYPE_TCP, TB_IPADDR_FAMILY_IPV4);
            tb_assert_and_check_break(listen->sock);

            // bind socket
            tb_ipaddr_set(&listen->addr, tb_null, TB_DEMO_PORT, TB_IPADDR_FAMILY_IPV4);
            if (!tb_socket_bind(listen->sock, &listen->addr)) break;

            // listen socket
            if (!tb_socket_listen(listen->sock, 1000)) break;

            // trace
            tb_trace_i("listening ..");

            // loop
            while (1)
            {
                // accept client sockets
                if ((listen->client = tb_socket_accept(listen->sock, tb_null)))
                {
                    // start client connection
                    if (!tb_lo_coroutine_start(tb_lo_scheduler_self(), tb_demo_lo_coroutine_client, tb_lo_coroutine_pass1(tb_demo_http_session_t, sock, listen->client))) break;
                }
                else
                {
                    // wait accept events
                    tb_lo_coroutine_wait_sock(listen->sock, TB_SOCKET_EVENT_ACPT, -1);
                    if (tb_lo_coroutine_wait_result() <= 0) break;
                }
            }

        } while (0);

        // exit socket
        if (listen->sock) tb_socket_exit(listen->sock);
        listen->sock = tb_null;
    }
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_lo_coroutine_http_server_main(tb_int_t argc, tb_char_t** argv)
{
    // init the root directory
    if (argv[1]) tb_strlcpy(g_rootdir, argv[1], sizeof(g_rootdir));
#ifndef TB_CONFIG_MICRO_ENABLE
    else tb_directory_current(g_rootdir, sizeof(g_rootdir));
#endif

    // only data?
    if (!tb_file_info(g_rootdir, tb_null)) g_onlydata = tb_true;

    // trace
    tb_trace_i("%s: %s", g_onlydata? "data" : "rootdir", g_rootdir);

    // init scheduler
    tb_lo_scheduler_ref_t scheduler = tb_lo_scheduler_init();
    if (scheduler)
    {
        // start coroutines
        tb_lo_coroutine_start(scheduler, tb_demo_lo_coroutine_listen, tb_lo_coroutine_pass(tb_demo_http_listen_t));

        // run scheduler, enable exclusive mode if be only one cpu
        tb_lo_scheduler_loop(scheduler, tb_true);

        // exit scheduler
        tb_lo_scheduler_exit(scheduler);
    }

    // ok
    return 0;
}
