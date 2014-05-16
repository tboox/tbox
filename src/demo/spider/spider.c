/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */ 

// the spider url maxn
#define TB_DEMO_SPIDER_URL_MAXN 		(4096)

// the spider task maxn
#define TB_DEMO_SPIDER_TASK_MAXN 		(256)

// the spider filter maxn
#define TB_DEMO_SPIDER_FILTER_MAXN 		(100000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */ 

// the demo spider type
typedef struct __tb_demo_spider_t
{
	// the pool
	tb_handle_t 				pool;

	// the root
	tb_char_t 					root[256];

	// the filter
	tb_bloom_filter_t* 			filter;

}tb_demo_spider_t;

// the demo spider task type
typedef struct __tb_demo_spider_task_t
{
	// the pool
	tb_demo_spider_t* 			spider;

	// the iurl
	tb_char_t 					iurl[TB_DEMO_SPIDER_URL_MAXN];

	// the ourl
	tb_char_t 					ourl[TB_DEMO_SPIDER_URL_MAXN];

}tb_demo_spider_task_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * declaration
 */ 
static tb_void_t tb_demo_spider_task_exit(tb_demo_spider_task_t* task);
static tb_bool_t tb_demo_spider_task_done(tb_demo_spider_t* spider, tb_char_t const* url);

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */ 
static tb_basic_stream_t* tb_demo_spider_parser_open_html(tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(url, tb_null);

	// done
	tb_bool_t 			ok = tb_false;
	tb_basic_stream_t* 	stream = tb_null;
	do
	{
		// init stream
		stream = tb_basic_stream_init_from_url(url);
		tb_assert_and_check_break(stream);

		// open stream
		if (!tb_basic_stream_open(stream)) break;

		// the url length
		tb_size_t length = tb_strlen(url);
		tb_assert_and_check_break(length);

		// has ".html" suffix?
		if (length > 5 && !tb_stricmp(url + length - 5, ".html")) 
		{
			// ok
			ok  = tb_true;
			break;
		}

		// has ".htm" suffix?
		if (length > 4 && !tb_stricmp(url + length - 4, ".htm")) 
		{
			// ok
			ok  = tb_true;
			break;
		}

		// has ".shtml" suffix?
		if (length > 6 && !tb_stricmp(url + length - 6, ".shtml")) 
		{
			// ok
			ok  = tb_true;
			break;
		}

		// the stream size
		tb_hong_t size = tb_stream_size(stream);

		// prefetch some data
		tb_byte_t* 	data = tb_null;
		tb_size_t 	need = tb_min((tb_size_t)size, 4095);
		if (!tb_basic_stream_need(stream, &data, need)) break;

		// is html?
		ok = tb_strnistr((tb_char_t const*)data, need, "<html>")? tb_true : tb_false;

	} while (0);

	// failed?
	if (!ok) 
	{
		// exit stream
		if (stream) tb_basic_stream_exit(stream);
		stream = tb_null;
	}

	// ok?
	return stream;
}
static tb_size_t tb_demo_spider_parser_get_url(tb_handle_t reader, tb_char_t* data, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(reader && data && maxn, tb_false);

	// walk
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
				if ( 	!tb_stricmp(name, "a")
					|| 	!tb_stricmp(name, "link")
					|| 	!tb_stricmp(name, "img")
					|| 	!tb_stricmp(name, "frame")
					|| 	!tb_stricmp(name, "source"))
				{
					// walk attributes
					tb_xml_node_t* attr = (tb_xml_node_t*)tb_xml_reader_attributes(reader);	
					for (; attr; attr = attr->next)
					{
						// href or src?
						if ( 	tb_scoped_string_size(&attr->data)
							&& 	( 	!tb_scoped_string_cstricmp(&attr->name, "href")
								|| 	!tb_scoped_string_cstricmp(&attr->name, "src"))
							&& 	( 	!tb_scoped_string_cstrnicmp(&attr->data, "http://", 7)
								|| 	!tb_scoped_string_cstrnicmp(&attr->data, "https://", 8)))
						{
							// copy
							tb_strlcpy(data, tb_scoped_string_cstr(&attr->data), maxn);

							// ok
							ok = tb_scoped_string_size(&attr->data);
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
static tb_void_t tb_demo_spider_parser_done(tb_cpointer_t priv)
{
	// check
	tb_demo_spider_task_t* task = (tb_demo_spider_task_t*)priv;
	tb_assert_and_check_return(task);

	// init stream
	tb_basic_stream_t* stream = tb_demo_spider_parser_open_html(task->ourl);
	if (stream)
	{
		// init reader
		tb_handle_t reader = tb_xml_reader_init(stream);
		if (reader)
		{
			// parse url
			tb_char_t data[TB_DEMO_SPIDER_URL_MAXN] = {0};
			while (tb_demo_spider_parser_get_url(reader, data, sizeof(data) - 1))
			{
				// trace
				tb_trace_d("parser: done: %s => %s", task->iurl, data);

				// done
				tb_demo_spider_task_done(task->spider, data);
			}

			// exit reader
			tb_xml_reader_exit(reader);
		}

		// exit stream
		tb_basic_stream_exit(stream);
	}
}
static tb_void_t tb_demo_spider_parser_exit(tb_cpointer_t priv)
{
	// check
	tb_demo_spider_task_t* task = (tb_demo_spider_task_t*)priv;
	tb_assert_and_check_return(task);

	// exit task
	tb_demo_spider_task_exit(task);
}
static tb_bool_t tb_demo_spider_make_ourl(tb_demo_spider_t* spider, tb_char_t const* url, tb_char_t* data, tb_size_t maxn)
{
	// check
	tb_assert_and_check_return_val(spider && url && data && maxn, tb_false);

	// skip protocol
	tb_char_t const* p = url;
	if (!tb_strnicmp(p, "http://", 7)) p += 7;
	else if (!tb_strnicmp(p, "https://", 8)) p += 8;

	// format ourl
	tb_long_t n = tb_snprintf(data, maxn, "%s/%s", spider->root, p);
	if (n >= 0) data[n] = '\0';

	// trace
	tb_trace_d("make: %s => %s", url, data);

	// ok?
	return n > 0? tb_true : tb_false;
}
static tb_void_t tb_demo_spider_task_exit(tb_demo_spider_task_t* task)
{
	// check
	tb_assert_and_check_return(task && task->spider && task->spider->pool);

	// trace
	tb_trace_d("task: exit: %s", task->iurl);

	// exit task
	tb_fixed_pool_free(task->spider->pool, task);
}
static tb_bool_t tb_demo_spider_task_save(tb_size_t state, tb_hize_t offset, tb_hong_t size, tb_hize_t save, tb_size_t rate, tb_pointer_t priv)
{
	// check
	tb_demo_spider_task_t* task = (tb_demo_spider_task_t*)priv;
	tb_assert_and_check_return_val(task, tb_false);

	// percent
	tb_size_t percent = 0;
	if (size > 0) percent = (offset * 100) / size;
	else if (state == TB_STATE_OK) percent = 100;

	// trace
	tb_trace_d("save[%s]: %llu, rate: %lu bytes/s, percent: %lu%%, state: %s", task->iurl, save, rate, percent, tb_state_cstr(state));

	// ok? continue it
	tb_bool_t ok = tb_false;
	if (state == TB_STATE_OK) ok = tb_true;
	// closed?
	else if (state == TB_STATE_CLOSED)
	{
		// trace
		tb_trace_d("task: done: %s: ok", task->iurl);

		// post parser task
		tb_thread_pool_task_post(tb_thread_pool(), "parser_task", tb_demo_spider_parser_done, tb_demo_spider_parser_exit, task, tb_false);
	}
	// failed or killed?
	else 
	{
		// trace
		tb_trace_e("task: done: %s: no", task->iurl);

		// exit task
		tb_demo_spider_task_exit(task);
	}

	// break or continue?
	return ok;
}
static tb_bool_t tb_demo_spider_task_done(tb_demo_spider_t* spider, tb_char_t const* url)
{
	// check
	tb_assert_and_check_return_val(spider && spider->filter && spider->pool && url, tb_false);

	// done
	tb_bool_t 				ok = tb_false;
	tb_demo_spider_task_t* 	task = tb_null;
	do
	{
		// have been done already?
		if (!tb_bloom_filter_set(spider->filter, url)) 
		{
			// trace
			tb_trace_d("task: done: %s: repeat", url);
			ok = tb_true;
			break;
		}

		// trace
		tb_trace_d("task: done: %s: ..", url);

		// make task
		task = (tb_demo_spider_task_t*)tb_fixed_pool_malloc0(spider->pool);
		tb_assert_and_check_break(task);

		// init task
		task->spider = spider;
		tb_strlcpy(task->iurl, url, sizeof(task->iurl) - 1);
		if (!tb_demo_spider_make_ourl(spider, url, task->ourl, sizeof(task->ourl) - 1)) break;

		// done task
		if (!tb_transfer_pool_done(tb_transfer_pool(), url, task->ourl, 0, tb_demo_spider_task_save, task)) break;

		// ok 
		ok = tb_true;

	} while (0);

	// failed?
	if (!ok)
	{
		// trace
		tb_trace_e("task: done: %s: no", url);

		// exit task
		if (task) tb_demo_spider_task_exit(task);
		task = tb_null;
	}

	// ok?
	return ok;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_spider_main(tb_int_t argc, tb_char_t** argv)
{
	// done
	tb_demo_spider_t spider = {0};
	do
	{
		// init home
		tb_char_t const* home = argv[1]? argv[1] : tb_null;
		tb_trace_d("home: %s", home);

		// init root
		if (argv[2]) tb_strlcpy(spider.root, argv[2], sizeof(spider.root) - 1);
		else tb_directory_temp(spider.root, sizeof(spider.root) - 1);
		tb_trace_d("root: %s", spider.root);

		// strip root tail: '/' or '\\'
		tb_size_t size = tb_strlen(spider.root);
		if (size && (spider.root[size - 1] == '/' || spider.root[size - 1] == '\\')) spider.root[size - 1] = '\0';

		// init pool
		spider.pool = tb_fixed_pool_init(TB_DEMO_SPIDER_TASK_MAXN, sizeof(tb_demo_spider_task_t), 0);
		tb_assert_and_check_break(spider.pool);

		// init filter
		spider.filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_01, 3, TB_DEMO_SPIDER_FILTER_MAXN, tb_item_func_str(tb_true, tb_null));
		tb_assert_and_check_break(spider.filter);

		// done the home task if exists
		if (home) tb_demo_spider_task_done(&spider, home);

		// wait 
		getchar();

	} while (0);

	// exit filter
	if (spider.filter) tb_bloom_filter_exit(spider.filter);
	spider.filter = tb_null;

	// exit pool
	if (spider.pool) tb_fixed_pool_exit(spider.pool);
	spider.pool = tb_null;

	// end
	return 0;
}
