#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_cookies_main(tb_int_t argc, tb_char_t** argv)
{
    // set cookies
    tb_cookies_set(tb_cookies(), "www.baidu.com", "/", tb_false, "HttpOnly;Secure");
    tb_cookies_set(tb_cookies(), tb_null, tb_null, tb_false, "BAIDUID=0; max-age=946080000; expires=Sat, 06-Apr-41 03:21:53 GMT; domain=.baidu.com; path=/");
    tb_cookies_set(tb_cookies(), tb_null, tb_null, tb_true, "FG=1; max-age=946080000; expires=Sat, 06-Apr-2017 03:21:53 GMT; domain=.baidu.com; path=/; version=1");
    tb_cookies_set(tb_cookies(), tb_null, tb_null, tb_false, "BAIDUID=2; max-age=946080000; expires=Sat, 06-Apr-2016 03:21:53 GMT; domain=.baidu.com; path=/style; version=1");
    tb_cookies_set(tb_cookies(), tb_null, tb_null, tb_false, "BG=3; expires=Wednesday, 09-Nov-99 23:12:40 GMT; domain=.space.baidu.com; path=/");
    tb_cookies_set(tb_cookies(), tb_null, tb_null, tb_false, "BAIDSID=4; expires=Wednesday, 09-Nov-99 23:12:40 GMT; domain=.baidu.com; path=/style/foo");
    tb_cookies_set(tb_cookies(), tb_null, tb_null, tb_false, "pvid=3317093836; path=/; domain=qq.com; expires=Sun, 18 Jan 2038 00:00:00 GMT;");
    tb_cookies_set(tb_cookies(), tb_null, tb_null, tb_false, "clientuin=; EXPIRES=Fri, 02-Jan-1970 00:00:00 GMT; PATH=/; DOMAIN=qq.com;");
    tb_cookies_set_from_url(tb_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main", "User=wangrq; Expires=Tue, 19-Apr-2015 07:10:56 GMT; path=/");
    tb_cookies_set_from_url(tb_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main", "Session=Yz9eJRh6QijR; path=/");
    tb_cookies_set_from_url(tb_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main", "Lang=zh; Expires=Tue, 19-Apr-2015 07:10:56 GMT; path=/");
    tb_cookies_set_from_url(tb_cookies(), "http://mail.163.com:2000/WorldClient.dll?View=Main", "Theme=Standard; Expires=Tue, 19-Apr-2015 07:10:56 GMT; path=/");

    // get cookies
    tb_string_t value;
    tb_string_init(&value);
    tb_trace_i("%s", tb_cookies_get_from_url(tb_cookies(), "http://www.space.baidu.com/style/foo/login.css", &value));
    tb_trace_i("%s", tb_cookies_get_from_url(tb_cookies(), "http://www.baidu.com/style/foo/login.css", &value));
    tb_trace_i("%s", tb_cookies_get_from_url(tb_cookies(), "http://www.baidu.com/style", &value));
    tb_trace_i("%s", tb_cookies_get_from_url(tb_cookies(), "http://www.baidu.com", &value));
    tb_trace_i("%s", tb_cookies_get_from_url(tb_cookies(), "https://www.baidu.com/", &value));
    tb_trace_i("%s", tb_cookies_get_from_url(tb_cookies(), "http://pingfore.qq.com/pingd?dm=qzone.qq.com&url=login/qzone_n&tt=-&rdm=-&rurl=-&pvid=7466815060&scr=-&scl=-&lang=-&java=1&cc=-&pf=-&tz=-8&ct=-&vs=3.3&emu=0.20486706611700356", &value));
    tb_trace_i("%s", tb_cookies_get_from_url(tb_cookies(), "http://mail.163.com:2000/?Session=LZBMQVW&View=Menu", &value));
    tb_string_exit(&value);

    return 0;
}
