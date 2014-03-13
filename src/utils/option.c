/*!The Treasure Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 * *
 * @author		ruki
 * @file		option.c
 * @ingroup 	utils
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_IMPL_TAG 		"option"

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "option.h"
#include "../libc/libc.h"
#include "../object/object.h"

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the option type
typedef struct tb_option_t
{
	// the command name
	tb_char_t 					name[64];

	// the command help
	tb_pstring_t 				help;

	// the options
	tb_option_item_t const* 	opts;

	// the option list
	tb_object_t* 				list;

}tb_option_t;

/* ///////////////////////////////////////////////////////////////////////
 * helper
 */
static __tb_inline__ tb_bool_t tb_option_is_bool(tb_char_t const* data)
{
	// check
	tb_assert_and_check_return_val(data, tb_false);
	return (!tb_stricmp(data, "y") || !tb_stricmp(data, "n"))? tb_true : tb_false;
}
static __tb_inline__ tb_bool_t tb_option_is_integer(tb_char_t const* data)
{
	// check
	tb_assert_and_check_return_val(data, tb_false);
	
	// init
	tb_char_t const* p = data;

	// skip '-'
	if (*p == '-') p++;

	// walk
	for (; *p && tb_isdigit(*p); p++);

	// ok?
	return *p? tb_false : tb_true;
}
static __tb_inline__ tb_bool_t tb_option_is_float(tb_char_t const* data)
{
	// check
	tb_assert_and_check_return_val(data, tb_false);
	
	// init
	tb_char_t const* p = data;

	// walk
	for (; *p && (tb_isdigit10(*p) || *p == '.'); p++);

	// ok?
	return *p? tb_false : tb_true;
}
static __tb_inline__ tb_option_item_t const* tb_option_item_find(tb_option_item_t const* opts, tb_char_t const* lname, tb_char_t sname)
{
	// check
	tb_assert_and_check_return_val(opts, tb_null);

	// walk
	tb_bool_t 			ok = tb_false;
	tb_option_item_t* 	item = opts;
	while (item && !ok)
	{
		switch (item->mode)
		{
		case TB_OPTION_MODE_KEY:
		case TB_OPTION_MODE_KEY_VAL:
			{
				// find lname
				if (item->lname && lname && !tb_strcmp(lname, item->lname)) 
				{
					ok = tb_true;
					break;
				}

				// find sname
				if (item->sname && sname && (sname == item->sname) && (sname != '-')) 
				{
					ok = tb_true;
					break;
				}

				// next
				item++;
			}
			break;
		case TB_OPTION_MODE_VAL:
			item++;
			break;
		case TB_OPTION_MODE_MORE:
		case TB_OPTION_MODE_END:
		default:
			item = tb_null;
			break;
		}
	}

	// ok?
	return ok? item : tb_null;
}
static __tb_inline__ tb_bool_t tb_option_check(tb_option_t* option)
{
	// check
	tb_assert_and_check_return_val(option && option->list && option->opts, tb_false);

	// walk
	tb_bool_t 			ok = tb_true;
	tb_option_item_t* 	item = option->opts;
	while (item && ok)
	{
		switch (item->mode)
		{
		case TB_OPTION_MODE_KEY:
		case TB_OPTION_MODE_KEY_VAL:
			item++;
			break;
		case TB_OPTION_MODE_VAL:
			{
				if (item->lname && !tb_option_find(option, item->lname)) ok = tb_false;
				item++;
			}
			break;
		case TB_OPTION_MODE_MORE:
		case TB_OPTION_MODE_END:
		default:
			item = tb_null;
			break;
		}
	}

	// ok?
	return ok;
}
/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_handle_t tb_option_init(tb_char_t const* name, tb_char_t const* help, tb_option_item_t const* opts)
{
	// check
	tb_assert_and_check_return_val(name && opts, tb_null);

	// make option
	tb_option_t* option = tb_malloc0(sizeof(tb_option_t));
	tb_assert_and_check_return_val(option, tb_null);

	// init option
	option->opts = opts;
	option->list = tb_dictionary_init(TB_DICTIONARY_SIZE_MICRO, tb_false);
	tb_assert_and_check_goto(option->list, fail);

	// init name
	tb_strlcpy(option->name, name, sizeof(option->name) - 1);

	// init help
	if (!tb_pstring_init(&option->help)) goto fail;
	if (help) tb_pstring_cstrcpy(&option->help, help);

	// ok
	return option;

fail:
	if (option) tb_option_exit(option);
	return tb_null;
}
tb_void_t tb_option_exit(tb_handle_t handle)
{
	tb_option_t* option = (tb_option_t*)handle;
	if (option)
	{
		// exit help
		tb_pstring_exit(&option->help);

		// exit list
		if (option->list) tb_object_exit(option->list);
		option->list = tb_null;

		// exit option
		tb_free(option);
	}
}
tb_bool_t tb_option_find(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, tb_false);

	// find it
	return tb_dictionary_val(option->list, name)? tb_true : tb_false;
}
tb_bool_t tb_option_done(tb_handle_t handle, tb_size_t argc, tb_char_t** argv)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && option->opts, tb_false);

	// walk arguments
	tb_size_t 			i = 0;
	tb_size_t 			more = 0;
	tb_option_item_t* 	item = option->opts;
	tb_option_item_t* 	last = tb_null;
	for (i = 0; i < argc; i++)
	{
		// the argument
		tb_char_t* p = argv[i];
		tb_char_t* e = p + tb_strlen(p);
		tb_assert_and_check_return_val(p && p < e, tb_false);

		// is long key?
		if (p + 2 < e && p[0] == '-' && p[1] == '-' && tb_isalpha(p[2]))
		{
			// the key
			tb_char_t key[512] = {0};
			{
				tb_char_t* k = key;
				tb_char_t* e = key + 511;
				for (p += 2; *p && *p != '=' && k < e; p++, k++) *k = *p; 
			}

			// the val
			tb_char_t* val = (*p == '=')? (p + 1) : tb_null;

			// trace
			tb_trace_impl("[lname]: %s => %s", key, val);

			// find the item
			tb_option_item_t const* find = tb_option_item_find(option->opts, key, '\0');
			if (find)
			{
				// check key & val
				if (!val == !(find->mode == TB_OPTION_MODE_KEY_VAL))
				{
					// has value?
					tb_object_t* object = tb_null;
					if (val)
					{
						// init the value object
						switch (find->type)
						{
						case TB_OPTION_TYPE_CSTR:
							object = tb_string_init_from_cstr(val);
							break;
						case TB_OPTION_TYPE_INTEGER:
							tb_assert_and_check_return_val(tb_option_is_integer(val), tb_false);
							object = tb_number_init_from_sint64(tb_atoll(val));
							break;
						case TB_OPTION_TYPE_BOOL:
							tb_assert_and_check_return_val(tb_option_is_bool(val), tb_false);
							object = tb_boolean_init(!tb_stricmp(val, "y")? tb_true : tb_false);
							break;
#ifdef TB_CONFIG_TYPE_FLOAT
						case TB_OPTION_TYPE_FLOAT:
							tb_assert_and_check_return_val(tb_option_is_float(val), tb_false);
							object = tb_number_init_from_double(tb_atof(val));
							break;
#endif
						default:
							tb_assert_and_check_return_val(0, tb_false);
							break;
						}
					}
					else
					{
						// check
						tb_assert_and_check_return_val(find->type == TB_OPTION_TYPE_BOOL, tb_false);

						// key => true
						object = tb_boolean_init(tb_true);
					}

					// add the value object
					if (object)
					{
						tb_dictionary_set(option->list, key, object);
						if (tb_isalpha(find->sname)) 
						{
							tb_char_t ch[2] = {0};
							ch[0] = find->sname;
							tb_dictionary_set(option->list, ch, object);
							tb_object_inc(object);
						}
					}
				}
				else if (val)
				{
					// print
					tb_printf("%s: unrecognized option value '--%s=%s'\n", option->name, key, val);

					// next
					continue ;
				}
				else
				{
					// print
					tb_printf("%s: no option value '--%s='\n", option->name, key);

					// next
					continue ;
				}
			}
			else
			{
				// print
				tb_printf("%s: unrecognized option '--%s'\n", option->name, key);

				// next
				continue ;
			}
		}
		// is short key?
		else if (p + 1 < e && p[0] == '-' && tb_isalpha(p[1]))
		{
			// the key
			tb_char_t key[512] = {0};
			{
				tb_char_t* k = key;
				tb_char_t* e = key + 511;
				for (p += 1; *p && *p != '=' && k < e; p++, k++) *k = *p; 
			}

			// the val
			tb_char_t const* val = (*p == '=')? (p + 1) : tb_null;

			// trace
			tb_trace_impl("[sname]: %s => %s", key, val);

			// is short name?
			if (tb_strlen(key) != 1)
			{
				// print
				tb_printf("%s: unrecognized option '-%s'\n", option->name, key);

				// next
				continue ;
			}

			// find the item
			tb_option_item_t const* find = tb_option_item_find(option->opts, tb_null, key[0]);
			if (find)
			{
				// check key & val
				if (!val == !(find->mode == TB_OPTION_MODE_KEY_VAL))
				{
					// has value?
					tb_object_t* object = tb_null;
					if (val)
					{
						// add value
						switch (find->type)
						{
						case TB_OPTION_TYPE_CSTR:
							object = tb_string_init_from_cstr(val);
							break;
						case TB_OPTION_TYPE_INTEGER:
							tb_assert_and_check_return_val(tb_option_is_integer(val), tb_false);
							object = tb_number_init_from_sint64(tb_atoll(val));
							break;
						case TB_OPTION_TYPE_BOOL:
							tb_assert_and_check_return_val(tb_option_is_bool(val), tb_false);
							object = tb_boolean_init(!tb_stricmp(val, "y")? tb_true : tb_false);
							break;
#ifdef TB_CONFIG_TYPE_FLOAT
						case TB_OPTION_TYPE_FLOAT:
							tb_assert_and_check_return_val(tb_option_is_float(val), tb_false);
							object = tb_number_init_from_double(tb_atof(val));
							break;
#endif
						default:
							tb_assert_and_check_return_val(0, tb_false);
							break;
						}
					}
					else
					{
						// check
						tb_assert_and_check_return_val(find->type == TB_OPTION_TYPE_BOOL, tb_false);

						// key => true
						object = tb_boolean_init(tb_true);
					}

					// add the value object 
					if (object)
					{
						tb_dictionary_set(option->list, key, object);
						if (find->lname)
						{
							tb_dictionary_set(option->list, find->lname, object);
							tb_object_inc(object);
						}
					}
				}
				else if (val)
				{
					// print
					tb_printf("%s: unrecognized option value '--%s=%s'\n", option->name, key, val);

					// next
					continue ;
				}
				else
				{
					// print
					tb_printf("%s: no option value '--%s='\n", option->name, key);

					// next
					continue ;
				}
			}
			else
			{
				// print
				tb_printf("%s: unrecognized option '-%s'\n", option->name, key);

				// next
				continue ;
			}
		}
		// is value?
		else
		{
			// trace
			tb_trace_impl("[val]: %s", p);

			// find the value item 
			while (item && item->mode != TB_OPTION_MODE_VAL && item->mode != TB_OPTION_MODE_END && item->mode != TB_OPTION_MODE_MORE)
				item++;

			// has value item?
			if (item->mode == TB_OPTION_MODE_VAL)
			{
				// check
				tb_assert_and_check_return_val(item->lname, tb_false);

				// add value
				switch (item->type)
				{
				case TB_OPTION_TYPE_CSTR:
					tb_dictionary_set(option->list, item->lname, tb_string_init_from_cstr(p));
					break;
				case TB_OPTION_TYPE_INTEGER:
					tb_assert_and_check_return_val(tb_option_is_integer(p), tb_false);
					tb_dictionary_set(option->list, item->lname, tb_number_init_from_sint64(tb_atoll(p)));
					break;
				case TB_OPTION_TYPE_BOOL:
					tb_assert_and_check_return_val(tb_option_is_bool(p), tb_false);
					tb_dictionary_set(option->list, item->lname, tb_boolean_init(!tb_stricmp(p, "y")? tb_true : tb_false));
					break;
#ifdef TB_CONFIG_TYPE_FLOAT
				case TB_OPTION_TYPE_FLOAT:
					tb_assert_and_check_return_val(tb_option_is_float(p), tb_false);
					tb_dictionary_set(option->list, item->lname, tb_number_init_from_double(tb_atof(p)));
					break;
#endif
				default:
					tb_assert_and_check_return_val(0, tb_false);
					break;
				}

				// save last
				last = item;

				// next item
				item++;
			}
			// has more item?
			else if (item->mode == TB_OPTION_MODE_MORE && last)
			{
				// the more name
				tb_char_t name[64] = {0};
				tb_snprintf(name, 63, "more%lu", more);

				// add value
				switch (last->type)
				{
				case TB_OPTION_TYPE_CSTR:
					tb_dictionary_set(option->list, name, tb_string_init_from_cstr(p));
					break;
				case TB_OPTION_TYPE_INTEGER:
					tb_assert_and_check_return_val(tb_option_is_integer(p), tb_false);
					tb_dictionary_set(option->list, name, tb_number_init_from_sint64(tb_atoll(p)));
					break;
				case TB_OPTION_TYPE_BOOL:
					tb_assert_and_check_return_val(tb_option_is_bool(p), tb_false);
					tb_dictionary_set(option->list, name, tb_boolean_init(!tb_stricmp(p, "y")? tb_true : tb_false));
					break;
#ifdef TB_CONFIG_TYPE_FLOAT
				case TB_OPTION_TYPE_FLOAT:
					tb_assert_and_check_return_val(tb_option_is_float(p), tb_false);
					tb_dictionary_set(option->list, name, tb_number_init_from_double(tb_atof(p)));
					break;
#endif
				default:
					tb_assert_and_check_return_val(0, tb_false);
					break;
				}

				// next more
				more++;
			}
		}
	}

	// ok
	return tb_true;//tb_option_check(option);
}
tb_void_t tb_option_dump(tb_handle_t handle)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return(option && option->list);

	// dump 
	tb_object_dump(option->list);
}
tb_void_t tb_option_help(tb_handle_t handle)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return(option && option->opts);

	// dump usage head
	tb_printf("======================================================================\n");
	tb_printf("[usage]: %s", option->name);

	// dump usage item
	tb_bool_t 			bopt = tb_false;
	tb_option_item_t* 	item = option->opts;
	while (item)
	{
		// dump options
		if (bopt && item->mode != TB_OPTION_MODE_KEY && item->mode != TB_OPTION_MODE_KEY_VAL)
		{
			tb_printf(" [options]");
			bopt = tb_false;
		}

		// dump item
		switch (item->mode)
		{
		case TB_OPTION_MODE_KEY:
		case TB_OPTION_MODE_KEY_VAL:
			{
				bopt = tb_true;
				item++;
			}
			break;
		case TB_OPTION_MODE_VAL:
			{
				tb_printf(" %s", item->lname);
				item++;
			}
			break;
		case TB_OPTION_MODE_MORE:
			tb_printf(" ...");
		case TB_OPTION_MODE_END:
		default:
			item = tb_null;
			break;
		}
	}

	// dump usage tail
	tb_printf("\n\n");

	// dump help
	if (tb_pstring_size(&option->help)) 
		tb_printf("[help]:  %s\n\n", tb_pstring_cstr(&option->help));

	// dump options head
	tb_printf("[options]: \n");
	for (item = option->opts; item; )
	{
		// dump item
		tb_size_t spaces = 32;
		switch (item->mode)
		{
		case TB_OPTION_MODE_KEY:
		case TB_OPTION_MODE_KEY_VAL:
			{
				// dump spaces
				tb_printf("  "); spaces -= 3;

				// has short name?
				if (tb_isalpha(item->sname))
				{
					// dump short name
					tb_printf("-%c", item->sname);
					spaces -= 2;

					// dump long name
					if (item->lname) 
					{
						tb_printf(", --%s", item->lname);
						spaces -= 4;
						if (tb_strlen(item->lname) <= spaces) spaces -= tb_strlen(item->lname);
					}
				}
				// dump long name
				else if (item->lname) 
				{
					tb_printf("    --%s", item->lname);
					spaces -= 6;
					if (tb_strlen(item->lname) <= spaces) spaces -= tb_strlen(item->lname);
				}

				// dump value
				if (item->mode == TB_OPTION_MODE_KEY_VAL)
				{
					switch (item->type)
					{
					case TB_OPTION_TYPE_BOOL:
						tb_printf("=BOOL"); spaces -= 5;
						break;
					case TB_OPTION_TYPE_CSTR:
						tb_printf("=STRING"); spaces -= 7;
						break;
					case TB_OPTION_TYPE_INTEGER:
						tb_printf("=INTEGER"); spaces -= 8;
						break;
					case TB_OPTION_TYPE_FLOAT:
						tb_printf("=FLOAT"); spaces -= 6;
						break;
					default:
						break;
					}
				}

				// dump help
				if (item->help) 
				{
					tb_char_t 			line[8192] = {0};
					tb_char_t const* 	pb = item->help;
					tb_char_t* 			qb = line;
					tb_char_t* 			qe = line + 8192;
					while (qb < qe)
					{
						if (*pb != '\n' && *pb) *qb++ = *pb++;
						else
						{
							// strip line and next
							*qb = '\0'; qb = line;

							// dump spaces
							while (spaces--) tb_printf(" ");

							// dump help line
							tb_printf("%s", line);

							// reset spaces
							spaces = 32;

							// next or end?
							if (*pb) 
							{
								// dump new line
								tb_printf("\n");
								pb++;
							}
							else break;
						}
					}
				}

				// dump newline
				tb_printf("\n");

				// next
				item++;
			}
			break;
		case TB_OPTION_MODE_VAL:
			item++;
			break;
		case TB_OPTION_MODE_MORE:
		case TB_OPTION_MODE_END:
		default:
			item = tb_null;
			break;
		}
	}

	// dump options tail
	tb_printf("\n\n");

	// dump values head
	tb_printf("[values]: \n");
	for (item = option->opts; item; )
	{
		// dump item
		tb_size_t spaces = 32;
		switch (item->mode)
		{
		case TB_OPTION_MODE_KEY:
		case TB_OPTION_MODE_KEY_VAL:
			item++;
			break;
		case TB_OPTION_MODE_VAL:
			{
				// dump spaces
				tb_printf("  "); spaces -= 3;

				// dump long name
				if (item->lname) 
				{
					tb_printf("%s", item->lname);
					if (tb_strlen(item->lname) <= spaces) spaces -= tb_strlen(item->lname);
				}

				// dump help
				if (item->help) 
				{
					tb_char_t 			line[8192] = {0};
					tb_char_t const* 	pb = item->help;
					tb_char_t* 			qb = line;
					tb_char_t* 			qe = line + 8192;
					while (qb < qe)
					{
						if (*pb != '\n' && *pb) *qb++ = *pb++;
						else
						{
							// strip line and next
							*qb = '\0'; qb = line;

							// dump spaces
							while (spaces--) tb_printf(" ");

							// dump help line
							tb_printf("%s", line);

							// reset spaces
							spaces = 32;

							// next or end?
							if (*pb) 
							{
								// dump new line
								tb_printf("\n");
								pb++;
							}
							else break;
						}
					}
				}

				// dump newline
				tb_printf("\n");

				// next
				item++;
			}
			break;
		case TB_OPTION_MODE_MORE:
			tb_printf("  ...\n");
		case TB_OPTION_MODE_END:
		default:
			item = tb_null;
			break;
		}
	}

	// dump values tail
	tb_printf("\n");
}
tb_char_t const* tb_option_item_cstr(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, tb_null);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, tb_null);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_STRING, tb_null);

	// the option item value
	return tb_string_size(item)? tb_string_cstr(item) : tb_null;
}
tb_bool_t tb_option_item_bool(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, tb_false);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, tb_false);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_BOOLEAN, tb_false);

	// the option item value
	return tb_boolean_bool(item);
}
tb_uint8_t tb_option_item_uint8(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_uint8(item);
}
tb_sint8_t tb_option_item_sint8(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_sint8(item);
}
tb_uint16_t tb_option_item_uint16(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_uint16(item);
}
tb_sint16_t tb_option_item_sint16(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_sint16(item);
}
tb_uint32_t tb_option_item_uint32(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_uint32(item);
}
tb_sint32_t tb_option_item_sint32(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_sint32(item);
}
tb_uint64_t tb_option_item_uint64(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_uint64(item);
}
tb_sint64_t tb_option_item_sint64(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_sint64(item);
}
#ifdef TB_CONFIG_TYPE_FLOAT
tb_float_t tb_option_item_float(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_float(item);
}
tb_double_t tb_option_item_double(tb_handle_t handle, tb_char_t const* name)
{
	// check
	tb_option_t* option = (tb_option_t*)handle;
	tb_assert_and_check_return_val(option && option->list && name, 0);

	// the option item
	tb_object_t* item = tb_dictionary_val(option->list, name);
	tb_check_return_val(item, 0);
	tb_assert_and_check_return_val(tb_object_type(item) == TB_OBJECT_TYPE_NUMBER, 0);

	// the option item value
	return tb_number_double(item);
}
#endif
