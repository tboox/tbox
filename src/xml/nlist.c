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
 * Copyright (C) 2009 - 2011, ruki All rights reserved.
 *
 * \author		ruki
 * \file		nlist.c
 *
 */

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "nlist.h"
#include "../memory/memory.h"

/* /////////////////////////////////////////////////////////
 * interfaces
 */

tb_xml_nlist_t* tb_xml_nlist_init()
{
	// alloc nlist
	tb_xml_node_t* nlist = (tb_xml_node_t*)tb_malloc(sizeof(tb_xml_nlist_t));
	if (!nlist) return TB_NULL;

	// init it
	tb_memset(nlist, 0, sizeof(tb_xml_nlist_t));
	nlist->next = nlist;
	nlist->prev = nlist;
	((tb_xml_nlist_t*)nlist)->size = 0;

	return nlist;
}
tb_void_t tb_xml_nlist_exit(tb_xml_nlist_t* nlist)
{
	if (nlist)
	{
		// get head
		tb_xml_node_t* head = (tb_xml_node_t*)nlist;

		// free nodes
		tb_xml_node_t* node = head->next;
		while (node && node != head)
		{
			tb_xml_node_t* next = node->next;
			tb_xml_node_exit(node);
			node = next;
		}

		// free it
		tb_free(nlist);
	}
}

tb_xml_node_t* tb_xml_nlist_at(tb_xml_nlist_t* nlist, tb_int_t index)
{
	tb_assert(nlist && index >= 0 && index < nlist->size);
	if (!nlist || index < 0 || index >= nlist->size) return TB_NULL;
	
	// get head
	tb_xml_node_t* head = (tb_xml_node_t*)nlist;

	// by head?
	if ((index << 1) < nlist->size)
	{
		// find it
		tb_int_t i = 0;
		tb_xml_node_t* node = head->next;
		while (node && node != head)
		{
			if (i++ == index) return node;
			node = node->next;
		}
	}
	else
	{
		// find it
		tb_int_t i = nlist->size;
		tb_xml_node_t* node = head->prev;
		while (node && node != head)
		{
			if (--i == index) return node;
			node = node->prev;
		}
	}
	return TB_NULL;
}
tb_xml_node_t* tb_xml_nlist_get(tb_xml_nlist_t* nlist, tb_char_t const* name)
{
	tb_assert(nlist && name);
	if (!nlist || !name) return TB_NULL;
	
	// get head
	tb_xml_node_t* head = (tb_xml_node_t*)nlist;

	// find it
	tb_xml_node_t* node = head->next;
	while (node && node != head)
	{
		if (!tb_pstring_cstrcmp(&node->name, name))
			return node;
		node = node->next;
	}
	return TB_NULL;
}

tb_void_t tb_xml_nlist_add(tb_xml_nlist_t* nlist, tb_xml_node_t* node)
{
	tb_assert(nlist && node);
	if (!nlist || !node) return ;
	
	// get head
	tb_xml_node_t* head = (tb_xml_node_t*)nlist;

	// is null?
	if (head->next == head)
	{
		head->next = node;
		head->prev = node;
		node->next = head;
		node->prev = head;
	}
	else
	{
		head->prev->next = node;
		node->prev = head->prev;
		node->next = head;
		head->prev = node;
	}

	nlist->size++;
}
tb_void_t tb_xml_nlist_det(tb_xml_nlist_t* nlist, tb_xml_node_t* node)
{
	tb_assert(nlist && node);
	if (!nlist || !node) return ;
	
	// get head
	tb_xml_node_t* head = (tb_xml_node_t*)nlist;

	// find it
	tb_xml_node_t* item = head->next;
	while (item && item != head)
	{
		// detach it
		if (item == node) 
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
			nlist->size--;
			break;
		}
		item = item->next;
	}
}
