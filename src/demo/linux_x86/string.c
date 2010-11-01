#include "tplat/tplat.h"
#include "../../tbox.h"

int main(int argc, char** argv)
{
	tplat_size_t regular_block_n[TPLAT_POOL_REGULAR_CHUNCK_MAX_COUNT] = {0, 0, 0, 0, 0, 0, 0};
	tplat_pool_create(TB_CONFIG_MEMORY_POOL_INDEX, malloc(1024 * 1024), 1024 * 1024, regular_block_n);

	tb_string_t s1;
	tb_string_init(&s1);
	tb_string_assign_c_string_by_ref(&s1, "hello");
	tb_string_append_char(&s1, ' ');
	tb_string_append_c_string(&s1, "world");
	tb_string_append_format(&s1, "%s", "...");
	TB_DBG("%s", tb_string_c_string(&s1));

	tb_string_t s2;
	tb_string_init(&s2);
	tb_string_assign_by_ref(&s2, &s1);
	tb_string_append_char(&s2, ' ');
	tb_string_append_c_string(&s2, "world");
	tb_string_assign_c_string_by_ref(&s2, "hello");
	tb_string_append_format(&s2, "%s", "...");
	TB_DBG("%s", tb_string_c_string(&s2));

	tb_stack_string_t s3;
	tb_string_init_stack_string(&s3);
	tb_string_assign_by_ref(&s3, &s1);
	tb_string_append_char(&s3, ' ');
	tb_string_append_c_string(&s3, "world");
	tb_string_assign_c_string_by_ref(&s3, "hello");
	tb_string_append_format(&s3, "%s", "...");
	TB_DBG("%s", tb_string_c_string(&s3));

	tb_string_uninit(&s1);
	tb_string_uninit(&s2);
	tb_string_uninit(&s3);

	TPLAT_POOL_DUMP(TB_CONFIG_MEMORY_POOL_INDEX);
	return 0;
}
