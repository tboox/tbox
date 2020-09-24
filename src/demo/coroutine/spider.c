/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME            "spider"
#define TB_TRACE_MODULE_DEBUG           (1)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the timeout
//#define TB_DEMO_TIMEOUT             (-1)
#define TB_DEMO_TIMEOUT             (15000)

// the limit rate, 256KB/s
#define TB_DEMO_LIMITRATE           (0)
//#define TB_DEMO_LIMITRATE           (256000)

// the filter maxn
#define TB_DEMO_FILTER_MAXN         (100000)

// the task maxn
#define TB_DEMO_TASK_MAXN           (50)

// the stack size
#define TB_DEMO_STACKSIZE           (8192 << 4)

// the user agent
#define TB_DEMO_USER_AGENT           "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_9_2) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/34.0.1847.137 Safari/537.36"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the spider type
typedef struct __tb_demo_spider_t
{
    // the homepage
    tb_char_t const*        homepage;

    // the root directory
    tb_char_t               rootdir[TB_PATH_MAXN];

    // the bloom filter
    tb_bloom_filter_ref_t   filter;

    // the urls
    tb_co_channel_ref_t     urls;

    // the scheduler
    tb_co_scheduler_ref_t   scheduler;

}tb_demo_spider_t, *tb_demo_spider_ref_t;

// the spider parser type
typedef struct __tb_demo_spider_parser_t
{
    // the stream
    tb_stream_ref_t         stream;

    // the data
    tb_byte_t*              data;

    // the data size
    tb_size_t               size;

    // the data maxn
    tb_size_t               maxn;

    // the data offset
    tb_size_t               offset;

    // the path buffer
    tb_char_t               path[TB_PATH_MAXN];

    // the url
    tb_url_t                url;

}tb_demo_spider_parser_t, *tb_demo_spider_parser_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */
static tb_void_t tb_demo_spider_urls_free(tb_pointer_t data, tb_cpointer_t priv);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_void_t tb_demo_spider_exit(tb_demo_spider_ref_t spider)
{
    // check
    tb_assert_and_check_return(spider);

    // exit homepage
    if (spider->homepage) tb_free(spider->homepage);
    spider->homepage = tb_null;

    // exit filter
    if (spider->filter) tb_bloom_filter_exit(spider->filter);
    spider->filter = tb_null;

    // exit scheduler
    if (spider->scheduler) tb_co_scheduler_exit(spider->scheduler);
    spider->scheduler = tb_null;

    // exit urls
    if (spider->urls) tb_co_channel_exit(spider->urls);
    spider->urls = tb_null;

    // exit spider
    tb_free(spider);
}
static tb_demo_spider_ref_t tb_demo_spider_init(tb_char_t const* homepage, tb_char_t const* rootdir)
{
    // check
    tb_assert_and_check_return_val(homepage, tb_null);

    // done
    tb_bool_t               ok = tb_false;
    tb_demo_spider_ref_t    spider = tb_null;
    do
    {
        // make spider
        spider = tb_malloc0_type(tb_demo_spider_t);
        tb_assert_and_check_break(spider);

        // init homepage
        spider->homepage = tb_strdup(homepage);
        tb_assert_and_check_break(spider->homepage);

        // init the root directory
        if (rootdir) tb_strlcpy(spider->rootdir, rootdir, sizeof(spider->rootdir));
        else tb_directory_current(spider->rootdir, sizeof(spider->rootdir));

        // init filter
        spider->filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 3, TB_DEMO_FILTER_MAXN, tb_element_str(tb_true));
        tb_assert_and_check_break(spider->filter);

        // init scheduler
        spider->scheduler = tb_co_scheduler_init();
        tb_assert_and_check_break(spider->scheduler);

        // init urls
        spider->urls = tb_co_channel_init(TB_DEMO_TASK_MAXN, tb_demo_spider_urls_free, tb_null);
        tb_assert_and_check_break(spider->urls);

        // trace
        tb_trace_i("rootdir: %s, homepage: %s", spider->rootdir, spider->homepage);

        // ok
        ok = tb_true;

    } while (0);

    // failed
    if (!ok)
    {
        // exit it
        if (spider) tb_demo_spider_exit(spider);
        spider = tb_null;
    }

    // ok?
    return spider;
}
static tb_void_t tb_demo_spider_urls_free(tb_pointer_t data, tb_cpointer_t priv)
{
    if (data) tb_free(data);
}
static tb_bool_t tb_demo_spider_make_ourl(tb_demo_spider_ref_t spider, tb_char_t const* url, tb_char_t* data, tb_size_t maxn)
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
    tb_long_t n = tb_snprintf(data, maxn, "%s/%s", spider->rootdir, p);
    tb_check_return_val(n > 0 && n < maxn, tb_false);

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
static tb_bool_t tb_demo_spider_parser_open(tb_demo_spider_parser_ref_t parser, tb_char_t const* iurl, tb_char_t const* ourl)
{
    // check
    tb_assert_and_check_return_val(parser && parser->stream && iurl && ourl, tb_false);

    // done
    tb_bool_t ok = tb_false;
    do
    {
        // find the .suffix
        tb_char_t const* p = tb_strrchr(ourl, '.');
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
        if (!tb_stream_ctrl(parser->stream, TB_STREAM_CTRL_SET_URL, ourl)) break;

        // open stream
        if (!tb_stream_open(parser->stream)) break;

        // the stream size
        tb_hong_t size = tb_stream_size(parser->stream);
        tb_check_break(size && size < 1024 * 1024);

        // prefetch some data
        tb_byte_t*  data = tb_null;
        tb_size_t   need = tb_min((tb_size_t)size, 256);
        if (!tb_stream_need(parser->stream, &data, need)) break;

        // is html?
        if (    tb_strnistr((tb_char_t const*)data, need, "<!DOCTYPE html>")
            ||  tb_strnistr((tb_char_t const*)data, need, "<html"))
        {
            // set iurl
            if (!tb_url_cstr_set(&parser->url, iurl)) break;

            // init data
            if (!parser->data)
            {
                parser->maxn = (tb_size_t)size + 1;
                parser->data = tb_malloc_bytes(parser->maxn);
            }
            else if (size + 1 > parser->maxn)
            {
                parser->maxn = (tb_size_t)size + 1;
                parser->data = tb_ralloc_bytes(parser->data, parser->maxn);
            }

            // read data
            if (parser->data && tb_stream_bread(parser->stream, parser->data, (tb_size_t)size))
            {
                // save data
                parser->size = (tb_size_t)size;
                parser->offset = 0;
                parser->data[size] = '\0';
                ok = tb_true;
            }
        }

    } while (0);

    // failed?
    if (!ok) tb_stream_clos(parser->stream);

    // ok?
    return ok;
}
static tb_void_t tb_demo_spider_parser_clos(tb_demo_spider_parser_ref_t parser)
{
    // check
    tb_assert_and_check_return(parser);

    // reset data
    parser->size = 0;
    parser->offset = 0;

    // close stream
    if (parser->stream) tb_stream_clos(parser->stream);
}
static tb_char_t const* tb_demo_spider_parser_read(tb_demo_spider_parser_ref_t parser)
{
    // check
    tb_assert_and_check_return_val(parser && parser->data && parser->size, tb_null);

    // end?
    tb_check_return_val(parser->offset + 16 < parser->size, tb_null);

    /* read the next url
     *
     * <a href="" />?
     * <link href="" />
     * <img src="" />?
     * <script src="" />?
     * <source src="" />?
     * <frame src="" />?
     */
    tb_bool_t ok = tb_false;
    while (!ok && parser->offset + 16 < parser->size)
    {
        tb_char_t const* b = (tb_char_t const*)parser->data + parser->offset;
        tb_size_t        n = parser->size - parser->offset;
        tb_char_t const* p = tb_strnistr(b, n, " href=");
        if (!p) p = tb_strnistr(b, n, " src=");
        if (p)
        {
            // seek to "url"
            while (p < b + n && *p && *p != '\"') p++;
            p++;

            // get url
            tb_char_t const* e = p;
            while (e < b + n && *e && *e != '\"') e++;
            if (p < e && e - p < sizeof(parser->path) && e - p > 16)
            {
                // save path
                tb_strncpy(parser->path, p, e - p);
                parser->path[e - p] = '\0';

                // http?
                tb_size_t protocol = tb_url_protocol_probe(parser->path);
                if (protocol == TB_URL_PROTOCOL_HTTP)
                    ok = tb_url_cstr_set(&parser->url, parser->path);
                else if (protocol == TB_URL_PROTOCOL_FILE)
                {
                    tb_url_path_set(&parser->url, parser->path);
                    ok = tb_true;
                }
            }

            // update offset
            parser->offset = e - (tb_char_t const*)parser->data + 1;
        }
        else break;
    }

    // ok?
    return ok? tb_url_cstr(&parser->url) : tb_null;
}
static tb_void_t tb_demo_spider_parser_exit(tb_demo_spider_parser_ref_t parser)
{
    // check
    tb_assert_and_check_return(parser);

    // exit url
    tb_url_exit(&parser->url);

    // exit stream
    if (parser->stream) tb_stream_exit(parser->stream);
    parser->stream = tb_null;

    // exit data
    if (parser->data) tb_free(parser->data);
    parser->data = tb_null;
    parser->size = 0;
    parser->maxn = 0;

    // exit it
    tb_free(parser);
}
static tb_demo_spider_parser_ref_t tb_demo_spider_parser_init()
{
    // done
    tb_bool_t                   ok = tb_false;
    tb_demo_spider_parser_ref_t parser = tb_null;
    do
    {
        // make parser
        parser = tb_malloc0_type(tb_demo_spider_parser_t);
        tb_assert_and_check_break(parser);

        // init stream
        parser->stream = tb_stream_init_file();
        tb_assert_and_check_break(parser->stream);

        // init url
        if (!tb_url_init(&parser->url)) break;

        // ok
        ok = tb_true;

    } while (0);

    // failed?
    if (!ok)
    {
        // exit it
        if (parser) tb_demo_spider_parser_exit(parser);
        parser = tb_null;
    }

    // ok
    return parser;
}
static tb_bool_t tb_demo_spider_page_save(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // check
    tb_char_t const* url = (tb_char_t const*)priv;
    tb_assert_and_check_return_val(url, tb_false);

    // percent
#ifdef __tb_debug__
    tb_size_t percent = 0;
    if (size > 0) percent = (tb_size_t)((offset * 100) / size);
    else if (state == TB_STATE_OK) percent = 100;

    // trace
    tb_trace_d("save[%s]: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", url, save, rate, percent, tb_state_cstr(state));
#endif

    // ok? continue it
    tb_bool_t ok = tb_false;
    if (state == TB_STATE_OK) ok = tb_true;
    // closed?
    else if (state == TB_STATE_CLOSED)
    {
        // trace
        tb_trace_i("save[%s]: ok", url);
        ok = tb_true;
    }
    // failed or killed?
    else
    {
        // trace
        tb_trace_e("save[%s]: %s", url, tb_state_cstr(state));
    }

    // break or continue?
    return ok;
}
static tb_void_t tb_demo_spider_page_grab(tb_cpointer_t priv)
{
    // check
    tb_demo_spider_ref_t spider = (tb_demo_spider_ref_t)priv;
    tb_assert_and_check_return(spider && spider->urls);

    // done
    tb_stream_ref_t             stream = tb_null;
    tb_char_t                   ourl[TB_PATH_MAXN];
    tb_char_t const*            iurl = tb_null;
    tb_demo_spider_parser_ref_t parser = tb_null;
    while (1)
    {
        // recv url
        iurl = (tb_char_t const*)tb_co_channel_recv(spider->urls);
        tb_check_break(iurl);

        // trace
        tb_trace_d("grab: %s", iurl);

        // init stream
        if (!stream)
        {
            // init stream
            stream = tb_stream_init_from_url(iurl);
            tb_assert_and_check_break(stream);

            // set timeout
            if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_TIMEOUT, TB_DEMO_TIMEOUT)) break;

#if defined(TB_CONFIG_MODULE_HAVE_ZIP) && defined(TB_CONFIG_PACKAGE_HAVE_ZLIB)
            // need gzip
            if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_HTTP_SET_HEAD, "Accept-Encoding", "gzip,deflate")) break;

            // auto unzip
            if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_HTTP_SET_AUTO_UNZIP, 1)) break;
#endif

            // user agent
            if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_HTTP_SET_HEAD, "User-Agent", TB_DEMO_USER_AGENT)) break;

            // enable cookies
            if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_HTTP_SET_COOKIES, tb_cookies())) break;
        }
        else
        {
            // set url
            if (!tb_stream_ctrl(stream, TB_STREAM_CTRL_SET_URL, iurl)) break;
        }

        // make the output url
        if (!tb_demo_spider_make_ourl(spider, iurl, ourl, sizeof(ourl))) break;

        // save stream to file
        if (tb_transfer_to_url(stream, ourl, TB_DEMO_LIMITRATE, tb_demo_spider_page_save, iurl) > 0)
        {
            // init parser first
            if (!parser) parser = tb_demo_spider_parser_init();
            tb_assert_and_check_break(parser);

            // open parser
            if (tb_demo_spider_parser_open(parser, iurl, ourl))
            {
                // read url
                tb_char_t const* url = tb_null;
                while ((url = tb_demo_spider_parser_read(parser)))
                {
                    // have been cached already?
                    if (!tb_bloom_filter_set(spider->filter, url))
                        continue ;

                    // trace
                    tb_trace_d("send %s", url);

                    // try to send url
                    url = tb_strdup(url);
                    if (url && !tb_co_channel_send_try(spider->urls, url))
                    {
                        tb_free(url);
                        break;
                    }
                }

                // close parser
                tb_demo_spider_parser_clos(parser);
            }
        }

        // close stream
        tb_stream_clos(stream);

        // exit url
        tb_free(iurl);
    }

    // exit url
    if (iurl) tb_free(iurl);
    iurl = tb_null;

    // exit stream
    if (stream) tb_stream_exit(stream);
    stream = tb_null;

    // exit parser
    if (parser) tb_demo_spider_parser_exit(parser);
    parser = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_coroutine_spider_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_demo_spider_ref_t    spider = tb_null;
    tb_size_t               count = TB_DEMO_TASK_MAXN;
    do
    {
        // init spider
        spider = tb_demo_spider_init(argv[1], argv[2]);
        tb_assert_and_check_break(spider && spider->scheduler && spider->homepage);

        // send homepage to handler
        tb_co_channel_send(spider->urls, tb_strdup(spider->homepage));

        // grab pages
        while (count--)
        {
            if (!tb_coroutine_start(spider->scheduler, tb_demo_spider_page_grab, spider, TB_DEMO_STACKSIZE)) break;
        }

        // run scheduler
        tb_co_scheduler_loop(spider->scheduler, tb_true);

    } while (0);

    // exit spider
    if (spider) tb_demo_spider_exit(spider);
    spider = tb_null;

    // end
    return 0;
}
