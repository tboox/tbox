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
    if (tb_url_cstr_set(&u, url))
    {
        tb_trace_i("=================================================");
        tb_trace_i("irl: %s",   url);
        tb_trace_i("orl: %s",   tb_url_cstr(&u));
        tb_trace_i("poto: %s",  tb_url_protocol_cstr(&u));
        tb_trace_i("port: %u",  tb_url_port(&u));
        tb_trace_i("host: %s",  tb_url_host(&u));
        tb_trace_i("path: %s",  tb_url_path(&u));
        tb_trace_i("args: %s",  tb_url_args(&u));
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
    tb_url_test_set("http://[::1]:8080/ruki/file.txt");
    tb_url_test_set("http://[fe80::7a31:c1ff:fecf:e8ae%en0]:8080/ruki/file.txt");
    tb_url_test_set("http://[fe80::7a31:c1ff:fecf:e8ae%7]:8080/ruki/file.txt");

    tb_url_test_set("socks://localhost:8080");
    tb_url_test_set("socks://localhost:8080/ruki/file.txt");
    tb_url_test_set("https://localhost");
    tb_url_test_set("https://localhost:80");
    tb_url_test_set("https://localhost:8080");
    tb_url_test_set("https://localhost/ruki/file.txt");
    tb_url_test_set("https://localhost:443/ruki/file.txt");
    tb_url_test_set("https://localhost:8080/ruki/file.txt");
    tb_url_test_set("https://localhost:8080/ruki/file.txt/#/hello");
    tb_url_test_set("https://github.githubassets.com/#start-of-content");

    tb_url_test_set("file:///home/ruki/file.txt?");
    tb_url_test_set("sock://localhost:8080?udp=");
    tb_url_test_set("sock://localhost:8080/ruki/file.txt?proto=tcp");
    tb_url_test_set("http://localhost?=");
    tb_url_test_set("http://127.0.0.1:8080?ssl=true");
    tb_url_test_set("http://localhost/ruki/file.txt?arg0=11111&arg1=222222");
    tb_url_test_set("http://localhost:8080/ruki/file.txt?xxxx&sds");
    tb_url_test_set("http://vcode.baidu.com/genimage?00140254475301668D95A071A0C29E7F10FD51CA7F1548CF49B02D47F821146A1F7A4154B649D30D2E4FE5B24AD645EAA0D957938A0E838D854084A77B3589F7E0E6784E9919C2178A5978F8CA51E878E1DB3B84C7D2BE5E4BC145811FEC7DBE6E17DB25931A00F0A31E45A7C476FF8209BD9DDD6824D89D8E0EC7260FE53F4B412FC67D161DE6FEFC3F884E079170A1A8085629B84AED10352DCAA946D80EDCBB04A664936CAB0DDC585110D11D2747C27260EF6CD44648");
    tb_url_test_set("http://中文.中文");

    tb_url_test_set("FILE:///HOME/RUKI/FILE.TXT?");
    tb_url_test_set("SOCK://LOCALHOST:8080?UDP=");
    tb_url_test_set("SOCK://LOCALHOST:8080/RUKI/FILE.TXT?PROTO=TCP");
    tb_url_test_set("HTTP://LOCALHOST?=");
    tb_url_test_set("HTTP://127.0.0.1:8080?SSL=TRUE");
    tb_url_test_set("HTTP://LOCALHOST/RUKI/FILE.TXT?ARG0=11111&ARG1=222222");
    tb_url_test_set("HTTP://LOCALHOST:8080/RUKI/FILE.TXT?XXXX&SDS");

    tb_url_test_set("sql://localhost/?type=mysql&user=xxxx&pwd=xxxx");
    tb_url_test_set("sql://localhost:3306/?type=mysql&user=xxxx&pwd=xxxx&database=xxxx");
    tb_url_test_set("Sql:///home/file.sqlite3?type=sqlite3");

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
