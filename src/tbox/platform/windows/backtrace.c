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
 * Copyright (C) 2009 - 2015, ruki All rights reserved.
 *
 * @author      ruki
 * @file        backtrace.c
 * @ingroup     platform
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <malloc.h>
#include "interface/interface.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */
#define TB_MAX_SYM_NAME             (2000)

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_backtrace_frames(tb_pointer_t* frames, tb_size_t nframe, tb_size_t nskip)
{
    // check
    tb_check_return_val(tb_kernel32()->RtlCaptureStackBackTrace && frames && nframe, 0);

    // note: cannot use assert
    return (tb_size_t)tb_kernel32()->RtlCaptureStackBackTrace((DWORD)nskip, (DWORD)nframe < 63? nframe : 62, frames, tb_null);
}
tb_handle_t tb_backtrace_symbols_init(tb_pointer_t* frames, tb_size_t nframe)
{
    // check
    tb_check_return_val(frames && nframe, tb_null);

    // make symbol
    tb_dbghelp_symbol_info_t* symbol = (tb_dbghelp_symbol_info_t*)calloc(sizeof(tb_dbghelp_symbol_info_t) + TB_MAX_SYM_NAME * sizeof(tb_char_t), 1);
    tb_check_return_val(symbol, tb_null);

    // init symbol
    symbol->MaxNameLen = TB_MAX_SYM_NAME;
    symbol->SizeOfStruct = sizeof(tb_dbghelp_symbol_info_t);

    // ok
    return symbol;
}
tb_char_t const* tb_backtrace_symbols_name(tb_handle_t handle, tb_pointer_t* frames, tb_size_t nframe, tb_size_t iframe)
{
    // check
    tb_dbghelp_symbol_info_t* symbol = (tb_dbghelp_symbol_info_t*)handle;
    tb_check_return_val(symbol && tb_dbghelp()->SymFromAddr && frames && nframe && iframe < nframe, tb_null);

    // done symbol
    if (!tb_dbghelp()->SymFromAddr(GetCurrentProcess(), (DWORD64)(tb_size_t)(frames[iframe]), 0, symbol)) return tb_null;
    
    // the symbol name
    return symbol->Name;
}
tb_void_t tb_backtrace_symbols_exit(tb_handle_t handle)
{
    if (handle) free(handle);
}
