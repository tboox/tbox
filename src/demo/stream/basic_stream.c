/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */
typedef struct __tb_demo_context_t
{
    // verbose 
    tb_bool_t           verbose;

}tb_demo_context_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * func
 */
static tb_bool_t tb_demo_http_post_func(tb_handle_t http, tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // percent
    tb_size_t percent = 0;
    if (size > 0) percent = (tb_size_t)((offset * 100) / size);
    else if (state == TB_STATE_CLOSED) percent = 100;

    // trace
    tb_trace_i("post: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", save, rate, percent, tb_state_cstr(state));

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_basic_stream_head_func(tb_handle_t http, tb_char_t const* line, tb_cpointer_t priv)
{
    tb_printf("response: %s\n", line);
    return tb_true;
}
static tb_bool_t tb_demo_basic_stream_save_func(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // check
    tb_demo_context_t* context = (tb_demo_context_t*)priv;
    tb_assert_and_check_return_val(context, tb_false);

    // print verbose info
    if (context->verbose) 
    {
        // percent
        tb_size_t percent = 0;
        if (size > 0) percent = (tb_size_t)((offset * 100) / size);
        else if (state == TB_STATE_CLOSED) percent = 100;

        // trace
        tb_printf("save: %llu bytes, rate: %lu bytes/s, percent: %lu%%, state: %s\n", save, rate, percent, tb_state_cstr(state));
    }

    // ok
    return tb_true;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */ 
static tb_option_item_t g_options[] = 
{
    {'-',   "gzip",         TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "enable gzip"               }
,   {'-',   "no-verbose",   TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "disable verbose info"      }
,   {'d',   "debug",        TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "enable debug info"         }
,   {'k',   "keep-alive",   TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "keep alive"                }
,   {'h',   "header",       TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_CSTR,        "the custem http header"    }
,   {'-',   "post-data",    TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_CSTR,        "set the post data"         }
,   {'-',   "post-file",    TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_CSTR,        "set the post file"         }
,   {'-',   "range",        TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_CSTR,        "set the range"             }
,   {'-',   "timeout",      TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_INTEGER,     "set the timeout"           }
,   {'-',   "limitrate",    TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_INTEGER,     "set the limitrate"         }
,   {'h',   "help",         TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "display this help and exit"}
,   {'-',   "url",          TB_OPTION_MODE_VAL,         TB_OPTION_TYPE_CSTR,        "the url"                   }
,   {'-',   tb_null,        TB_OPTION_MODE_MORE,        TB_OPTION_TYPE_NONE,        tb_null                     }

};

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_stream_basic_stream_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_handle_t     option = tb_null;
    tb_basic_stream_t*  istream = tb_null;
    tb_basic_stream_t*  ostream = tb_null;
    tb_basic_stream_t*  pst = tb_null;
    do
    {
        // init option
        option = tb_option_init("bstream", "the bstream demo", g_options);
        tb_assert_and_check_break(option);
    
        // done option
        if (tb_option_done(option, argc - 1, &argv[1]))
        {
            // debug & verbose
            tb_bool_t debug = tb_option_find(option, "debug");
            tb_bool_t verbose = tb_option_find(option, "no-verbose")? tb_false : tb_true;
        
            // done url
            if (tb_option_find(option, "url")) 
            {
                // init istream
                istream = tb_basic_stream_init_from_url(tb_option_item_cstr(option, "url"));
                tb_assert_and_check_break(istream);
    
                // ctrl http
                if (tb_stream_type(istream) == TB_STREAM_TYPE_HTTP) 
                {
                    // enable gzip?
                    if (tb_option_find(option, "gzip"))
                    {
                        // auto unzip
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_AUTO_UNZIP, 1)) break;

                        // need gzip
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_HEAD, "Accept-Encoding", "gzip,deflate")) break;
                    }

                    // enable debug?
                    if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_HEAD_FUNC, debug? tb_demo_basic_stream_head_func : tb_null)) break;

                    // custem header?
                    if (tb_option_find(option, "header"))
                    {
                        // init
                        tb_scoped_string_t key;
                        tb_scoped_string_t val;
                        tb_scoped_string_init(&key);
                        tb_scoped_string_init(&val);

                        // done
                        tb_bool_t           k = tb_true;
                        tb_char_t const*    p = tb_option_item_cstr(option, "header");
                        while (*p)
                        {
                            // is key?
                            if (k)
                            {
                                if (*p != ':' && !tb_isspace(*p)) tb_scoped_string_chrcat(&key, *p++);
                                else if (*p == ':') 
                                {
                                    // skip ':'
                                    p++;

                                    // skip space
                                    while (*p && tb_isspace(*p)) p++;

                                    // is val now
                                    k = tb_false;
                                }
                                else p++;
                            }
                            // is val?
                            else
                            {
                                if (*p != ';') tb_scoped_string_chrcat(&val, *p++);
                                else
                                {
                                    // skip ';'
                                    p++;

                                    // skip space
                                    while (*p && tb_isspace(*p)) p++;

                                    // set header
                                    if (tb_scoped_string_size(&key) && tb_scoped_string_size(&val))
                                    {
                                        if (debug) tb_printf("header: %s: %s\n", tb_scoped_string_cstr(&key), tb_scoped_string_cstr(&val));
                                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_HEAD, tb_scoped_string_cstr(&key), tb_scoped_string_cstr(&val))) break;
                                    }

                                    // is key now
                                    k = tb_true;

                                    // clear key & val
                                    tb_scoped_string_clear(&key);
                                    tb_scoped_string_clear(&val);
                                }
                            }
                        }

                        // set header
                        if (tb_scoped_string_size(&key) && tb_scoped_string_size(&val))
                        {
                            if (debug) tb_printf("header: %s: %s\n", tb_scoped_string_cstr(&key), tb_scoped_string_cstr(&val));
                            if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_HEAD, tb_scoped_string_cstr(&key), tb_scoped_string_cstr(&val))) break;
                        }

                        // exit 
                        tb_scoped_string_exit(&key);
                        tb_scoped_string_exit(&val);
                    }

                    // keep alive?
                    if (tb_option_find(option, "keep-alive"))
                    {
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_HEAD, "Connection", "keep-alive")) break;
                    }

                    // post-data?
                    if (tb_option_find(option, "post-data"))
                    {
                        tb_char_t const*    post_data = tb_option_item_cstr(option, "post-data");
                        tb_hize_t           post_size = tb_strlen(post_data);
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_METHOD, TB_HTTP_METHOD_POST)) break;
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_POST_DATA, post_data, post_size)) break;
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_POST_FUNC, tb_demo_http_post_func)) break;
                        if (debug) tb_printf("post: %llu\n", post_size);
                    }
                    // post-file?
                    else if (tb_option_find(option, "post-file"))
                    {
                        tb_char_t const* url = tb_option_item_cstr(option, "post-file");
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_METHOD, TB_HTTP_METHOD_POST)) break;
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_POST_URL, url)) break;
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_POST_FUNC, tb_demo_http_post_func)) break;
                        if (debug) tb_printf("post: %s\n", url);
                    }
                }

                // set range
                if (tb_option_find(option, "range"))
                {
                    tb_char_t const* p = tb_option_item_cstr(option, "range");
                    if (p)
                    {
                        // the bof
                        tb_hize_t eof = 0;
                        tb_hize_t bof = tb_atoll(p);
                        while (*p && tb_isdigit(*p)) p++;
                        if (*p == '-')
                        {
                            p++;
                            eof = tb_atoll(p);
                        }
                        if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_RANGE, bof, eof)) break;
                    }
                }

                // set timeout
                if (tb_option_find(option, "timeout"))
                {
                    tb_size_t timeout = tb_option_item_uint32(option, "timeout");
                    if (!tb_stream_ctrl(istream, TB_STREAM_CTRL_SET_TIMEOUT, timeout)) break;
                }

                // print verbose info
                if (verbose) tb_printf("open: %s: ..\n", tb_option_item_cstr(option, "url"));

                // open istream
                if (!tb_basic_stream_open(istream)) 
                {
                    // print verbose info
                    if (verbose) tb_printf("open: %s\n", tb_state_cstr(tb_basic_stream_state(istream)));
                    break;
                }

                // print verbose info
                if (verbose) tb_printf("open: ok\n");

                // init ostream
                if (tb_option_find(option, "more0"))
                {
                    // the path
                    tb_char_t const* path = tb_option_item_cstr(option, "more0");

                    // init
                    ostream = tb_basic_stream_init_from_file(path, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);

                    // print verbose info
                    if (verbose) tb_printf("save: %s\n", path);
                }
                else 
                {
                    // the name
                    tb_char_t const* name = tb_strrchr(tb_option_item_cstr(option, "url"), '/');
                    if (!name) name = tb_strrchr(tb_option_item_cstr(option, "url"), '\\');
                    if (!name) name = "/basic_stream.file";

                    // the path
                    tb_char_t path[TB_PATH_MAXN] = {0};
                    if (tb_directory_curt(path, TB_PATH_MAXN))
                        tb_strcat(path, name);
                    else break;

                    // init file
                    ostream = tb_basic_stream_init_from_file(path, TB_FILE_MODE_RW | TB_FILE_MODE_CREAT | TB_FILE_MODE_BINARY | TB_FILE_MODE_TRUNC);

                    // print verbose info
                    if (verbose) tb_printf("save: %s\n", path);
                }
                tb_assert_and_check_break(ostream);

                // the limit rate
                tb_size_t limitrate = 0;
                if (tb_option_find(option, "limitrate"))
                    limitrate = tb_option_item_uint32(option, "limitrate");

                // save it
                tb_hong_t           save = 0;
                tb_demo_context_t   context = {0}; 
                context.verbose     = verbose;
                if ((save = tb_basic_transfer_done(istream, ostream, limitrate, tb_demo_basic_stream_save_func, &context)) < 0) break;
            }
            else tb_option_help(option);
        }
        else tb_option_help(option);

    } while (0);

    // exit pst
    if (pst) tb_basic_stream_exit(pst);
    pst = tb_null;

    // exit istream
    if (istream) tb_basic_stream_exit(istream);
    istream = tb_null;

    // exit ostream
    if (ostream) tb_basic_stream_exit(ostream);
    ostream = tb_null;

    // exit option
    if (option) tb_option_exit(option);
    option = tb_null;

    return 0;
}
