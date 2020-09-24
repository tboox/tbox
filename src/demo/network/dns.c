/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_DNS_TEST_INVALID_HOST        (0)
#define TB_DNS_TEST_MORE_HOST           (0)
#define TB_DNS_TEST_HOST_SOME           (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * test
 */
static tb_void_t tb_dns_test_done(tb_char_t const* name)
{
    tb_ipaddr_t addr;
    tb_hong_t   time = tb_mclock();
    if (tb_dns_looker_done(name, &addr))
    {
        time = tb_mclock() - time;
        tb_trace_i("lookup: %s => %{ipaddr}, %lld ms", name, &addr, time);
    }
    else tb_trace_i("lookup: %s failed", name);
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_network_dns_main(tb_int_t argc, tb_char_t** argv)
{
    // test the invalid host
#if TB_DNS_TEST_INVALID_HOST
    // add not dns host
    tb_dns_server_add("127.0.0.1");

    // add not ipv4 host
    tb_dns_server_add("localhost");

#endif

    // test the more host
#if TB_DNS_TEST_MORE_HOST
    tb_dns_server_add("2001:470:20::2");
    tb_dns_server_add("fec0:0:0:ffff::1%1");
    tb_dns_server_add("205.252.144.228");
    tb_dns_server_add("208.151.69.65");
    tb_dns_server_add("202.181.202.140");
    tb_dns_server_add("202.181.224.2");
    tb_dns_server_add("202.175.3.8");
    tb_dns_server_add("202.175.3.3");
    tb_dns_server_add("168.95.192.1");
    tb_dns_server_add("168.95.1.1");
    tb_dns_server_add("208.67.222.222");
    tb_dns_server_add("205.171.2.65");
    tb_dns_server_add("193.0.14.129");
    tb_dns_server_add("202.12.27.33");
    tb_dns_server_add("202.216.228.18");
    tb_dns_server_add("209.166.160.132");
    tb_dns_server_add("208.96.10.221");
    tb_dns_server_add("61.144.56.101");
    tb_dns_server_add("202.101.98.55");
    tb_dns_server_add("202.96.128.166");
    tb_dns_server_add("202.96.209.134");
    tb_dns_server_add("221.12.65.228");
#endif

    // sort
    tb_dns_server_sort();

    // dump
#ifdef __tb_debug__
    tb_dns_server_dump();
#endif

#if TB_DNS_TEST_HOST_SOME
    tb_hong_t time = tb_mclock();
    tb_dns_test_done("www.tboox.org");
    tb_dns_test_done("www.tboox.net");
    tb_dns_test_done("www.baidu.com");
    tb_dns_test_done("www.google.com");
    tb_dns_test_done("www.google.com.hk");
    tb_dns_test_done("www.csdn.net");
    tb_dns_test_done("www.qq.com");
    tb_dns_test_done("www.youku.com");
    tb_dns_test_done("www.ibm.com");
    tb_dns_test_done("www.sina.com.cn");
    tb_dns_test_done("www.hao123.com");
    tb_dns_test_done("www.sohu.com");
    tb_dns_test_done("www.weibo.com");
    tb_dns_test_done("www.126.com");
    tb_dns_test_done("www.163.com");
    tb_dns_test_done("www.taobao.com");
    tb_dns_test_done("www.microsoft.com");
    tb_dns_test_done("www.qiyi.com");
    tb_dns_test_done("www.xunlei.com");
    tb_dns_test_done("www.360buy.com");
    tb_dns_test_done("www.tudou.com");
    tb_dns_test_done("www.pps.tv");
    tb_dns_test_done("www.yahoo.com");
    tb_dns_test_done("www.zol.com.cn");
    tb_dns_test_done("www.download.com");
    tb_dns_test_done("www.webkit.org");
    tb_dns_test_done("www.douban.com");
    tb_dns_test_done("www.github.com");
    tb_dns_test_done("www.videolan.org");
    tb_dns_test_done("www.net.cn");
    tb_dns_test_done("www.yahoo.com");
    tb_dns_test_done("www.sina.com.cn");
    tb_dns_test_done("www.hao123.com");
    tb_dns_test_done("www.sohu.com");
    tb_dns_test_done("www.weibo.com");
    tb_dns_test_done("www.126.com");
    tb_dns_test_done("www.163.com");
    tb_dns_test_done("www.taobao.com");
    tb_dns_test_done("www.microsoft.com");
    tb_dns_test_done("www.qiyi.com");
    tb_dns_test_done("www.xunlei.com");
    tb_dns_test_done("www.360buy.com");
    tb_dns_test_done("www.tudou.com");
    tb_dns_test_done("www.pps.tv");
    tb_dns_test_done("www.yahoo.com");
    tb_dns_test_done("www.zol.com.cn");
    tb_dns_test_done("www.download.com");
    tb_dns_test_done("www.webkit.org");
    tb_dns_test_done("www.huaxiazi.com");
    tb_dns_test_done("www.facebook.com");
    tb_dns_test_done("www.youtube.com");
    tb_dns_test_done("www.bing.com");
    tb_dns_test_done("www.baidu.com");
    tb_dns_test_done("www.tianya.com");
    tb_dns_test_done("www.adobe.com");
    tb_dns_test_done("web2.qq.com");
    tb_dns_test_done("www.bluehost.com");
    tb_dns_test_done("www.pediy.com");
    tb_dns_test_done("www.wordpress.com");
    tb_dns_test_done("www.gitorious.org");
    tb_dns_test_done("t.qq.com");
    tb_dns_test_done("www.wordpress.com");
    tb_dns_test_done("www.mop.com");
    tb_dns_test_done("www.56.com");
    tb_dns_test_done("www.joy.com.cn");
    tb_dns_test_done("www.xxxxx.com");
    tb_dns_test_done("www.wordpress.com");
    tb_dns_test_done("developer.android.com");
    tb_dns_test_done("developer.apple.com");
    tb_dns_test_done("www.arm.com");
    tb_dns_test_done("www.nginx.com");
    tb_dns_test_done("www.ted.com");
    tb_dns_test_done("www.tboox.org");
    tb_dns_test_done("www.tboox.net");
    tb_dns_test_done("www.baidu.com");
    tb_dns_test_done("www.google.com");
    tb_dns_test_done("www.google.com.hk");
    tb_dns_test_done("www.csdn.net");
    tb_dns_test_done("www.qq.com");
    tb_dns_test_done("www.youku.com");
    tb_dns_test_done("www.ibm.com");
    tb_dns_test_done("www.sina.com.cn");
    tb_dns_test_done("www.hao123.com");
    tb_dns_test_done("www.sohu.com");
    tb_dns_test_done("www.weibo.com");
    tb_dns_test_done("www.126.com");
    tb_dns_test_done("www.163.com");
    tb_dns_test_done("www.taobao.com");
    tb_dns_test_done("www.microsoft.com");
    tb_dns_test_done("www.qiyi.com");
    tb_dns_test_done("www.xunlei.com");
    tb_dns_test_done("www.360buy.com");
    tb_dns_test_done("www.tudou.com");
    tb_dns_test_done("www.pps.tv");
    tb_dns_test_done("www.yahoo.com");
    tb_dns_test_done("www.zol.com.cn");
    tb_dns_test_done("www.download.com");
    tb_dns_test_done("www.webkit.org");
    tb_dns_test_done("www.douban.com");
    tb_dns_test_done("www.github.com");
    tb_dns_test_done("www.videolan.org");
    tb_dns_test_done("www.net.cn");
    tb_dns_test_done("www.yahoo.com");
    tb_dns_test_done("www.sina.com.cn");
    tb_dns_test_done("www.hao123.com");
    tb_dns_test_done("www.sohu.com");
    tb_dns_test_done("www.weibo.com");
    tb_dns_test_done("www.126.com");
    tb_dns_test_done("www.163.com");
    tb_dns_test_done("www.taobao.com");
    tb_dns_test_done("www.microsoft.com");
    tb_dns_test_done("www.qiyi.com");
    tb_dns_test_done("www.xunlei.com");
    tb_dns_test_done("www.360buy.com");
    tb_dns_test_done("www.tudou.com");
    tb_dns_test_done("www.pps.tv");
    tb_dns_test_done("www.yahoo.com");
    tb_dns_test_done("www.zol.com.cn");
    tb_dns_test_done("www.download.com");
    tb_dns_test_done("www.webkit.org");
    tb_dns_test_done("www.huaxiazi.com");
    tb_dns_test_done("www.facebook.com");
    tb_dns_test_done("www.youtube.com");
    tb_dns_test_done("www.bing.com");
    tb_dns_test_done("www.baidu.com");
    tb_dns_test_done("www.tianya.com");
    tb_dns_test_done("www.adobe.com");
    tb_dns_test_done("web2.qq.com");
    tb_dns_test_done("www.bluehost.com");
    tb_dns_test_done("www.pediy.com");
    tb_dns_test_done("www.wordpress.com");
    tb_dns_test_done("www.gitorious.org");
    tb_dns_test_done("t.qq.com");
    tb_dns_test_done("www.wordpress.com");
    tb_dns_test_done("www.mop.com");
    tb_dns_test_done("www.56.com");
    tb_dns_test_done("www.joy.com.cn");
    tb_dns_test_done("www.xxxxx.com");
    tb_dns_test_done("www.wordpress.com");
    tb_dns_test_done("developer.android.com");
    tb_dns_test_done("developer.apple.com");
    tb_dns_test_done("www.arm.com");
    tb_dns_test_done("www.nginx.com");
    tb_dns_test_done("www.ted.com");
    tb_dns_test_done("www.mcu-memory.com");
    tb_dns_test_done("mail.126.com");
    tb_dns_test_done("mail.163.com");
    tb_dns_test_done("mail.google.com");
    tb_dns_test_done("mail.qq.com");
    tb_dns_test_done("mail.sina.com");
    tb_dns_test_done("mail.sohu.com");
    tb_dns_test_done("mail.qq.com");
    tb_dns_test_done("www.renren.com");
    tb_dns_test_done("www.cepark.com");
    tb_dns_test_done("www.ifttt.com");
    tb_dns_test_done("www.china-pub.com");
    tb_dns_test_done("www.amazon.com");
    tb_dns_test_done("www.amazon.cn");
    tb_dns_test_done("www.2688.com");
    tb_dns_test_done("www.mtime.com");
    tb_dns_test_done("hi.baidu.com");
    tb_dns_test_done("repo.or.cz");
    tb_dns_test_done("www.ifeng.com");
    tb_dns_test_done("www.sourceforge.com");
    tb_dns_test_done("www.wikipedia.org");
    tb_dns_test_done("baike.baidu.com");
    tb_dns_test_done("www.ted.com");
    tb_dns_test_done("www.ted.com");
    tb_dns_test_done("www.ted.com");
    time = tb_mclock() - time;
    tb_trace_i("[demo]: done %lld ms", time);
#else
    tb_dns_test_done(argv[1]);
#endif

    return 0;
}
