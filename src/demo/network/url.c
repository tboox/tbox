/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_url_test_set(tb_char_t const* url)
{
	tb_url_t u;
	tb_url_init(&u);

	if (tb_url_set(&u, url))
	{
		static tb_char_t const* poto[] = 
		{
			tb_null
		, 	"data"
		, 	"file"
		, 	"sock"
		, 	"http"
		, 	"rtsp"
		};

		tb_trace_i("=================================================");
		tb_trace_i("irl: %s", url);
		tb_trace_i("orl: %s", tb_url_get(&u));
		tb_trace_i("poto: %s", poto[tb_url_poto_get(&u)]);
		tb_trace_i("port: %u", tb_url_port_get(&u));
		tb_trace_i("host: %s", tb_url_host_get(&u));
		tb_trace_i("path: %s", tb_url_path_get(&u));
		tb_trace_i("args: %s", tb_url_args_get(&u));
	}
	else tb_trace_i("invalid url: %s", url);

	tb_url_exit(&u);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_network_url_main(tb_int_t argc, tb_char_t** argv)
{	
	tb_url_test_set("data://VGhlIFRyZWFzdXJlIEJveCBMaWJyYXJ5Cgo=");
	tb_url_test_set("file:///home/ruki/file.txt");
	tb_url_test_set("sock://localhost:8080");
	tb_url_test_set("sock://localhost:8080/ruki/file.txt");
	tb_url_test_set("rtsp://localhost:8080");
	tb_url_test_set("rtsp://localhost:8080/ruki/file.txt");
	tb_url_test_set("http://localhost");
	tb_url_test_set("http://localhost:80");
	tb_url_test_set("http://localhost:8080");
	tb_url_test_set("http://localhost/ruki/file.txt");
	tb_url_test_set("http://localhost:8080/ruki/file.txt");

	tb_url_test_set("files:///home/ruki/file.txt");
	tb_url_test_set("socks://localhost:8080");
	tb_url_test_set("socks://localhost:8080/ruki/file.txt");
	tb_url_test_set("https://localhost");
	tb_url_test_set("https://localhost:80");
	tb_url_test_set("https://localhost:8080");
	tb_url_test_set("https://localhost/ruki/file.txt");
	tb_url_test_set("https://localhost:443/ruki/file.txt");
	tb_url_test_set("https://localhost:8080/ruki/file.txt");

	tb_url_test_set("file:///home/ruki/file.txt?");
	tb_url_test_set("sock://localhost:8080?udp=");
	tb_url_test_set("sock://localhost:8080/ruki/file.txt?proto=tcp");
	tb_url_test_set("http://localhost?=");
	tb_url_test_set("http://127.0.0.1:8080?ssl=true");
	tb_url_test_set("http://localhost/ruki/file.txt?arg0=11111&arg1=222222");
	tb_url_test_set("http://localhost:8080/ruki/file.txt?xxxx&sds");

	tb_url_test_set("FILE:///HOME/RUKI/FILE.TXT?");
	tb_url_test_set("SOCK://LOCALHOST:8080?UDP=");
	tb_url_test_set("SOCK://LOCALHOST:8080/RUKI/FILE.TXT?PROTO=TCP");
	tb_url_test_set("HTTP://LOCALHOST?=");
	tb_url_test_set("HTTP://127.0.0.1:8080?SSL=TRUE");
	tb_url_test_set("HTTP://LOCALHOST/RUKI/FILE.TXT?ARG0=11111&ARG1=222222");
	tb_url_test_set("HTTP://LOCALHOST:8080/RUKI/FILE.TXT?XXXX&SDS");

	tb_url_test_set("C:/HOME/RUKI/FILE.TXT");
	tb_url_test_set("d:/home/ruki/file.txt");
	tb_url_test_set("d:\\home\\ruki\\file.txt");

	tb_url_test_set("/home/ruki/file.txt");
	tb_url_test_set("ruki/file.txt");
	tb_url_test_set("./ruki/file.txt");
	tb_url_test_set("../ruki/file.txt");
	tb_url_test_set("../../ruki/file.txt");
	tb_url_test_set("..\\..\\ruki\\file.txt");
	tb_url_test_set("../../ruki/../file.txt");

	return 0;
}
