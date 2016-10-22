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
#define TB_DEMO_TIMEOUT             (-1)
//#define TB_DEMO_TIMEOUT             (15000)

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

    // the reader
    tb_xml_reader_ref_t     reader;

    // the spider
    tb_demo_spider_ref_t    spider;

    // the url
    tb_url_t                url;

}tb_demo_spider_parser_t, *tb_demo_spider_parser_ref_t;

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
        spider->urls = tb_co_channel_init(TB_DEMO_TASK_MAXN);
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
static tb_bool_t tb_demo_spider_parser_open(tb_stream_ref_t stream, tb_char_t const* url)
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
static tb_char_t const* tb_demo_spider_parser_read(tb_demo_spider_parser_ref_t parser)
{
    // check
    tb_assert_and_check_return_val(parser && parser->reader, tb_null);

    // done
    tb_bool_t           ok = tb_false;
    tb_size_t           event = TB_XML_READER_EVENT_NONE;
    tb_xml_reader_ref_t reader = parser->reader;
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
                                ok = tb_url_cstr_set(&parser->url, tb_string_cstr(&attr->data));
                            }
                            // file?
                            else if (protocol == TB_URL_PROTOCOL_FILE)
                            {
                                // save path
                                tb_url_path_set(&parser->url, tb_string_cstr(&attr->data));

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

    // exit reader
    if (parser->reader) tb_xml_reader_exit(parser->reader);
    parser->reader = tb_null;

    // exit it
    tb_free(parser);
}
static tb_demo_spider_parser_ref_t tb_demo_spider_parser_init(tb_demo_spider_ref_t spider, tb_char_t const* iurl, tb_char_t const* ourl)
{
    // check
    tb_assert_and_check_return_val(spider && iurl && ourl, tb_null);

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

        // init reader
        parser->reader = tb_xml_reader_init();
        tb_assert_and_check_break(parser->reader);

        // save spider
        parser->spider = spider;

        // init url
        if (!tb_url_init_from_cstr(&parser->url, iurl)) break;

        // open stream
        if (!tb_demo_spider_parser_open(parser->stream, ourl)) break;

        // open reader
        if (!tb_xml_reader_open(parser->reader, parser->stream, tb_false)) break;
        
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
static tb_void_t tb_demo_spider_parser_loop(tb_cpointer_t priv)
{
    // check
    tb_demo_spider_parser_ref_t parser = (tb_demo_spider_parser_ref_t)priv;
    tb_assert_and_check_return(parser && parser->reader && parser->spider && parser->spider->filter);

    // read url
    tb_char_t const* url = tb_null;
    while ((url = tb_demo_spider_parser_read(parser)))
    {
        // have been cached already?
        if (!tb_bloom_filter_set(parser->spider->filter, url)) 
        {
            // trace
            tb_trace_d("skip repeat %s", url);

            // skip it
            continue ;
        }

        // trace
        tb_trace_d("send %s", url);

        // send url 
        tb_co_channel_send(parser->spider->urls, tb_strdup(url));
    }

    // exit parser
    tb_demo_spider_parser_exit(parser);
}
static tb_bool_t tb_demo_spider_page_save(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_cpointer_t priv)
{
    // check
    tb_value_ref_t args = (tb_value_ref_t)priv;
    tb_assert_and_check_return_val(args, tb_false);

    // percent
#ifdef __tb_debug__
    tb_size_t percent = 0;
    if (size > 0) percent = (tb_size_t)((offset * 100) / size);
    else if (state == TB_STATE_OK) percent = 100;

    // trace
    tb_trace_d("save[%s]: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", args[1].cstr, save, rate, percent, tb_state_cstr(state));
#endif

    // ok? continue it
    tb_bool_t ok = tb_false;
    if (state == TB_STATE_OK) ok = tb_true;
    // closed?
    else if (state == TB_STATE_CLOSED)
    {
        // trace
        tb_trace_i("save[%s]: ok", args[1].cstr);

        // init parser
        tb_demo_spider_parser_ref_t parser = tb_demo_spider_parser_init(args[0].ptr, args[1].cstr, args[2].cstr);
        if (parser)
        {
            // parse this page
            ok = tb_coroutine_start(tb_null, tb_demo_spider_parser_loop, parser, TB_DEMO_STACKSIZE);
        }
    }
    // failed or killed?
    else 
    {
        // trace
        tb_trace_e("save[%s]: %s", args[1].cstr, tb_state_cstr(state));
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
    tb_stream_ref_t     stream = tb_null;
    tb_char_t           ourl[TB_PATH_MAXN];
    tb_char_t const*    iurl = tb_null;
    while (1)
    {
        // recv url
        iurl = tb_co_channel_recv(spider->urls);
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
        tb_value_t args[3];
        args[0].ptr     = spider;
        args[1].cstr    = iurl;
        args[2].cstr    = ourl;
        if (tb_transfer_to_url(stream, ourl, TB_DEMO_LIMITRATE, tb_demo_spider_page_save, args) < 0) break;

        // close stream
        tb_stream_clos(stream);

        // exit url
        tb_free(iurl);

    } while (0);

    // exit url
    if (iurl) tb_free(iurl);
    iurl = tb_null;

    // exit stream
    if (stream) tb_stream_exit(stream);
    stream = tb_null;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_coroutine_spider_main(tb_int_t argc, tb_char_t** argv)
{
    // done
    tb_demo_spider_ref_t spider = tb_null;
    do
    {
        // init spider
        spider = tb_demo_spider_init(argv[1], argv[2]);
        tb_assert_and_check_break(spider && spider->scheduler && spider->homepage);

        // send homepage to handler
        tb_co_channel_send(spider->urls, tb_strdup(spider->homepage));

        // grab pages
        if (!tb_coroutine_start(spider->scheduler, tb_demo_spider_page_grab, spider, TB_DEMO_STACKSIZE)) break;
        if (!tb_coroutine_start(spider->scheduler, tb_demo_spider_page_grab, spider, TB_DEMO_STACKSIZE)) break;
        if (!tb_coroutine_start(spider->scheduler, tb_demo_spider_page_grab, spider, TB_DEMO_STACKSIZE)) break;
        if (!tb_coroutine_start(spider->scheduler, tb_demo_spider_page_grab, spider, TB_DEMO_STACKSIZE)) break;
        if (!tb_coroutine_start(spider->scheduler, tb_demo_spider_page_grab, spider, TB_DEMO_STACKSIZE)) break;

        // run scheduler
        tb_co_scheduler_loop(spider->scheduler);

    } while (0);

    // exit spider
    if (spider) tb_demo_spider_exit(spider);
    spider = tb_null;

    // end
    return 0;
}
