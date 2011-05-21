#include "tbox.h"
#include "stdio.h"

static tb_bool_t http_head_func(tb_char_t const* line, void* priv)
{
	tplat_printf("head: %s\n", line);
	return TB_TRUE;
}

typedef struct __tb_http_ssl_t
{
	SSL_CTX* 	ctx;
	BIO* 		bio;
	SSL* 		ssl;

}tb_http_ssl_t;

static void http_sclose_func(tb_handle_t handle)
{
	TB_ASSERT_RETURN(handle != TB_INVALID_HANDLE);
	tb_http_ssl_t* ssl = (tb_http_ssl_t*)handle;

	if (ssl)
	{
		if (ssl->bio) BIO_free_all(ssl->bio);
		if (ssl->ctx) SSL_CTX_free(ssl->ctx);
		tb_free(ssl);
	}
}

static tb_handle_t http_sopen_func(tb_char_t const* host, tb_size_t port)
{
	TB_ASSERT_RETURN_VAL(host && host[0] != '\0' && port == 443, TB_INVALID_HANDLE);

	tb_http_ssl_t* ssl = (tb_http_ssl_t*)tb_calloc(1, sizeof(tb_http_ssl_t));
	TB_ASSERT_RETURN_VAL(ssl, TB_INVALID_HANDLE);

	tb_int_t ret = 0;
	tb_char_t hostname[1024];

	// meth
	SSL_METHOD const* meth = SSLv23_method();
	TPLAT_ASSERT_GOTO(meth, fail);

	// ctx
	ssl->ctx = SSL_CTX_new(meth);
	TPLAT_ASSERT_GOTO(ssl->ctx, fail);

	// bio
	ssl->bio = BIO_new_ssl_connect(ssl->ctx);
	TPLAT_ASSERT_GOTO(ssl->bio, fail);

	// ssl
	BIO_get_ssl(ssl->bio, &ssl->ssl);
	TPLAT_ASSERT_GOTO(ssl->ssl, fail);

	// format hostname
	ret = snprintf(hostname, 1024, "%s:%d", host, port);
	if (ret >= 0) hostname[ret] = '\0';

	// setting
	SSL_set_mode(ssl->ssl, SSL_MODE_AUTO_RETRY);
	BIO_set_conn_hostname(ssl->bio, hostname);

	// connect 
	if (BIO_do_connect(ssl->bio) <= 0) goto fail;

	// ok
	return (tb_handle_t)ssl;

fail:
	if (ssl) http_sclose_func((tb_handle_t)ssl);
	return TB_INVALID_HANDLE;
}
static tb_int_t http_sread_func(tb_handle_t handle, tb_byte_t* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(handle != TB_INVALID_HANDLE, -1);
	tb_http_ssl_t* ssl = (tb_http_ssl_t*)handle;

	return BIO_read(ssl->bio, data, size);
}
static tb_int_t http_swrite_func(tb_handle_t handle, tb_byte_t const* data, tb_size_t size)
{
	TB_ASSERT_RETURN_VAL(handle != TB_INVALID_HANDLE, -1);
	tb_http_ssl_t* ssl = (tb_http_ssl_t*)handle;

	return BIO_write(ssl->bio, data, size);
}

int main(int argc, char** argv)
{
	// init tplat
	if (TPLAT_FALSE == tplat_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	// init ssl
	SSL_library_init();
	SSL_load_error_strings();
	//ERR_load_BIO_strings();
	//OpenSSL_add_all_algorithms();

	// create http
	tb_handle_t http = tb_http_create(TB_NULL);
	if (http == TB_INVALID_HANDLE) goto end;
	
	// init option
	if (TB_FALSE == tb_http_option_set_url(http, argv[1])) goto end;
	if (TB_FALSE == tb_http_option_set_head_func(http, http_head_func, http)) goto end;
	if (TB_FALSE == tb_http_option_set_sopen_func(http, http_sopen_func)) goto end;
	if (TB_FALSE == tb_http_option_set_sclose_func(http, http_sclose_func)) goto end;
	if (TB_FALSE == tb_http_option_set_sread_func(http, http_sread_func)) goto end;
	if (TB_FALSE == tb_http_option_set_swrite_func(http, http_swrite_func)) goto end;

	// open http
	if (TB_FALSE == tb_http_open(http)) goto end;

	// open file
	tplat_handle_t hfile = tplat_file_open(argv[2], TPLAT_FILE_WO | TPLAT_FILE_CREAT | TPLAT_FILE_TRUNC);
	if (hfile == TPLAT_INVALID_HANDLE) goto end;
	
	// read data
	tb_byte_t 		data[8192];
	tb_size_t 		read = 0;
	tb_size_t 		base = (tb_size_t)tplat_clock();
	tb_size_t 		time = (tb_size_t)tplat_clock();
	tb_size_t 		size = tb_http_status_content_size(http);
	do
	{
		tb_int_t ret = tb_http_read(http, data, 8192);
		//TB_DBG("ret: %d", ret);
		if (ret > 0)
		{
			read += ret;
			time = (tb_size_t)tplat_clock();

#if 1
			tb_int_t write = 0;
			while (write < ret)
			{
				tb_int_t ret2 = tplat_file_write(hfile, data + write, ret - write);
				if (ret2 > 0) write += ret2;
				else if (ret2 < 0) break;
			}
#endif

		}
		else if (!ret) 
		{
			tb_size_t timeout = ((tb_size_t)tplat_clock()) - time;
			if (timeout > 10000) break;
		}
		else break;

		// update info
		if (time > base && ((time - base) % 1000)) 
		{
			tplat_printf("speed: %5d kb/s, load: %8d kb\r", (read / (time - base)), read / 1000);
			fflush(stdout);
		}

		// is end?
		if (size && read >= size) break;

	} while(1);

end:

	// close file
	tplat_file_close(hfile);

	// close it
	tb_http_close(http);

	// destroy it
	if (http != TB_INVALID_HANDLE) tb_http_destroy(http);


	tplat_printf("end\n");
	getchar();

	// exit tplat
	tplat_exit();
	return 0;
}
