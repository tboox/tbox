/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */ 

// the spider url maxn
#define TB_DEMO_SPIDER_URL_MAXN             (4096)

// the spider task maxn
#define TB_DEMO_SPIDER_TASK_MAXN            (100)

// the spider task rate, 256KB/s
#define TB_DEMO_SPIDER_TASK_RATE            (256000)

// the spider task timeout, 15s
#define TB_DEMO_SPIDER_TASK_TIMEOUT         (15000)

// the spider filter maxn
#define TB_DEMO_SPIDER_FILTER_MAXN          (100000)

// the spider user agent
#define TB_DEMO_SPIDER_USER_AGENT           "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.137 Safari/537.36"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */ 

// the demo spider type
typedef struct __tb_demo_spider_t
{
    // the pool
    tb_fixed_pool_ref_t             pool;

    // the lock
    tb_spinlock_t                   lock;

    // the filter
    tb_bloom_filter_ref_t           filter;

    // the state
    tb_atomic_t                     state;

    // the option
    tb_option_ref_t                 option;

    // the home
    tb_url_t                        home;

    // only home website?
    tb_bool_t                       home_only;

    // the home domain
    tb_char_t                       home_domain[64];

    // the root
    tb_char_t                       root[256];

    // the timeout 
    tb_long_t                       timeout;

    // the user agent
    tb_char_t const*                user_agent;

    // the limited rate
    tb_size_t                       limited_rate;

    // the ourl
    tb_char_t                       ourl[TB_DEMO_SPIDER_URL_MAXN];

}tb_demo_spider_t;

// the demo spider parser type
typedef struct __tb_demo_spider_parser_t
{
    // the stream
    tb_stream_ref_t                 stream;

    // the reader
    tb_xml_reader_ref_t             reader;

    // the cache
    tb_circle_queue_ref_t           cache;

    // the iurl
    tb_url_t                        iurl;

}tb_demo_spider_parser_t;

// the demo spider task type
typedef struct __tb_demo_spider_task_t
{
    // the pool
    tb_demo_spider_t*               spider;

    // the iurl
    tb_char_t                       iurl[TB_DEMO_SPIDER_URL_MAXN];

    // the ourl
    tb_char_t                       ourl[TB_DEMO_SPIDER_URL_MAXN];

}tb_demo_spider_task_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */ 
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
static tb_option_item_t g_options[] = 
{
    {'t',   "timeout",      TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_INTEGER,     "set the timeout"               }
,   {'d',   "directory",    TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_CSTR,        "set the root directory"        }
,   {'u',   "agent",        TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_CSTR,        "set the user agent"            }
,   {'r',   "rate",         TB_OPTION_MODE_KEY_VAL,     TB_OPTION_TYPE_INTEGER,     "set limited rate"              }
,   {'h',   "help",         TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "display this help and exit"    } 
,   {'o',   "only",         TB_OPTION_MODE_KEY,         TB_OPTION_TYPE_BOOL,        "only this home website"        }
,   {'-',   "home",         TB_OPTION_MODE_VAL,         TB_OPTION_TYPE_CSTR,        "the home url"                  }
,   {'-',   tb_null,        TB_OPTION_MODE_END,         TB_OPTION_TYPE_NONE,        tb_null                         }
};
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */ 
#if defined(TB_CONFIG_MODULE_HAVE_CHARSET) \
    && defined(TB_CONFIG_MODULE_HAVE_ASIO) \
    && defined(TB_CONFIG_MODULE_HAVE_XML)
static tb_void_t tb_demo_spider_task_exit(tb_demo_spider_task_t* task);
static tb_bool_t tb_demo_spider_task_done(tb_demo_spider_t* spider, tb_char_t const* iurl, tb_bool_t* full);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_bool_t tb_demo_spider_parser_open_html(tb_stream_ref_t stream, tb_char_t const* url)
{
    // check
    tb_assert_and_check_return_val(stream && url, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // find the .suffix
        tb_char_t const* p = tb_strrchr(url, '.');
        if (p)
        {
            // not html?
            tb_check_break (    tb_stricmp(p, ".css")
                            &&  tb_stricmp(p, ".js")
                            &&  tb_stricmp(p, ".png")
                            &&  tb_stricmp(p, ".jpg")
                            &&  tb_stricmp(p, ".gif")
                            &&  tb_stricmp(p, ".ico")
                            &&  tb_stricmp(p, ".bmp")
                            &&  tb_stricmp(p, ".mp4")
                            &&  tb_stricmp(p, ".mp3")
                            &&  tb_stricmp(p, ".flv")
                            &&  tb_stricmp(p, ".avi")
                            &&  tb_stricmp(p, ".exe")
                            &&  tb_stricmp(p, ".msi")
                            &&  tb_stricmp(p, ".zip")
                            &&  tb_stricmp(p, ".rar")
                            &&  tb_stricmp(p, ".7z"));
        }

        // ctrl stream
        if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_URL, url)) break;

        // open stream
        if (!tb_stream_open(stream)) break;

        // the stream size
        tb_hong_t size = tb_stream_size(stream);
        tb_check_break(size);

        // prefetch some data
        tb_byte_t*  data = tb_null;
        tb_size_t   need = tb_min((tb_size_t)size, 256);
        if (!tb_stream_need(stream, &data, need)) break;

        // is html?
        if (tb_strnistr((tb_char_t const*)data, need, "<!DOCTYPE html>"))
        {
            ok = tb_true;
            break;
        }

        // is html?
        ok = tb_strnistr((tb_char_t const*)data, need, "<html")? tb_true : tb_false;

    } while (0);

    // failed?
    if (!ok) 
    {
        // clos stream
        if (stream) tb_stream_clos(stream);
    }

    // ok?
    return ok;
}
static tb_size_t tb_demo_spider_parser_get_url(tb_xml_reader_ref_t reader, tb_url_ref_t url)
{
    // check
    tb_assert_and_check_return_val(reader && url, tb_false);

    // done
    tb_size_t ok = 0;
    tb_size_t event = TB_XML_READER_EVENT_NONE;
    while (!ok && (event = tb_xml_reader_next(reader)))
    {
        switch (event)
        {
        case TB_XML_READER_EVENT_ELEMENT_EMPTY: 
        case TB_XML_READER_EVENT_ELEMENT_BEG: 
            {
                // the element name
                tb_char_t const* name = tb_xml_reader_element(reader);
                tb_check_break(name);

                // <a href="" />? 
                // <link href="" /> 
                // <img src="" />? 
                // <script src="" />? 
                // <source src="" />? 
                // <frame src="" />? 
                if (    !tb_stricmp(name, "a")
                    ||  !tb_stricmp(name, "link")
                    ||  !tb_stricmp(name, "img")
                    ||  !tb_stricmp(name, "frame")
                    ||  !tb_stricmp(name, "source"))
                {
                    // walk attributes
                    tb_xml_node_ref_t attr = (tb_xml_node_ref_t)tb_xml_reader_attributes(reader); 
                    for (; attr; attr = attr->next)
                    {
                        // href or src?
                        if (    tb_string_size(&attr->data)
                            &&  (   !tb_string_cstricmp(&attr->name, "href")
                                ||  !tb_string_cstricmp(&attr->name, "src")))
                        {
                            // the url protocol
                            tb_size_t protocol = tb_url_protocol_probe(tb_string_cstr(&attr->data));

                            // http?
                            if(protocol == TB_URL_PROTOCOL_HTTP)
                            {
                                // save url
                                ok = tb_url_set(url, tb_string_cstr(&attr->data));
                            }
                            // file?
                            else if (protocol == TB_URL_PROTOCOL_FILE)
                            {
                                // save path
                                tb_url_path_set(url, tb_string_cstr(&attr->data));

                                // ok
                                ok = tb_true;
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
        }
    }

    // ok?
    return ok;
}
static tb_void_t tb_demo_spider_parser_exit(tb_thread_pool_worker_ref_t worker, tb_cpointer_t priv)
{
    // check
    tb_demo_spider_parser_t* parser = (tb_demo_spider_parser_t*)priv;
    tb_assert_and_check_return(parser);

    // exit stream
    if (parser->stream) tb_stream_exit(parser->stream);
    parser->stream = tb_null;

    // exit reader
    if (parser->reader) tb_xml_reader_exit(parser->reader);
    parser->reader = tb_null;

    // exit cache
    if (parser->cache) tb_circle_queue_exit(parser->cache);
    parser->cache = tb_null;

    // exit iurl
    tb_url_exit(&parser->iurl);

    // exit it
    tb_free(parser);
}
static tb_demo_spider_parser_t* tb_demo_spider_parser_init(tb_thread_pool_worker_ref_t worker)
{
    // check 
    tb_assert_and_check_return_val(worker, tb_null);

    // done
    tb_bool_t                   ok = tb_false;
    tb_demo_spider_parser_t*    parser = tb_null;
    do
    {
        // attempt to get the parser
        parser = (tb_demo_spider_parser_t*)tb_thread_pool_worker_getp(worker, 0);
        if (!parser)
        {
            // make parser
            parser = tb_malloc0_type(tb_demo_spider_parser_t);
            tb_assert_and_check_break(parser);

            // save parser
            tb_thread_pool_worker_setp(worker, 0, tb_demo_spider_parser_exit, (tb_cpointer_t)parser);

            // init stream
            parser->stream = tb_stream_init_file();
            tb_assert_and_check_break(parser->stream);

            // init reader
            parser->reader = tb_xml_reader_init();
            tb_assert_and_check_break(parser->reader);

            // init cache
            parser->cache = tb_circle_queue_init(255, tb_item_func_str(tb_true));
            tb_assert_and_check_break(parser->cache);

            // init iurl
            if (!tb_url_init(&parser->iurl)) break;
        }

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (parser) tb_demo_spider_parser_exit(worker, (tb_cpointer_t)parser);
        parser = tb_null;
    }

    // ok
    return parser;
}
static tb_void_t tb_demo_spider_parser_task_done(tb_thread_pool_worker_ref_t worker, tb_cpointer_t priv)
{
    // check
    tb_demo_spider_task_t* task = (tb_demo_spider_task_t*)priv;
    tb_assert_and_check_return(worker && task && task->spider);

    // init parser
    tb_demo_spider_parser_t* parser = tb_demo_spider_parser_init(worker);
    tb_assert_and_check_return(parser && parser->stream && parser->reader && parser->cache);

    // open stream
    if (tb_demo_spider_parser_open_html(parser->stream, task->ourl))
    {
        // open reader
        if (tb_xml_reader_open(parser->reader, parser->stream, tb_false))
        {
            // trace
            tb_trace_d("parser: open: %s", task->ourl);

            // init url
            tb_url_set(&parser->iurl, task->iurl);

            // parse url
            while (     TB_STATE_OK == tb_atomic_get(&task->spider->state)
                    &&  tb_demo_spider_parser_get_url(parser->reader, &parser->iurl))
            {
                // trace
                tb_trace_d("parser: done: %s", tb_url_get(&parser->iurl));

                // done task
                tb_bool_t full = tb_false;
                if (!tb_demo_spider_task_done(task->spider, tb_url_get(&parser->iurl), &full))
                {
                    // full?
                    tb_assert_and_check_break(full);

                    // cache url
                    if (!tb_circle_queue_full(parser->cache)) tb_circle_queue_put(parser->cache, tb_url_get(&parser->iurl));

                    // trace
                    tb_trace_d("parser: cache: save: %s, size: %lu", tb_url_get(&parser->iurl), tb_circle_queue_size(parser->cache));
                }
            }

            // clos reader
            tb_xml_reader_clos(parser->reader);
        }

        // clos stream
        tb_stream_clos(parser->stream);
    }

    // done task from the cache
    while (!tb_circle_queue_null(parser->cache))
    {
        // the url
        tb_char_t const* url = (tb_char_t const*)tb_circle_queue_get(parser->cache);
        tb_assert_and_check_break(url);

        // done task
        if (!tb_demo_spider_task_done(task->spider, url, tb_null)) break;

        // trace
        tb_trace_d("parser: cache: load: %s, size: %lu", url, tb_circle_queue_size(parser->cache));

        // pop it
        tb_circle_queue_pop(parser->cache);
    }
}
static tb_void_t tb_demo_spider_parser_task_exit(tb_thread_pool_worker_ref_t worker, tb_cpointer_t priv)
{
    // check
    tb_demo_spider_task_t* task = (tb_demo_spider_task_t*)priv;
    tb_assert_and_check_return(worker && task);
   
    // exit task
    tb_demo_spider_task_exit(task);
}
static tb_bool_t tb_demo_spider_make_ourl(tb_demo_spider_t* spider, tb_char_t const* url, tb_char_t* data, tb_size_t maxn)
{
    // check
    tb_assert_and_check_return_val(spider && url && data && maxn, tb_false);

    // skip protocol
	tb_char_t* p = (tb_char_t*)url;
	if (!tb_strnicmp(p, "http://", 7)) p += 7;
	else if (!tb_strnicmp(p, "https://", 8)) p += 8;

    // skip space
    while (*p && tb_isspace(*p)) p++;

	// format ourl
	tb_long_t n = tb_snprintf(data, maxn, "%s/%s", spider->root, p);
    tb_assert_and_check_return_val(n > 0 && n < maxn, tb_false);

    // no root? append '/'
    if (!tb_strchr(p, '/') && !tb_strchr(p, '\\')) data[n++] = '/';
    tb_assert_and_check_return_val(n < maxn, tb_false);

    // '\\' => '/'
    if (data[n - 1] == '/') data[n - 1] = '/';

    // directory? append index.html
    if (data[n - 1] == '/') n += tb_snprintf(data + n, maxn - n, "%s", "index.html");
    tb_assert_and_check_return_val(n > 0 && n < maxn, tb_false);

    // end
	data[n] = '\0';

    // replace '?' => '_'
    p = data;
    while (*p)
    {
        // replace
        if (*p == '?') *p = '_';

        // next
        p++;
    }

	// trace
	tb_trace_d("make: %s => %s", url, data);

	// ok?
	return n > 0? tb_true : tb_false;
}
static tb_void_t tb_demo_spider_task_exit(tb_demo_spider_task_t* task)
{
    // check
    tb_assert_and_check_return(task);

    // the spider 
    tb_demo_spider_t* spider = task->spider;
    tb_assert_and_check_return(spider);

    // trace
    tb_trace_d("task: exit: %s", task->iurl);

    // enter
    tb_spinlock_enter(&spider->lock);

    // exit task
    if (spider->pool) tb_fixed_pool_free(spider->pool, task);

    // leave
    tb_spinlock_leave(&spider->lock);
}
static tb_bool_t tb_demo_spider_task_save(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // check
    tb_demo_spider_task_t* task = (tb_demo_spider_task_t*)priv;
    tb_assert_and_check_return_val(task && task->spider, tb_false);

    // percent
#ifdef TB_TRACE_DEBUG
    tb_size_t percent = 0;
    if (size > 0) percent = (tb_size_t)((offset * 100) / size);
    else if (state == TB_STATE_OK) percent = 100;

    // trace
    tb_trace_d("save[%s]: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", task->iurl, save, rate, percent, tb_state_cstr(state));
#endif

    // ok? continue it
    tb_bool_t ok = tb_false;
    if (state == TB_STATE_OK) ok = tb_true;
    // closed?
    else if (state == TB_STATE_CLOSED && TB_STATE_OK == tb_atomic_get(&task->spider->state))
    {
        // trace
        tb_trace_i("task: done: %s: ok", task->iurl);

        // post parser task
        tb_thread_pool_task_post(tb_thread_pool(), "parser_task", tb_demo_spider_parser_task_done, tb_demo_spider_parser_task_exit, task, tb_false);
    }
    // failed or killed?
    else 
    {
        // trace
        tb_trace_e("task: done: %s: %s", task->iurl, tb_state_cstr(state));

        // exit task
        tb_demo_spider_task_exit(task);
    }

    // break or continue?
    return ok;
}
static tb_bool_t tb_demo_spider_task_ctrl(tb_async_stream_ref_t istream, tb_async_stream_ref_t ostream, tb_cpointer_t priv)
{
    // check
    tb_demo_spider_task_t* task = (tb_demo_spider_task_t*)priv;
    tb_assert_and_check_return_val(task && task->spider, tb_false);
    tb_assert_and_check_return_val(istream && ostream, tb_false);
    tb_assert_and_check_return_val(tb_async_stream_type(istream) == TB_STREAM_TYPE_HTTP, tb_false);

    // the url
    tb_char_t const* url = tb_null;
    if (!tb_async_stream_ctrl(istream, TB_STREAM_CTRL_GET_URL, &url)) return tb_false;

    // trace
    tb_trace_d("ctrl: %s: ..", url);

    // set timeout
    if (!tb_async_stream_ctrl(istream, TB_STREAM_CTRL_SET_TIMEOUT, task->spider->timeout)) return tb_false;

#if defined(TB_CONFIG_MODULE_HAVE_ZIP) && defined(TB_CONFIG_THIRD_HAVE_ZLIB)
    // need gzip
    if (!tb_async_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_HEAD, "Accept-Encoding", "gzip,deflate")) return tb_false;

    // auto unzip
    if (!tb_async_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_AUTO_UNZIP, 1)) return tb_false;
#endif

    // user agent
    if (!tb_async_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_HEAD, "User-Agent", task->spider->user_agent)) return tb_false;

    // enable cookies
    if (!tb_async_stream_ctrl(istream, TB_STREAM_CTRL_HTTP_SET_COOKIES, tb_cookies())) return tb_false;

    // ok
    return tb_true;
}
static tb_bool_t tb_demo_spider_task_done(tb_demo_spider_t* spider, tb_char_t const* iurl, tb_bool_t* full)
{
    // check
    tb_assert_and_check_return_val(spider && iurl, tb_false);

    // killed?
    tb_check_return_val(TB_STATE_OK == tb_atomic_get(&spider->state), tb_false);

    // only for home?
    if (spider->home_only && !tb_stristr(iurl, spider->home_domain)) 
    {
        // trace
        tb_trace_d("task: done: %s: skip", iurl);
        return tb_true;
    }

    // enter
    tb_spinlock_enter(&spider->lock);

    // done
    tb_bool_t               ok = tb_false;
    tb_size_t               size = 0;
    tb_demo_spider_task_t*  task = tb_null;
    tb_bool_t               repeat = tb_false;
    do
    {
        // check
        tb_assert_and_check_break(spider->filter && spider->pool);

        // the task count
        size = tb_fixed_pool_size(spider->pool);
        
        // make the output url
        if (!tb_demo_spider_make_ourl(spider, iurl, spider->ourl, sizeof(spider->ourl) - 1)) break;

        // have been done already?
        if (!tb_bloom_filter_set(spider->filter, spider->ourl)) 
        {
            // trace
            tb_trace_d("task: size: %lu, done: %s: repeat", size, iurl);

            // ok
            ok = tb_true;
            repeat = tb_true;
            break;
        }

        // trace
        tb_trace_d("task: size: %lu, done: %s: ..", size, iurl);

        // full?
        tb_check_break(size < TB_DEMO_SPIDER_TASK_MAXN);

        // make task
        task = (tb_demo_spider_task_t*)tb_fixed_pool_malloc0(spider->pool);
        tb_assert_and_check_break(task);

        // init task
        task->spider = spider;
        tb_strlcpy(task->iurl, iurl, sizeof(task->iurl) - 1);
        tb_strlcpy(task->ourl, spider->ourl, sizeof(task->ourl) - 1);
           
        // ok 
        ok = tb_true;

    } while (0);

    // leave
    tb_spinlock_leave(&spider->lock);

    // ok?
    if (ok && !repeat)
    {
        // done
        ok = tb_false;
        do
        {
            // check
            tb_assert_and_check_break(task);

            // killed?
            tb_check_break(TB_STATE_OK == tb_atomic_get(&spider->state));

            // repeat?
            if (tb_file_info(task->ourl, tb_null))
            {
                // trace
                tb_trace_d("task: size: %lu, done: %s: repeat", size, iurl);

                // ok
                ok = tb_true;
                repeat = tb_true;
                break;
            }

            // done task
            ok = tb_transfer_pool_done(tb_transfer_pool(), task->iurl, task->ourl, 0, spider->limited_rate, tb_demo_spider_task_save, tb_demo_spider_task_ctrl, task);
            tb_assert_and_check_break(ok);

        } while (0);
    }

    // repeat or failed?
    if (repeat || !ok)
    {
        // exit task
        if (task) tb_demo_spider_task_exit(task);
        task = tb_null;

        // failed?
        if (!full && !ok)
        {
            // trace
            tb_trace_e("task: size: %lu, done: %s: post failed", size, iurl);
        }

        // save full
        if (full) *full = size < TB_DEMO_SPIDER_TASK_MAXN? tb_false : tb_true;
    }

    // ok?
    return ok;
}
static tb_bool_t tb_demo_spider_init(tb_demo_spider_t* spider, tb_int_t argc, tb_char_t** argv)
{
    // check
    tb_assert_and_check_return_val(spider && argc && argv, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
        // init option
        spider->option = tb_option_init("spider", "the spider demo", g_options);
        tb_assert_and_check_break(spider->option);
 
        // done option
        if (!tb_option_done(spider->option, argc - 1, &argv[1])) break;

        // check
        tb_assert_and_check_break(tb_option_find(spider->option, "home"));

        // init home
        if (!tb_url_init(&spider->home)) break;
        tb_url_set(&spider->home, tb_option_item_cstr(spider->option, "home"));
        tb_trace_d("home: %s", tb_url_get(&spider->home));

        // init only home?
        if (tb_option_find(spider->option, "only"))
            spider->home_only = tb_option_item_bool(spider->option, "only");

        // init root
        tb_char_t const* root = tb_option_item_cstr(spider->option, "directory");

        // init user agent
        spider->user_agent = tb_option_item_cstr(spider->option, "agent");

        // init timeout
        if (tb_option_find(spider->option, "timeout"))
            spider->timeout = tb_option_item_sint32(spider->option, "timeout");

        // init limited rate
        if (tb_option_find(spider->option, "rate"))
            spider->limited_rate = tb_option_item_uint32(spider->option, "rate");
#else

        // check
        tb_assert_and_check_break(argv[1]);

        // init home
        if (!tb_url_init(&spider->home)) break;
        spider->home = argv[1]? argv[1] : tb_null;
        tb_trace_d("home: %s", tb_url_get(&spider->home));

        // init root
        tb_char_t const* root = argv[2];
#endif

        // the home host
        tb_char_t const* host = tb_url_host_get(&spider->home);
        tb_assert_and_check_break(host);

        // init home domain
        tb_char_t const* domain = tb_strchr(host, '.');
        if (domain)
        {
            tb_strlcpy(spider->home_domain, domain, sizeof(spider->home_domain) - 1);
            spider->home_domain[sizeof(spider->home_domain) - 1] = '\0';
        }

        // using the default root
        if (root) tb_strlcpy(spider->root, root, sizeof(spider->root) - 1);
        else 
        {
            // the temporary root
            tb_directory_temp(spider->root, sizeof(spider->root) - 1);

            // append spider
            tb_strcat(spider->root, "/spider");
        }
        tb_trace_d("root: %s", spider->root);

        // using the default user agent
        if (!spider->user_agent) spider->user_agent = TB_DEMO_SPIDER_USER_AGENT;

        // using the default timeout
        if (!spider->timeout) spider->timeout = TB_DEMO_SPIDER_TASK_TIMEOUT;

        // using the default rate
        if (!spider->limited_rate) spider->limited_rate = TB_DEMO_SPIDER_TASK_RATE;

        // strip root tail: '/' or '\\'
        tb_size_t size = tb_strlen(spider->root);
        if (size && (spider->root[size - 1] == '/' || spider->root[size - 1] == '\\')) spider->root[size - 1] = '\0';

        // init state
        spider->state = TB_STATE_OK;

        // init lock
        if (!tb_spinlock_init(&spider->lock)) break;

        // init pool
        spider->pool = tb_fixed_pool_init(tb_null, TB_DEMO_SPIDER_TASK_MAXN >> 2, sizeof(tb_demo_spider_task_t), tb_null, tb_null, tb_null);
        tb_assert_and_check_break(spider->pool);

        // init filter
        spider->filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 3, TB_DEMO_SPIDER_FILTER_MAXN, tb_item_func_str(tb_true));
        tb_assert_and_check_break(spider->filter);

        // register lock profiler
#ifdef TB_LOCK_PROFILER_ENABLE
        tb_lock_profiler_register(tb_lock_profiler(), (tb_pointer_t)&spider->lock, "spider");
#endif

        // ok
        ok = tb_true;

    } while (0);

    // failed? help it
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
    if (!ok && spider->option) tb_option_help(spider->option);
#endif

    // ok?
    return ok;
}
static tb_void_t tb_demo_spider_exit(tb_demo_spider_t* spider)
{
    // check
    tb_assert_and_check_return(spider);

    // trace
    tb_trace_d("exit: ..");

    // kill it
    tb_atomic_set(&spider->state, TB_STATE_KILLING);

    // kill all transfer tasks
    tb_transfer_pool_kill_all(tb_transfer_pool());

    // kill all parser tasks
    tb_thread_pool_task_kill_all(tb_thread_pool());

    // wait all transfer tasks exiting
    tb_transfer_pool_wait_all(tb_transfer_pool(), -1);

    // wait all parser tasks exiting
    tb_thread_pool_task_wait_all(tb_thread_pool(), -1);

    // enter
    tb_spinlock_enter(&spider->lock);

    // exit filter
    if (spider->filter) tb_bloom_filter_exit(spider->filter);
    spider->filter = tb_null;

    // exit pool
    if (spider->pool) tb_fixed_pool_exit(spider->pool);
    spider->pool = tb_null;

    // leave
    tb_spinlock_leave(&spider->lock);

    // exit lock
    tb_spinlock_exit(&spider->lock);

    // exit home
    tb_url_exit(&spider->home);

    // exit option
#ifdef TB_CONFIG_MODULE_HAVE_OBJECT
    if (spider->option) tb_option_exit(spider->option);
    spider->option = tb_null;
#endif

    // trace
    tb_trace_d("exit: ok");
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_spider_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_demo_spider_t spider = {0};
    do
    {
        // init spider
        if (!tb_demo_spider_init(&spider, argc, argv)) break;

        // done the home task if exists
        tb_demo_spider_task_done(&spider, tb_url_get(&spider.home), tb_null);

        // wait 
        getchar();

    } while (0);

    // exit spider
    tb_demo_spider_exit(&spider);

    // end
    return 0;
}
#else
tb_int_t tb_demo_network_spider_main(tb_int_t argc, tb_char_t** argv)
{
    return 0;
}
#endif
