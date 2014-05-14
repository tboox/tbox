/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */
tb_int_t tb_demo_container_bloom_filter_main(tb_int_t argc, tb_char_t** argv)
{
	// the count
//	tb_size_t count = 1000000;
	tb_size_t count = 10000000;

	// init filter
	tb_bloom_filter_t* filter = tb_bloom_filter_init(TB_BLOOM_FILTER_PROBABILITY_0_001, 3, count, tb_item_func_long());
	if (filter)
	{
		// init hash
		tb_hash_t* hash = tb_hash_init(TB_HASH_BULK_SIZE_LARGE, tb_item_func_long(), tb_item_func_true());
		if (hash)
		{
			// clear random
			tb_random_clear(tb_random_generator());

			// done
			tb_size_t i = 0;
			tb_size_t r = 0;
			tb_size_t f = 0;
			for (i = 0; i < count; i++)
			{
				// the value
				tb_long_t value = tb_random();

				// set value to filter
				if (!tb_bloom_filter_set(filter, (tb_cpointer_t)value))
				{
					// repeat++
					r++;
	
					// false?
					if (!tb_hash_get(hash, (tb_cpointer_t)value)) f++;
				}
			
				// set value to hash
				tb_hash_set(hash, (tb_cpointer_t)value, (tb_cpointer_t)tb_true);
			}

			// trace
#ifdef TB_CONFIG_TYPE_FLOAT
			tb_trace_i("count: %lu, false: %lu, repeat: %lu, p: %lf", count, f, r, (tb_double_t)f / count);
#else
			tb_trace_i("count: %lu, false: %lu, repeat: %lu", count, f, r);
#endif

			// dump hash
#ifdef __tb_debug__
//			tb_hash_dump(hash);
#endif

			// exit hash
			tb_hash_exit(hash);
		}

		// exit filter
		tb_bloom_filter_exit(filter);
	}
	return 0;
}
