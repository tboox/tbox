#include "tbox.h"
#include <stdio.h>

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init cookies
	tb_cookies_t* cookies = tb_cookies_init();
	tb_assert_and_check_return_val(cookies, 0);

	// set cookies
	tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_FALSE, "BAIDUID=0; max-age=946080000; expires=Sat, 06-Apr-41 03:21:53 GMT; domain=.baidu.com; path=/");
	tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_TRUE, "FG=1; max-age=946080000; expires=Sat, 06-Apr-41 03:21:53 GMT; domain=.baidu.com; path=/; version=1");
	tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_FALSE, "BAIDUID=2; max-age=946080000; expires=Sat, 06-Apr-41 03:21:53 GMT; domain=.baidu.com; path=/style; version=1");
	tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_FALSE, "BG=3; expires=Wednesday, 09-Nov-99 23:12:40 GMT; domain=.space.baidu.com; path=/");
	tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_FALSE, "BAIDSID=4; expires=Wednesday, 09-Nov-99 23:12:40 GMT; domain=.baidu.com; path=/style/foo");
	tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_FALSE, "pvid=3317093836; path=/; domain=qq.com; expires=Sun, 18 Jan 2038 00:00:00 GMT;");
	tb_cookies_set(cookies, TB_NULL, TB_NULL, TB_FALSE, "clientuin=; EXPIRES=Fri, 02-Jan-1970 00:00:00 GMT; PATH=/; DOMAIN=qq.com;");
	tb_cookies_set_from_url(cookies, "http://mail.163.com:2000/WorldClient.dll?View=Main", "User=wangrq; Expires=Tue, 19-Apr-2012 07:10:56 GMT; path=/");
	tb_cookies_set_from_url(cookies, "http://mail.163.com:2000/WorldClient.dll?View=Main", "Session=Yz9eJRh6QijR; path=/");
	tb_cookies_set_from_url(cookies, "http://mail.163.com:2000/WorldClient.dll?View=Main", "Lang=zh; Expires=Tue, 19-Apr-2012 07:10:56 GMT; path=/");
	tb_cookies_set_from_url(cookies, "http://mail.163.com:2000/WorldClient.dll?View=Main", "Theme=Standard; Expires=Tue, 19-Apr-2012 07:10:56 GMT; path=/");
	
	// get cookies
	tb_print("%s", tb_cookies_get_from_url(cookies, "http://www.space.baidu.com/style/foo/login.css"));
	tb_print("%s", tb_cookies_get_from_url(cookies, "http://www.baidu.com/style/foo/login.css"));
	tb_print("%s", tb_cookies_get_from_url(cookies, "http://www.baidu.com/style"));
	tb_print("%s", tb_cookies_get_from_url(cookies, "http://www.baidu.com"));
	tb_print("%s", tb_cookies_get_from_url(cookies, "https://www.baidu.com/"));
	tb_print("%s", tb_cookies_get_from_url(cookies, "http://pingfore.qq.com/pingd?dm=qzone.qq.com&url=login/qzone_n&tt=-&rdm=-&rurl=-&pvid=7466815060&scr=-&scl=-&lang=-&java=1&cc=-&pf=-&tz=-8&ct=-&vs=3.3&emu=0.20486706611700356"));
	tb_print("%s", tb_cookies_get_from_url(cookies, "http://mail.163.com:2000/?Session=LZBMQVW&View=Menu"));

#ifdef TB_DEBUG
	// dump
	tb_cookies_dump(cookies);
#endif

	getchar();

	// exit cookies
	tb_cookies_exit(cookies);

	return 0;
}
