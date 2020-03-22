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
 * Copyright (C) 2009-2020, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        process_group.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_PROCESS_GROUP_H
#define TB_PLATFORM_PROCESS_GROUP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the process group ref type
typedef __tb_typeref__(process_group);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init a given process 
 * 
 * @code
 
    // init process group
    tb_process_group_ref_t group = tb_process_group_init("/bin/echo", tb_null, tb_null);
    if (process)
    {
        // run process in group
        tb_process_attr_t attr = {0};
        attr.group = group;
        tb_process_run("echo", argv, &attr);

        // exit process group
        tb_process_group_exit(group);
    }

 * @endcode
 *
 * @param name          the process group name, it will create an anonymous process group if be null
 *
 * @return              the process group
 */
tb_process_group_ref_t  tb_process_group_init(tb_char_t const* name);

/*! exit the process group
 *
 * @param group         the process group
 */
tb_void_t               tb_process_group_exit(tb_process_group_ref_t group);

/*! kill all processes in the process group
 *
 * @param group         the process group
 */
tb_void_t               tb_process_group_kill(tb_process_group_ref_t group);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
