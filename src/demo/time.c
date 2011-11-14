#include "tbox.h"

int main(int argc, char** argv)
{
	if (!tb_init(malloc(1024 * 1024), 1024 * 1024)) return 0;

	tb_time_t local;
	tb_time_t utc;
	tb_int64_t time = tb_time();
	if (TB_TRUE == tb_time_to_local(time, &local))
	{
		tb_print("LMT(%s): %04d-%02d-%02d %02d:%02d:%02d:%03d"
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
		tb_print("GMT(%s): %04d-%02d-%02d %02d:%02d:%02d:%03d"
			, tb_time_week(&utc)
			, utc.year
			, utc.month
			, utc.day
			, utc.hours
			, utc.minutes
			, utc.seconds
			, utc.milliseconds);
	}
	tb_print("time: %lld ms, clock: %lld ms", time, tb_mclock());

	if (TB_TRUE == tb_time_from_local(&time, &local))
	{
		tb_print("local => time: %lld", time);
	}
	if (TB_TRUE == tb_time_from_utc(&time, &utc))
	{
		tb_print("utc => time: %lld", time);
	}

	
	return 0;
}
