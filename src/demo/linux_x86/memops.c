#include "stdio.h"
#include "../../tbox.h"
#include "tplat/tplat.h"

static tb_bool_t check_memset_u16(tb_byte_t* dst, tb_uint16_t src, tb_size_t size)
{
	tb_uint16_t* p = (tb_uint16_t*)dst;
	tb_uint16_t* e = p + size;
	while (p < e)
	{
		if (*p != src) 
		{
			TB_DBG("%d %x", ((tb_byte_t*)p - dst) >> 1, *p);
			return TB_FALSE;
		}
		p++;
	}

	return TB_TRUE;
}
static tb_bool_t check_memset_u24(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	tb_byte_t* p = (tb_byte_t*)dst;
	tb_byte_t* e = p + (size * 3);
	while (p < e)
	{
		if ((*((tb_uint32_t*)p) & 0xffffff) != (src & 0xffffff)) return TB_FALSE;
		p += 3;
	}

	return TB_TRUE;
}
static tb_bool_t check_memset_u32(tb_byte_t* dst, tb_uint32_t src, tb_size_t size)
{
	tb_uint32_t* p = (tb_uint32_t*)dst;
	tb_uint32_t* e = p + size;
	while (p < e)
	{
		if (*p != src) return TB_FALSE;
		p++;
	}

	return TB_TRUE;
}
int main(int argc, char** argv)
{
	tb_size_t i = 0;
	tplat_int64_t dt = 0;

	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(20 * 1024 * 1024), 20 * 1024 * 1024, regular_block_n);
	
	tb_size_t 	size = 15 * 1024 * 1024;
	tb_byte_t* 	data = tb_malloc(size);

	// test: u16 x 1024
	memset(data, 0, size);

	dt = tplat_clock();
	for (i = 0; i < 1000000; i++) tb_memset_u16(data, 0xbeef, 1024);
	TB_ASSERT(TB_TRUE == check_memset_u16(data, 0xbeef, 1024));
	dt = tplat_clock() - dt;
	TB_DBG("u16 x 1k: %d ms", (tb_int_t)(dt / 1000));

	// test: u16 x 1024 * 1024
	memset(data, 0, size);

	dt = tplat_clock();
	for (i = 0; i < 1000; i++) tb_memset_u16(data, 0xbeef, 1024 * 1024);
	TB_ASSERT(TB_TRUE == check_memset_u16(data, 0xbeef, 1024 * 1024));
	dt = tplat_clock() - dt;
	TB_DBG("u16 x 1m: %d ms", (tb_int_t)(dt / 1000));

	// test: u24 x 1024
	memset(data, 0, size);

	dt = tplat_clock();
	for (i = 0; i < 1000000; i++) tb_memset_u24(data, 0xbeefaa, 1024);
	TB_ASSERT(TB_TRUE == check_memset_u24(data, 0xbeefaa, 1024));
	dt = tplat_clock() - dt;
	TB_DBG("u24 x 1k: %d ms", (tb_int_t)(dt / 1000));

	// test: u24 x 1024 * 1024
	memset(data, 0, size);

	dt = tplat_clock();
	for (i = 0; i < 1000; i++) tb_memset_u24(data, 0xbeefaa, 1024 * 1024);
	TB_ASSERT(TB_TRUE == check_memset_u24(data, 0xbeefaa, 1024 * 1024));
	dt = tplat_clock() - dt;
	TB_DBG("u24 x 1m: %d ms", (tb_int_t)(dt / 1000));

	// test: u32 x 1024
	memset(data, 0, size);

	dt = tplat_clock();
	for (i = 0; i < 1000000; i++) tb_memset_u32(data, 0xbeefbeaf, 1024);
	TB_ASSERT(TB_TRUE == check_memset_u32(data, 0xbeefbeaf, 1024));
	dt = tplat_clock() - dt;
	TB_DBG("u32 x 1k: %d ms", (tb_int_t)(dt / 1000));

	// test: u32 x 1024 * 1024
	memset(data, 0, size);

	dt = tplat_clock();
	for (i = 0; i < 1000; i++) tb_memset_u32(data, 0xbeefbeaf, 1024 * 1024);
	TB_ASSERT(TB_TRUE == check_memset_u32(data, 0xbeefbeaf, 1024 * 1024));
	dt = tplat_clock() - dt;
	TB_DBG("u32 x 1m: %d ms", (tb_int_t)(dt / 1000));

	return 0;
}
