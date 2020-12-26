/*!The Treasure Box Library
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2009-present, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        virtual_memory.h
 * @defgroup    platform
 *
 */
#ifndef TB_PLATFORM_VIRTUAL_MEMORY_H
#define TB_PLATFORM_VIRTUAL_MEMORY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/// the virtual memory data size minimum
#define TB_VIRTUAL_MEMORY_DATA_MINN                 (128 * 1024)

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! malloc the virtual memory
 *
 * @param size          the size
 *
 * @return              the data address
 */
tb_pointer_t            tb_virtual_memory_malloc(tb_size_t size);

/*! realloc the virtual memory
 *
 * @param data          the data address
 * @param size          the size
 *
 * @return              the new data address
 */
tb_pointer_t            tb_virtual_memory_ralloc(tb_pointer_t data, tb_size_t size);

/*! free the virtual memory
 *
 * @param data          the data address
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_virtual_memory_free(tb_pointer_t data);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif

