#include "tplat/tplat.h"
#include "../../tbox.h"

#define CONV_TEST_ISHAS 		(1)

static void tb_conv_check_ishas()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if ((TB_CONV_ISSPACE(i)? 1 : 0) != (isspace(i)? 1 : 0)) tplat_printf("[e] isspace: 0x%02x\n", i);
		if ((TB_CONV_ISALPHA(i)? 1 : 0) != (isalpha(i)? 1 : 0)) tplat_printf("[e] isalpha: 0x%02x\n", i);
		if ((TB_CONV_ISDIGIT(i)? 1 : 0) != (isdigit(i)? 1 : 0)) tplat_printf("[e] isdigit: 0x%02x\n", i);
		if ((TB_CONV_ISUPPER(i)? 1 : 0) != (isupper(i)? 1 : 0)) tplat_printf("[e] isupper: 0x%02x\n", i);
		if ((TB_CONV_ISLOWER(i)? 1 : 0) != (islower(i)? 1 : 0)) tplat_printf("[e] islower: 0x%02x\n", i);
		if ((TB_CONV_ISASCII(i)? 1 : 0) != (isascii(i)? 1 : 0)) tplat_printf("[e] isascii: 0x%02x\n", i);
	}
}
static void tb_conv_make_space_table()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if (isspace(i)) tplat_printf("0x%02x\n", i);
	}
}
static void tb_conv_make_isalpha_table()
{
	tb_int_t i = 0;
	for (i = 0; i < 256; i++)
	{
		if (isalpha(i)) tplat_printf("0x%02x\n", i);
	}
}

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {10, 10, 10, 10, 10, 10, 10};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	//tb_conv_make_isspace_table();
	//tb_conv_make_isalpha_table();

#if CONV_TEST_ISHAS
	tb_conv_check_ishas();
#endif

	return 0;
}
