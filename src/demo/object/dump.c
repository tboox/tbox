/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "../demo.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t tb_demo_object_dump_main(tb_int_t argc, tb_char_t** argv)
{
	// read
	tb_object_t* root = tb_object_read_from_url(argv[1]);

	// writ
	if (root)
	{
		// seek?
		tb_object_t* object = root;
		if (argv[2]) object = tb_object_seek(root, argv[2], TB_OBJECT_TYPE_NONE);

		// dump object
		if (object) tb_object_dump(object);

		// exit object
		tb_object_exit(root);
	}
	
	return 0;
}

