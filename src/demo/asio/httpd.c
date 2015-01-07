/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the httpd port
#define TB_DEMO_HTTPD_PORT                              (8080)

// the httpd session maximum count
#define TB_DEMO_HTTPD_SESSION_MAXN                      (100000)

// the httpd session timeout: 15s
#define TB_DEMO_HTTPD_SESSION_TIMEOUT                   (15000)

// the httpd session buffer maximum size
#define TB_DEMO_HTTPD_SESSION_BUFF_MAXN                 (8192)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the httpd type
typedef struct __tb_demo_httpd_t
{
    // the root directory
    tb_char_t           root[TB_PATH_MAXN];

    // the listen port
    tb_uint16_t         port;

    // the listen aico
    tb_aico_ref_t       aico;

    // the aicp
    tb_aicp_ref_t       aicp;

#ifdef TB_DEMO_HTTPD_SESSION_MAXN
    // the loop
    tb_thread_ref_t     loop[16];
#endif

}tb_demo_httpd_t;

// the httpd session type
typedef struct __tb_demo_httpd_session_t
{
    // the aico
    tb_aico_ref_t       aico;

    // the code
    tb_size_t           code;

    // the file
    tb_file_ref_t       file;

    // the path
    tb_string_t         path;

    // the line
    tb_string_t         line;

    // the index
    tb_size_t           index;

    // the cache
    tb_buffer_t         cache;

    // the httpd
    tb_demo_httpd_t*    httpd;

    // the method
    tb_size_t           method;

    // the file offset
    tb_hize_t           file_offset;

    // the content size
    tb_hize_t           content_size;

    // the version
    tb_uint32_t         version : 1;

    // keep-alive?
    tb_uint32_t         balived : 1;

    // the recv and send buffer
    tb_byte_t           buffer[TB_DEMO_HTTPD_SESSION_BUFF_MAXN];

}tb_demo_httpd_session_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_char_t const* tb_demo_httpd_code_cstr(tb_size_t code)
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
    tb_assert_abort(cstr);

    // ok?
    return cstr;
}
static tb_bool_t tb_demo_httpd_aico_clos(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_CLOS, tb_false);

    // trace
    tb_trace_d("aico[%p]: clos: %s", aice->aico, tb_state_cstr(aice->state));

    // exit aico
    tb_aico_exit(aice->aico);

    // ok
    return tb_true;
}
static tb_void_t tb_demo_httpd_session_exit(tb_demo_httpd_session_t* session)
{
    // check
    tb_assert_and_check_return(session);

    // clos aico
    if (session->aico) tb_aico_clos(session->aico, tb_demo_httpd_aico_clos, tb_null);
    session->aico = tb_null;

    // exit file
    if (session->file) tb_file_exit(session->file);
    session->file = tb_null;

    // exit cache
    tb_buffer_exit(&session->cache);
    
    // exit line
    tb_string_exit(&session->line);
 
    // exit path
    tb_string_exit(&session->path);

    // clear status
    session->code           = TB_HTTP_CODE_OK;
    session->index          = 0;
    session->httpd          = tb_null;
    session->method         = TB_HTTP_METHOD_GET;
    session->version        = 1;
    session->balived        = 0;
    session->file_offset    = 0;
    session->content_size   = 0;
    
    // exit it
    tb_free(session);
}
static tb_void_t tb_demo_httpd_session_keep(tb_demo_httpd_session_t* session)
{
    // check
    tb_assert_and_check_return(session);

    // exit file
    if (session->file) tb_file_exit(session->file);
    session->file = tb_null;

    // clear cache
    tb_buffer_clear(&session->cache);
    
    // clear line
    tb_string_clear(&session->line);
 
    // clear path
    tb_string_clear(&session->path);

    // clear some status
    session->code           = TB_HTTP_CODE_OK;
    session->index          = 0;
    session->method         = TB_HTTP_METHOD_GET;
    session->file_offset    = 0;
    session->content_size   = 0;
}
static tb_demo_httpd_session_t* tb_demo_httpd_session_init(tb_demo_httpd_t* httpd, tb_aico_ref_t aico)
{
    // check
    tb_assert_and_check_return_val(httpd && httpd->aicp && aico, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_demo_httpd_session_t*    session = tb_null;
    do
    {
        // make session
        session = tb_malloc0_type(tb_demo_httpd_session_t);
        tb_assert_and_check_break(session);

        // init session
        session->httpd          = httpd;
        session->aico           = aico;
        session->code           = TB_HTTP_CODE_OK;
        session->version        = 1;
        session->balived        = 0;
        session->method         = TB_HTTP_METHOD_GET;
        session->file_offset    = 0;
        session->content_size   = 0;

        // init path
        if (!tb_string_init(&session->path)) break;

        // init line
        if (!tb_string_init(&session->line)) break;

        // init cache
        if (!tb_buffer_init(&session->cache)) break;

        // init timeout
        tb_aico_timeout_set(session->aico, TB_AICO_TIMEOUT_SEND, TB_DEMO_HTTPD_SESSION_TIMEOUT);
        tb_aico_timeout_set(session->aico, TB_AICO_TIMEOUT_RECV, TB_DEMO_HTTPD_SESSION_TIMEOUT);

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit session
        if (session) tb_demo_httpd_session_exit(session);
        session = tb_null;
    }

    // ok?
    return session;
}
static tb_bool_t tb_demo_httpd_session_head_recv(tb_aice_ref_t aice);
static tb_void_t tb_demo_httpd_session_resp_exit(tb_demo_httpd_session_t* session)
{
    // keep-alived?
    tb_bool_t ok = tb_false;
    if (session->balived && session->aico)
    {
        // keep session
        tb_demo_httpd_session_keep(session);

        // recv the header
        ok = tb_aico_recv(session->aico, session->buffer, sizeof(session->buffer), tb_demo_httpd_session_head_recv, session);
    }

    // exit session
    if (!ok) tb_demo_httpd_session_exit(session);
}
static tb_bool_t tb_demo_httpd_session_resp_send_file(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->code == TB_AICE_CODE_SENDF, tb_false);

    // the session
    tb_demo_httpd_session_t* session = (tb_demo_httpd_session_t*)aice->priv;
    tb_assert_and_check_return_val(session, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // ok?
        tb_check_break(aice->state == TB_STATE_OK);

        // trace
        tb_trace_d("resp_send_file[%p]: real: %lu, size: %llu", aice->aico, aice->u.sendf.real, aice->u.sendf.size);

        // save offset
        session->file_offset += aice->u.sendf.real;

        // continue to send it?
        if (aice->u.sendf.real < aice->u.sendf.size)
        {
            // send file
            ok = tb_aico_sendf(aice->aico, session->file, session->file_offset, aice->u.sendf.size - aice->u.sendf.real, tb_demo_httpd_session_resp_send_file, session);
        }

    } while (0);

    // finished or closed or failed?
    if (!ok)
    {
        // trace
        tb_trace_d("resp_send_file[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // exit response
        tb_demo_httpd_session_resp_exit(session);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_httpd_session_resp_send_head(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_SEND, tb_false);

    // the session
    tb_demo_httpd_session_t* session = (tb_demo_httpd_session_t*)aice->priv;
    tb_assert_and_check_return_val(session, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // ok?
        tb_check_break(aice->state == TB_STATE_OK);

        // trace
        tb_trace_d("resp_send_head[%p]: real: %lu, size: %lu", aice->aico, aice->u.send.real, aice->u.send.size);

        // check data
        tb_assert_and_check_break(aice->u.send.data && aice->u.send.size);

        // not finished? 
        if (aice->u.send.real < aice->u.send.size)
        {
            // continue to send it
            ok = tb_aico_send(aice->aico, aice->u.send.data, aice->u.send.size - aice->u.send.real, tb_demo_httpd_session_resp_send_head, session);
        }
        // send file if exists
        else if (session->file)
        {
            // send file
            ok = tb_aico_sendf(aice->aico, session->file, session->file_offset, tb_file_size(session->file), tb_demo_httpd_session_resp_send_file, session);
        }

    } while (0);

    // not continue?
    if (!ok)
    {
        // trace
        tb_trace_d("resp_send_head[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // exit response
        tb_demo_httpd_session_resp_exit(session);
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_httpd_session_resp_send_done(tb_demo_httpd_session_t* session)
{
    // check
    tb_assert_and_check_return_val(session, tb_false);
 
    // format the error info
    tb_long_t size = tb_snprintf(   (tb_char_t*)session->buffer
                                ,   sizeof(session->buffer) - 1
                                ,   "HTTP/1.%u %lu %s\r\n"
                                    "Server: %s\r\n"
                                    "Content-Type: text/html\r\n"
                                    "Content-Length: %llu\r\n"
                                    "Connection: %s\r\n"
                                    "\r\n"
                                ,   session->version
                                ,   session->code
                                ,   tb_demo_httpd_code_cstr(session->code)
                                ,   TB_VERSION_SHORT_STRING
                                ,   session->file? tb_file_size(session->file) : 0
                                ,   session->balived? "keep-alive" : "close");

    tb_assert_and_check_return_val(size > 0, tb_false);

    // end
    session->buffer[size] = '\0';

    // trace
    tb_trace_d("response[%p]: %s", session->aico, session->buffer);

    // send the error info
    return tb_aico_send(session->aico, session->buffer, size, tb_demo_httpd_session_resp_send_head, session);
}
static tb_bool_t tb_demo_httpd_session_reqt_done(tb_demo_httpd_session_t* session)
{
    // check
    tb_assert_and_check_return_val(session && session->aico, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // check 
        tb_check_break(session->code == TB_HTTP_CODE_OK);
        tb_assert_and_check_break(session->httpd);

        // get?
        if (session->method == TB_HTTP_METHOD_GET)
        {
            // check path
            tb_check_break_state(tb_string_size(&session->path), session->code, TB_HTTP_CODE_BAD_REQUEST);

            // the path
            tb_char_t const* path = tb_string_cstr(&session->path);
            tb_check_break_state(!session->file && path, session->code, TB_HTTP_CODE_INTERNAL_SERVER_ERROR);

            // the full path
            tb_char_t full[TB_PATH_MAXN] = {0};
            tb_long_t size = tb_snprintf(full, sizeof(full) - 1, "%s%s%s", session->httpd->root, path[0] != '/'? "/" : "", path);
            tb_assert_abort(size > 0);

            // end
            full[size] = '\0';

            // trace
            tb_trace_d("reqt_done[%p]: full path: %s", session->aico, full);
            
            // init file
            session->file = tb_file_init(full, TB_FILE_MODE_RO | TB_FILE_MODE_BINARY | TB_FILE_MODE_ASIO);
            tb_check_break_state(session->file, session->code, TB_HTTP_CODE_NOT_FOUND);

            // send the file
            if (!tb_demo_httpd_session_resp_send_done(session)) return tb_false;

            // ok
            ok = tb_true;
        }
        else
        {
            // not implemented
            session->code = TB_HTTP_CODE_NOT_IMPLEMENTED;
            break;
        }

    } while (0);

    // error?
    if (!ok)
    {
        // save code
        if (session->code == TB_HTTP_CODE_OK) session->code = TB_HTTP_CODE_INTERNAL_SERVER_ERROR;

        // send the error info
        if (!tb_demo_httpd_session_resp_send_done(session)) return tb_false;
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_httpd_session_head_done(tb_demo_httpd_session_t* session)
{
    // check
    tb_assert_and_check_return_val(session, tb_false);

    // the line and size
    tb_char_t const*    line = tb_string_cstr(&session->line);
    tb_size_t           size = tb_string_size(&session->line);
    tb_assert_and_check_return_val(line && size, tb_false);

    // the first line? 
    tb_char_t const* p = line;
    if (!session->index)
    {
        // check protocol
        if (tb_stristr(line, "HTTP/1.1")) session->version = 1;
        else if (tb_stristr(line, "HTTP/1.0")) session->version = 0;
        // bad request?
        else
        {
            // save code
            session->code = TB_HTTP_CODE_BAD_REQUEST;
            return tb_false;
        }

        // parse get
        if (!tb_strnicmp(line, "GET", 3))
        {
            // save the method
            session->method = TB_HTTP_METHOD_GET;

            // skip the method
            p += 3;
        }
        // parse post
        else if (!tb_strnicmp(line, "POST", 4))
        {
            // save the method
            session->method = TB_HTTP_METHOD_POST;

            // save code
            session->code = TB_HTTP_CODE_NOT_IMPLEMENTED;

            // skip the method
            p += 4;
        }
        else
        {
            // trace
            tb_trace_e("the method: %s is not supported now!", line);

            // save code
            session->code = TB_HTTP_CODE_NOT_IMPLEMENTED;
        }

        // get or post? parse the path
        if (    session->method == TB_HTTP_METHOD_GET
            ||  session->method == TB_HTTP_METHOD_POST)
        {
            // skip space
            while (*p && tb_isspace(*p)) p++;

            // append path
            while (*p && !tb_isspace(*p)) tb_string_chrcat(&session->path, *p++);
        }
    }
    // key: value?
    else
    {
        // seek to value
        while (*p && *p != ':') p++;
        tb_assert_and_check_return_val(*p, tb_false);
        p++; while (*p && tb_isspace(*p)) p++;

        // no value
        tb_check_return_val(*p, tb_true);

        // parse content-length
        if (!tb_strnicmp(line, "Content-Length", 14))
        {
            // the content size
            session->content_size = tb_stou64(p);
        }
        // parse connection
        else if (!tb_strnicmp(line, "Connection", 10))
        {
            // keep-alive?
            session->balived = !tb_stricmp(p, "keep-alive")? 1 : 0;
        }
        // parse accept-encoding
        else if (!tb_strnicmp(line, "Accept-Encoding", 15))
        {
        }
        // parse accept
        else if (!tb_strnicmp(line, "Accept", 6))
        {
        }
        // parse cookie
        else if (!tb_strnicmp(line, "Cookie", 6))
        {
        }
        // parse range
        else if (!tb_strnicmp(line, "Range", 5))
        {
            // save code
            session->code = TB_HTTP_CODE_NOT_IMPLEMENTED;
        }
        // parse host
        else if (!tb_strnicmp(line, "Host", 4))
        {
        }
    }

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_httpd_session_head_recv(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_RECV, tb_false);

    // the session
    tb_demo_httpd_session_t* session = (tb_demo_httpd_session_t*)aice->priv;
    tb_assert_and_check_return_val(session, tb_false);

    // done
    tb_bool_t state = tb_false;
    do
    {
        // ok?
        tb_check_break(aice->state == TB_STATE_OK);

        // trace
        tb_trace_d("head_recv[%p]: real: %lu, size: %lu", aice->aico, aice->u.recv.real, aice->u.recv.size);

        // check
        tb_assert_and_check_break(aice->u.recv.data);
            
        // done
        tb_long_t           ok = 0;
        tb_char_t           ch = '\0';
        tb_char_t const*    p = (tb_char_t const*)aice->u.recv.data;
        tb_char_t const*    e = p + aice->u.recv.real;
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
            if (ch != '\n') tb_string_chrcat(&session->line, ch);
            // is line end?
            else
            {
                // strip '\r' if exists
                tb_char_t const*    pb = tb_string_cstr(&session->line);
                tb_size_t           pn = tb_string_size(&session->line);
                if (!pb || !pn)
                {
                    ok = -1;
                    tb_assert(0);
                    break;
                }

                // line end? strip '\r\n'
                if (pb[pn - 1] == '\r') tb_string_strip(&session->line, pn - 1);

                // trace
                tb_trace_d("head_recv[%p]: %s", aice->aico, pb);
    
                // end?
                if (!tb_string_size(&session->line)) 
                {
                    // ok
                    ok = 1;
                    break;
                }

                // done the head request
                if (!tb_demo_httpd_session_head_done(session)) 
                {   
                    // error
                    ok = -1;
                    break;
                }

                // clear line 
                tb_string_clear(&session->line);

                // update index
                session->index++;
            }
        }

        // continue?
        if (!ok) 
        {
            // recv the header
            if (!tb_aico_recv(aice->aico, session->buffer, sizeof(session->buffer), tb_demo_httpd_session_head_recv, session)) break;
        }
        // end?
        else if (ok > 0) 
        {
            // trace
            tb_trace_d("head_recv[%p]: end, left: %lu", aice->aico, e - p);

            // save the left data to the cache
            tb_buffer_memncpy(&session->cache, (tb_byte_t const*)p, e - p);
 
            // trace
            tb_trace_d("head_recv[%p]: ok", aice->aico);
 
            // done request
            if (!tb_demo_httpd_session_reqt_done(session)) break;
        }
        // failed?
        else 
        {
            // trace
            tb_trace_e("head_recv[%p]: failed", aice->aico);
            break;
        }

        // ok
        state = tb_true;

    } while (0);

    // closed or failed?
    if (!state)
    {
        // trace
        tb_trace_d("head_recv[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // exit session
        tb_demo_httpd_session_exit(session);
    }

    // ok
    return tb_true;
}
static tb_void_t tb_demo_httpd_exit(tb_demo_httpd_t* httpd)
{
    // check
    tb_assert_and_check_return(httpd);

    // trace
    tb_trace_d("exit");

#ifdef TB_DEMO_HTTPD_SESSION_MAXN
    // kill it
    if (httpd->aicp)
    {
        // kill all
        tb_aicp_kill_all(httpd->aicp);
   
        // wait all
        tb_aicp_wait_all(httpd->aicp, -1);

        // kill aicp
        tb_aicp_kill(httpd->aicp);
    }

    // exit loop
    tb_thread_ref_t* loop = httpd->loop;
    for (; *loop; loop++)
    {
        tb_thread_wait(*loop, -1);
        tb_thread_exit(*loop);
    }

    // exit aicp
    if (httpd->aicp) tb_aicp_exit(httpd->aicp);
    httpd->aicp = tb_null;
#endif

    // clear aico
    httpd->aico = tb_null;

    // exit it
    tb_free(httpd);
}
#ifdef TB_DEMO_HTTPD_SESSION_MAXN
static tb_pointer_t tb_demo_httpd_loop(tb_cpointer_t priv)
{
    // aicp
    tb_aicp_ref_t   aicp = (tb_aicp_ref_t)priv;

    // trace
    tb_trace_d("[loop: %u]: init", (tb_uint16_t)tb_thread_self());

    // loop aicp
    if (aicp) tb_aicp_loop(aicp);
    
    // trace
    tb_trace_d("[loop: %u]: exit", (tb_uint16_t)tb_thread_self());

    // exit
    tb_thread_return(tb_null);
    return tb_null;
}
#endif
static tb_demo_httpd_t* tb_demo_httpd_init(tb_char_t const* root)
{
    // done
    tb_bool_t           ok = tb_false;
    tb_demo_httpd_t*    httpd = tb_null;
    do
    {
        // make httpd
        httpd = tb_malloc0_type(tb_demo_httpd_t);
        tb_assert_and_check_break(httpd);

        // init root
        if (root) tb_strlcpy(httpd->root, root, sizeof(httpd->root));
        else tb_directory_curt(httpd->root, sizeof(httpd->root));
        httpd->root[sizeof(httpd->root) - 1] = '\0';
        tb_assert_and_check_break(tb_file_info(httpd->root, tb_null));

        // init port
        httpd->port = TB_DEMO_HTTPD_PORT;

        // trace
        tb_trace_d("init: %s: %u", httpd->root, httpd->port);

#ifdef TB_DEMO_HTTPD_SESSION_MAXN
        // init aicp
        httpd->aicp = tb_aicp_init(TB_DEMO_HTTPD_SESSION_MAXN);
        tb_assert_and_check_break(httpd->aicp);
#else
        // init aicp
        httpd->aicp = tb_aicp();
        tb_assert_and_check_break(httpd->aicp);
#endif

        // init aico
        httpd->aico = tb_aico_init(httpd->aicp);
        tb_assert_and_check_break(httpd->aico);

        // open aico
        if (!tb_aico_open_sock_from_type(httpd->aico, TB_SOCKET_TYPE_TCP)) break;

        // bind port
        if (!tb_socket_bind(tb_aico_sock(httpd->aico), tb_null, httpd->port)) break;

        // listen sock
        if (!tb_socket_listen(tb_aico_sock(httpd->aico), TB_DEMO_HTTPD_SESSION_MAXN >> 2)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit httpd
        if (httpd) tb_demo_httpd_exit(httpd);
        httpd = tb_null;
    }

    // ok?
    return httpd;
}
static tb_bool_t tb_demo_httpd_acpt(tb_aice_ref_t aice)
{
    // check
    tb_assert_and_check_return_val(aice && aice->aico && aice->code == TB_AICE_CODE_ACPT, tb_false);

    // the httpd
    tb_demo_httpd_t* httpd = (tb_demo_httpd_t*)aice->priv;
    tb_assert_and_check_return_val(httpd && httpd->aicp, tb_false);

    // done
    tb_bool_t                   ok = tb_false;
    tb_demo_httpd_session_t*    session = tb_null;
    do
    {
        // ok?
        tb_check_break(aice->state == TB_STATE_OK);
    
        // check
        tb_assert_and_check_break(aice->u.acpt.aico);

        // trace
        tb_trace_d("acpt[%p]: aico: %p, addr: %{addr}", aice->aico, aice->u.acpt.aico, &aice->u.acpt.addr);

#if 1
        // init the session
        session = tb_demo_httpd_session_init(httpd, aice->u.acpt.aico);
        tb_assert_and_check_break(session && session->aico);

        // recv the header
        if (!tb_aico_recv(session->aico, session->buffer, sizeof(session->buffer), tb_demo_httpd_session_head_recv, session)) break;
#else
        tb_used(tb_demo_httpd_session_init);
        tb_used(tb_demo_httpd_session_head_recv);
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // trace
        tb_trace_d("acpt[%p]: state: %s", aice->aico, tb_state_cstr(aice->state));

        // exit session
        if (session) tb_demo_httpd_session_exit(session);
        session = tb_null;

        // clos aico
        if (aice->aico) tb_aico_clos(aice->aico, tb_demo_httpd_aico_clos, tb_null);
    }

    // ok
    return tb_true;
}
static tb_void_t tb_demo_httpd_done(tb_demo_httpd_t* httpd)
{
    // check
    tb_assert_and_check_return(httpd && httpd->aicp && httpd->aico);

#ifdef TB_DEMO_HTTPD_SESSION_MAXN
    // done loop
    httpd->loop[0] = tb_thread_init(tb_null, tb_demo_httpd_loop, httpd->aicp, 0);
    httpd->loop[1] = tb_thread_init(tb_null, tb_demo_httpd_loop, httpd->aicp, 0);
    httpd->loop[2] = tb_thread_init(tb_null, tb_demo_httpd_loop, httpd->aicp, 0);
    httpd->loop[3] = tb_thread_init(tb_null, tb_demo_httpd_loop, httpd->aicp, 0);
#endif

    // done listen
    if (!tb_aico_acpt(httpd->aico, tb_demo_httpd_acpt, httpd)) return ;

    // wait some time
    getchar();
}


/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_asio_httpd_main(tb_int_t argc, tb_char_t** argv)
{
    // init httpd
    tb_demo_httpd_t* httpd = tb_demo_httpd_init(argv[1]);
    if (httpd)
    {
        // done httpd
        tb_demo_httpd_done(httpd);

        // exit httpd
        tb_demo_httpd_exit(httpd);
    }
    return 0;
}
