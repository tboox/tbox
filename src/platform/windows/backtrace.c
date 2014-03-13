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
 * @author		ruki
 * @file		backtrace.c
 * @ingroup 	platform
 *
 */

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <windows.h>
#include <dbghelp.h>

/* ///////////////////////////////////////////////////////////////////////
 * types
 */

// the dynamic func
typedef BOOL WINAPI 	(*tb_SymInitialize_t)(HANDLE hProcess, PCTSTR UserSearchPath, BOOL fInvadeProcess);
typedef BOOL WINAPI 	(*tb_SymFromAddr_t)(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
typedef DWORD WINAPI 	(*tb_SymSetOptions_t)(DWORD SymOptions);

// the symbols type
typedef struct __tb_symbols_t
{
	// SymFromAddr
	tb_SymFromAddr_t 	pSymFromAddr;

	// SymSetOptions
	tb_SymSetOptions_t 	pSymSetOptions;

	// the dynamic library
	HANDLE 				library;

	// the symbol
	SYMBOL_INFO* 		symbol;

}tb_symbols_t;

/* ///////////////////////////////////////////////////////////////////////
 * implementation
 */
tb_size_t tb_backtrace_frames(tb_pointer_t* frames, tb_size_t nframe, tb_size_t nskip)
{
	// check
	tb_check_return_val(frames && nframe, 0);

	// init
	static tb_bool_t init = tb_false;
	if (!init)
	{
		// init library
		HANDLE library = LoadLibraryExA("dbghelp.dll", tb_null, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (library)
		{
			// init SymInitialize
			tb_SymInitialize_t pSymInitialize = (tb_SymInitialize_t)GetProcAddress(library, "SymInitialize");
			if (pSymInitialize)
			{
#if 0
				// init options
				tb_SymSetOptions_t pSymSetOptions = (tb_SymSetOptions_t)GetProcAddress(library, "SymSetOptions");
				if (pSymSetOptions) pSymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
#endif

				// init symbols
				if (pSymInitialize(GetCurrentProcess(), tb_null, TRUE))
					init = tb_true;
			}
		}
	}

	// check
	tb_check_return_val(init, 0);

	// note: cannot use assert
	return (tb_size_t)CaptureStackBackTrace((DWORD)nskip, (DWORD)nframe < 63? nframe : 62, frames, tb_null);
}
tb_handle_t tb_backtrace_symbols_init(tb_pointer_t* frames, tb_size_t nframe)
{
	// check
	tb_check_return_val(frames && nframe, tb_null);

	// make symbols
	tb_symbols_t* symbols = (tb_symbols_t*)calloc(sizeof(tb_symbols_t), 1);
	tb_check_return_val(symbols, tb_null);

	// make symbol
	symbols->symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(tb_char_t), 1);
	tb_check_goto(symbols->symbol, fail);

	// init symbol
	symbols->symbol->MaxNameLen = MAX_SYM_NAME;
	symbols->symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	// init dynamic
	symbols->library = LoadLibraryExA("dbghelp.dll", tb_null, LOAD_WITH_ALTERED_SEARCH_PATH);
	tb_check_goto(symbols->library, fail);

	// init SymFromAddr
	symbols->pSymFromAddr = (tb_SymFromAddr_t)GetProcAddress(symbols->library, "SymFromAddr");
	tb_check_goto(symbols->pSymFromAddr, fail);

	// init SymSetOptions
	symbols->pSymSetOptions = (tb_SymSetOptions_t)GetProcAddress(symbols->library, "SymSetOptions");
	tb_check_goto(symbols->pSymSetOptions, fail);

	// ok
	return symbols;

fail:
	tb_backtrace_symbols_exit(symbols);
	return tb_null;
}
tb_char_t const* tb_backtrace_symbols_name(tb_handle_t handle, tb_pointer_t* frames, tb_size_t nframe, tb_size_t iframe)
{
	// check
	tb_symbols_t* symbols = (tb_symbols_t*)handle;
	tb_check_return_val(symbols && symbols->pSymFromAddr && symbols->symbol && frames && nframe && iframe < nframe, tb_null);

	// done symbol
	if (!symbols->pSymFromAddr(GetCurrentProcess(), (DWORD64)(frames[iframe]), 0, symbols->symbol)) return tb_null;
	
	// the symbol name
	return symbols->symbol->Name;
}
tb_void_t tb_backtrace_symbols_exit(tb_handle_t handle)
{
	tb_symbols_t* symbols = (tb_symbols_t*)handle;
	if (symbols) 
	{
		if (symbols->symbol) free(symbols->symbol);
		free(symbols);
	}
}

