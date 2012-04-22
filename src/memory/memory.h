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
 * Copyright (C) 2009 - 2012, ruki All rights reserved.
 *
 * @author		ruki
 * @file		memory.h
 * @defgroup 	memory
 *
 */
#ifndef TB_MEMORY_H
#define TB_MEMORY_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "vpool.h"
#include "fpool.h"
#include "tpool.h"
#include "gpool.h"
#include "spool.h"
#include "rpool.h"
#include "malloc.h"
#include "pbuffer.h"
#include "sbuffer.h"
#include "rbuffer.h"
#include "qbuffer.h"

/*!architecture
 *
 * <pre>
 * small block - spool ------\
 *                            vpool --
 * globl block - gpool ------|        | 
 *                            tpool --|- data: |-------------------------------|
 *                                    |
 * fixed block - rpool ------ fpool --
 * </pre>
 */

#endif

