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
 * Copyright (C) 2009 - 2019, TBOOX Open Source Group.
 *
 * @author      ruki
 * @file        stdfile.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_STDFILE_H
#define TB_PLATFORM_STDFILE_H

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

/// the stdfile ref type
typedef __tb_typeref__(stdfile);

/// the stdfile type
typedef enum __tb_stdfile_type_e
{
    TB_STDFILE_TYPE_NONE    = 0
,   TB_STDFILE_TYPE_STDIN   = 1
,   TB_STDFILE_TYPE_STDOUT  = 2
,   TB_STDFILE_TYPE_STDERR  = 3

}tb_stdfile_type_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the standard input/ouput device file
 * 
 * @param type          the file type, e.g. stdin, stdout, stderr
 *
 * @return              the stdfile 
 */
tb_stdfile_ref_t        tb_stdfile_init(tb_size_t type);

/*! exit the stdfile
 *
 * @param stdfile       the stdfile
 */
tb_void_t               tb_stdfile_exit(tb_stdfile_ref_t stdfile);

/*! get the stdfile type
 *
 * @param stdfile       the stdfile
 *
 * @return              the file type
 */
tb_size_t               tb_stdfile_type(tb_stdfile_ref_t stdfile);

/*! flush cache data to file
 * 
 * @param file          the file 
 */
tb_bool_t               tb_stdfile_flush(tb_stdfile_ref_t stdfile);

/*! read the stdfile data
 * 
 * @param file          the stdfile 
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
tb_long_t               tb_stdfile_read(tb_stdfile_ref_t file, tb_byte_t* data, tb_size_t size);

/*! write the stdfile data
 * 
 * @param file          the stdfile 
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
tb_long_t               tb_stdfile_writ(tb_stdfile_ref_t file, tb_byte_t const* data, tb_size_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__

#endif
