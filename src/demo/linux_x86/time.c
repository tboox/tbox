#include "tbox.h"

int main(int argc, char** argv)
{
	if (TPLAT_FALSE == tplat_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	tb_time_t local;
	tb_time_t utc;
	tb_int64_t time = tplat_time();
	if (TB_TRUE == tb_time_to_local(time, &local))
	{
		TB_DBG("LMT(%s): %04d-%02d-%02d %02d:%02d:%02d:%03d"
			, tb_time_week(&local)
			, local.year
			, local.month
			, local.day
			, local.hours
			, local.minutes
			, local.seconds
			, local.milliseconds);
	}
	if (TB_TRUE == tb_time_to_utc(time, &utc))
	{
		TB_DBG("GMT(%s): %04d-%02d-%02d %02d:%02d:%02d:%03d"
			, tb_time_week(&utc)
			, utc.year
			, utc.month
			, utc.day
			, utc.hours
			, utc.minutes
			, utc.seconds
			, utc.milliseconds);
	}
	TB_DBG("time: %d ms, clock: %d ms", (tb_int_t)time, (tb_int_t)tplat_clock());

	if (TB_TRUE == tb_time_from_local(&time, &local))
	{
		TB_DBG("local => time: %d", (tb_int_t)time);
	}
	if (TB_TRUE == tb_time_from_utc(&time, &utc))
	{
		TB_DBG("utc => time: %d", (tb_int_t)time);
	}

	
	return 0;
}
