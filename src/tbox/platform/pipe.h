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
 * @file        pipe.h
 * @ingroup     platform
 *
 */
#ifndef TB_PLATFORM_PIPE_H
#define TB_PLATFORM_PIPE_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include "socket.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_enter__

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the pipe file ref type
typedef __tb_typeref__(pipe_file);

/// the pipe mode type
typedef enum __tb_pipe_mode_t
{
    TB_PIPE_MODE_RO         = 1     //!< read only
,   TB_PIPE_MODE_WO         = 2     //!< write only
,   TB_PIPE_MODE_BLOCK      = 4     //!< block mode

}tb_pipe_mode_t;

/// the pipe file event enum
typedef enum __tb_event_event_e
{
    TB_PIPE_EVENT_NONE = TB_SOCKET_EVENT_NONE
,   TB_PIPE_EVENT_CONN = TB_SOCKET_EVENT_CONN
,   TB_PIPE_EVENT_READ = TB_SOCKET_EVENT_RECV
,   TB_PIPE_EVENT_WRIT = TB_SOCKET_EVENT_SEND

}tb_pipe_event_e;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init the file of named pipe
 *
 * @note the pipe files will be inherited in child process and it will be blocked.
 *
 * @param name          the pipe name
 * @param mode          the pipe mode
 * @param buffer_size   the buffer size of pipe, it will use the default size if pass zero
 *
 * @return              the pipe file
 */
tb_pipe_file_ref_t      tb_pipe_file_init(tb_char_t const* name, tb_size_t mode, tb_size_t buffer_size);

/*! init the file pair of anonymous pipe
 *
 * @note the pipe files will be inherited in child process.
 *
 * @param pair          the pipe file pair, read: pair[0], write: pair[1]
 * @param mode          the pipe mode pair, only support TB_PIPE_MODE_BLOCK
 * @param buffer_size   the buffer size of pipe, it will use the default size if pass zero
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_pipe_file_init_pair(tb_pipe_file_ref_t pair[2], tb_size_t mode[2], tb_size_t buffer_size);

/*! exit the pipe file
 *
 * @param file          the pipe file
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_pipe_file_exit(tb_pipe_file_ref_t file);

/*! connect the given named pipe file on server side
 *
 * @param file          the pipe file
 *
 * @return              ok: 1, continue: 0; failed: -1
 */
tb_long_t               tb_pipe_file_connect(tb_pipe_file_ref_t file);

/*! read the pipe file data (non-block)
 *
 * @param file          the pipe file
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
tb_long_t               tb_pipe_file_read(tb_pipe_file_ref_t file, tb_byte_t* data, tb_size_t size);

/*! writ the pipe file data (non-block)
 *
 * @param file          the file
 * @param data          the data
 * @param size          the size
 *
 * @return              the real size or -1
 */
tb_long_t               tb_pipe_file_write(tb_pipe_file_ref_t file, tb_byte_t const* data, tb_size_t size);

/*! wait the pipe file events
 *
 * @param file          the file
 * @param events        the pipe file events
 * @param timeout       the timeout, infinity: -1
 *
 * @return              > 0: the events code, 0: timeout, -1: failed
 */
tb_long_t               tb_pipe_file_wait(tb_pipe_file_ref_t file, tb_size_t events, tb_long_t timeout);

/*! read the pipe file data for tcp with block mode
 *
 * @param file          the pipe file
 * @param data          the data
 * @param size          the size
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_pipe_file_bread(tb_pipe_file_ref_t file, tb_byte_t* data, tb_size_t size);

/*! writ the pipe file data for tcp with block mode
 *
 * @param file          the pipe file
 * @param data          the data
 * @param size          the size
 *
 * @return              tb_true or tb_false
 */
tb_bool_t               tb_pipe_file_bwrit(tb_pipe_file_ref_t file, tb_byte_t const* data, tb_size_t size);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
__tb_extern_c_leave__


#endif
