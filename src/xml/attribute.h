/*!The Tiny Box Library
 * 
 * TBox is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * TBox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with TBox; 
 * If not, see <a href="http://www.gnu.org/licenses/"> http://www.gnu.org/licenses/</a>
 * 
 * Copyright (C) 2009 - 2010, ruki All rights reserved.
 *
 * \author		ruki
 * \file		attribute.h
 *
 */
#ifndef TB_XML_ATTRIBUTE_H
#define TB_XML_ATTRIBUTE_H

// c plus plus
#ifdef __cplusplus
extern "C" {
#endif

/* /////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* /////////////////////////////////////////////////////////
 * types
 */

/* the xml attribute type
 *
 * inherit node, 
 * but since they are not actually child nodes of the element they describe, 
 * the DOM does not consider them part of the document tree.
 */
typedef struct __tb_xml_attribute_t
{
	// the node base
	tb_xml_node_t 			base;

}tb_xml_attribute_t;

// c plus plus
#ifdef __cplusplus
}
#endif

#endif
