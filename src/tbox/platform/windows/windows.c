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
 * @file        windows.c
 * @ingroup     platform
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "windows.h"
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_bool_t tb_windows_init_env()
{
    // ensure to init the kernel32 interfaces, because it is not singleton
    tb_kernel32_ref_t kernel32 = tb_kernel32();
    tb_assert_and_check_return_val(kernel32, tb_false);

    // ensure to init the ws2_32 interfaces, because it is not singleton
    tb_ws2_32_ref_t ws2_32 = tb_ws2_32();
    tb_assert_and_check_return_val(ws2_32, tb_false);

    // ok
    return tb_true;
}
tb_void_t tb_windows_exit_env()
{
}

