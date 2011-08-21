#include "tbox.h"

int test(int argc, char** argv)
{
	__tb_volatile__ int* a = new int[10];
	__tb_volatile__ int* b = new int;
	__tb_volatile__ int* c = new int;
	
	delete[] a;

	tb_mpool_dump();
	return 0;
}

extern "C"
{
	int test2_main(int argc, char** argv)
	{
		return test(argc, argv);
	}
}
