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
 * \file		writer.h
 *
 */
#ifndef TB_XML_WRITER_H
#define TB_XML_WRITER_H

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


// the xml writer - StAX
typedef struct __tb_xml_writer_t
{
	// the stream
	tb_stream_t* 			st;


}tb_xml_writer_t;


/* /////////////////////////////////////////////////////////
 * interfaces
 */

// open & close
tb_xml_writer_t* 		tb_xml_writer_open(tb_stream_t* st);
void 					tb_xml_writer_close(tb_xml_writer_t* writer);


// c plus plus
#ifdef __cplusplus
}
#endif

#endif
