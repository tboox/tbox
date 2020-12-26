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
 * @file        filelock.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_FILELOCK_H
#define TB_PLATFORM_FILELOCK_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "file.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the file lock ref type
typedef __tb_typeref__(filelock);

/// the file lock mode type
typedef enum __tb_filelock_mode_e
{
    TB_FILELOCK_MODE_NONE = 0     //!< none
,   TB_FILELOCK_MODE_EX   = 1     //!< exclusive lock mode
,   TB_FILELOCK_MODE_SH   = 2     //!< share lock mode

}tb_filelock_mode_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the file lock
 *
 * @param file      the file reference
 *
 * @return          the file lock
 */
tb_filelock_ref_t   tb_filelock_init(tb_file_ref_t file);

/*! init the file lock from the file path
 *
 * @param path      the file path
 * @param mode      the file mode
 *
 * @return          the file lock
 */
tb_filelock_ref_t   tb_filelock_init_from_path(tb_char_t const* path, tb_size_t mode);

/*! exit the file lock
 *
 * @param lock      the file lock
 */
tb_void_t           tb_filelock_exit(tb_filelock_ref_t lock);

/*! enter the file lock (block)
 *
 * @param lock      the file lock
 * @param mode      the lock mode
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_filelock_enter(tb_filelock_ref_t lock, tb_size_t mode);

/*! try to enter the file lock
 *
 * @param lock      the file lock
 * @param mode      the lock mode
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_filelock_enter_try(tb_filelock_ref_t lock, tb_size_t mode);

/*! leave the file lock
 *
 * @param lock      the file lock
 *
 * @return          tb_true or tb_false
 */
tb_bool_t           tb_filelock_leave(tb_filelock_ref_t lock);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__


#endif
