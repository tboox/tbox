/* ///////////////////////////////////////////////////////////////////////
 * includes
 */ 
#include "tbox.h"
#include <stdlib.h>

/* ///////////////////////////////////////////////////////////////////////
 * main
 */ 
tb_int_t main(tb_int_t argc, tb_char_t** argv)
{
	// init tbox
	if (!tb_init(malloc(10 * 1024 * 1024), 10 * 1024 * 1024)) return 0;

	// init stream
	tb_gstream_t* ist = tb_gstream_init_from_url(argv[1]);
	tb_gstream_t* ost = tb_gstream_init_from_url(argv[2]);
	if (ost) tb_gstream_ctrl(ost, TB_FSTREAM_CMD_SET_FLAGS, TB_FILE_WO | TB_FILE_CREAT | TB_FILE_TRUNC);
	if (ist && ost && tb_gstream_bopen(ist) && tb_gstream_bopen(ost))
	{
		// init reader & writer
		tb_handle_t reader = tb_xml_reader_init(ist);
		tb_handle_t writer = tb_xml_writer_init(ost, tb_false);
		if (reader && writer)
		{
#if 0
			// goto
			tb_bool_t ok = tb_true;
			if (argv[3]) ok = tb_xml_reader_goto(reader, argv[3]);

			// load & save;
			if (ok) tb_xml_writer_save(writer, tb_xml_reader_load(reader));
#else
			tb_xml_node_t* root = tb_xml_reader_load(reader);

			// goto
			if (argv[3]) root = tb_xml_node_goto(root, argv[3]);

			// save
			if (root) 
			{
				// trace
				tb_print("csize: %u", tb_xml_node_csize(root));
				tb_print("asize: %u", tb_xml_node_asize(root));

				// remove
				{
					tb_xml_node_t* dict = tb_xml_node_goto(root, "/plist/dict");

#if 1
					tb_xml_node_t* 	prev = tb_null;
					tb_xml_node_t* 	node = tb_xml_node_chead(dict);
					tb_bool_t 		rval = tb_false;
					while (node)
					{
						if (node->type == TB_XML_NODE_TYPE_ELEMENT)
						{
							if (!rval && !tb_pstring_cstricmp(&node->name, "key"))
							{
								tb_xml_node_t* value = tb_xml_node_chead(node);
								if (value && value->type == TB_XML_NODE_TYPE_TEXT)
								{
									tb_print("<key>%s</key>", tb_pstring_cstr(&value->data));
									if ( 	!tb_pstring_cstricmp(&value->data, "accountInfo")
										|| 	!tb_pstring_cstricmp(&value->data, "appleId")
										|| 	!tb_pstring_cstricmp(&value->data, "purchaseDate"))
									{
										// remove key
										if (prev) 
										{
											tb_xml_node_remove_next(prev);
											node = prev->next;
										}
										else 
										{
											tb_xml_node_remove_chead(dict);
											node = dict->chead;
										}
										
										// remove value next
										rval = tb_true;
										continue ;
									}
								}
							}
							else if (rval)
							{
								// remove value
								if (prev) 
								{
									tb_xml_node_remove_next(prev);
									node = prev->next;
								}
								else 
								{
									tb_xml_node_remove_chead(dict);
									node = dict->chead;
								}
								
								// remove key next
								rval = tb_false;
								continue ;
							}
						}

						// next
						prev = node;
						node = node->next;
					}
#endif
				}

				// save
				tb_xml_writer_save(writer, root);

				// exit
				tb_xml_node_exit(root);
			}
#endif
			// exit reader & writer 
			tb_xml_reader_exit(reader);
			tb_xml_writer_exit(writer);
		}
	
		// exit stream
		tb_gstream_exit(ist);
		tb_gstream_exit(ost);
	}
	
end:
	// exit tbox
	tb_exit();
	return 0;
}

