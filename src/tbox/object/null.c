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
 * Copyright (C) 2009 - 2017, ruki All rights reserved.
 *
 * @author      ruki
 * @file        null.c
 * @ingroup     object
 *
 */
 
/* //////////////////////////////////////////////////////////////////////////////////////
 * trace
 */
#define TB_TRACE_MODULE_NAME        "oc_null"
#define TB_TRACE_MODULE_DEBUG       (0)

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "object.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
static tb_object_ref_t tb_oc_null_copy(tb_object_ref_t object)
{
    return object;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * globals
 */

// null
static tb_object_t const g_null = 
{
    TB_OBJECT_FLAG_READONLY | TB_OBJECT_FLAG_SINGLETON
,   TB_OBJECT_TYPE_NULL
,   1
,   tb_null
,   tb_oc_null_copy
,   tb_null
,   tb_null

};

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
tb_object_ref_t tb_oc_null_init()
{
    return (tb_object_ref_t)&g_null;
}

