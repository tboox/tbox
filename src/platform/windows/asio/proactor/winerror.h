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
 * @file		winerror.h
 *
 */
#ifndef TB_PLATFROM_WINDOWS_ASIO_PROACTOR_WINERROR_H
#define TB_PLATFROM_WINDOWS_ASIO_PROACTOR_WINERROR_H

/* ///////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <windows.h>
#include <ntdef.h>
#include <ntstatus.h>

/* ///////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef FACILITY_NTWIN32
# 	define FACILITY_NTWIN32 		(0x7)
#endif

#ifndef STATUS_HOPLIMIT_EXCEEDED
# 	define STATUS_HOPLIMIT_EXCEEDED ((NTSTATUS)0xC000A012L)
#endif

/* ///////////////////////////////////////////////////////////////////////
 * inlines
 */

static __tb_inline__ tb_size_t tb_winerror_from_nsstatus(tb_size_t status)
{
	switch (status) 
	{
	case STATUS_SUCCESS:
		return ERROR_SUCCESS;

	case STATUS_PENDING:
		return ERROR_IO_PENDING;

	case STATUS_END_OF_FILE:
		return ERROR_HANDLE_EOF;

	case STATUS_INVALID_HANDLE:
	case STATUS_OBJECT_TYPE_MISMATCH:
		return WSAENOTSOCK;

	case STATUS_INSUFFICIENT_RESOURCES:
	case STATUS_PAGEFILE_QUOTA:
	case STATUS_COMMITMENT_LIMIT:
	case STATUS_WORKING_SET_QUOTA:
	case STATUS_NO_MEMORY:
	case STATUS_CONFLICTING_ADDRESSES:
	case STATUS_QUOTA_EXCEEDED:
	case STATUS_TOO_MANY_PAGING_FILES:
	case STATUS_REMOTE_RESOURCES:
	case STATUS_TOO_MANY_ADDRESSES:
		return WSAENOBUFS;

	case STATUS_SHARING_VIOLATION:
	case STATUS_ADDRESS_ALREADY_EXISTS:
		return WSAEADDRINUSE;

	case STATUS_LINK_TIMEOUT:
	case STATUS_IO_TIMEOUT:
	case STATUS_TIMEOUT:
		return WSAETIMEDOUT;

	case STATUS_GRACEFUL_DISCONNECT:
		return WSAEDISCON;

	case STATUS_REMOTE_DISCONNECT:
	case STATUS_CONNECTION_RESET:
	case STATUS_LINK_FAILED:
	case STATUS_CONNECTION_DISCONNECTED:
	case STATUS_PORT_UNREACHABLE:
	case STATUS_HOPLIMIT_EXCEEDED:
		return WSAECONNRESET;

	case STATUS_LOCAL_DISCONNECT:
	case STATUS_TRANSACTION_ABORTED:
	case STATUS_CONNECTION_ABORTED:
		return WSAECONNABORTED;

	case STATUS_BAD_NETWORK_PATH:
	case STATUS_NETWORK_UNREACHABLE:
	case STATUS_PROTOCOL_UNREACHABLE:
		return WSAENETUNREACH;

	case STATUS_HOST_UNREACHABLE:
		return WSAEHOSTUNREACH;

	case STATUS_CANCELLED:
	case STATUS_REQUEST_ABORTED:
		return WSAEINTR;

	case STATUS_BUFFER_OVERFLOW:
	case STATUS_INVALID_BUFFER_SIZE:
		return WSAEMSGSIZE;

	case STATUS_BUFFER_TOO_SMALL:
	case STATUS_ACCESS_VIOLATION:
		return WSAEFAULT;

	case STATUS_DEVICE_NOT_READY:
	case STATUS_REQUEST_NOT_ACCEPTED:
		return WSAEWOULDBLOCK;

	case STATUS_INVALID_NETWORK_RESPONSE:
	case STATUS_NETWORK_BUSY:
	case STATUS_NO_SUCH_DEVICE:
	case STATUS_NO_SUCH_FILE:
	case STATUS_OBJECT_PATH_NOT_FOUND:
	case STATUS_OBJECT_NAME_NOT_FOUND:
	case STATUS_UNEXPECTED_NETWORK_ERROR:
		return WSAENETDOWN;

	case STATUS_INVALID_CONNECTION:
		return WSAENOTCONN;

	case STATUS_REMOTE_NOT_LISTENING:
	case STATUS_CONNECTION_REFUSED:
		return WSAECONNREFUSED;

	case STATUS_PIPE_DISCONNECTED:
		return WSAESHUTDOWN;

	case STATUS_INVALID_ADDRESS:
	case STATUS_INVALID_ADDRESS_COMPONENT:
		return WSAEADDRNOTAVAIL;

	case STATUS_NOT_SUPPORTED:
	case STATUS_NOT_IMPLEMENTED:
		return WSAEOPNOTSUPP;

	case STATUS_ACCESS_DENIED:
		return WSAEACCES;

	default:
		if ( 	(status & (FACILITY_NTWIN32 << 16)) == (FACILITY_NTWIN32 << 16) 
			&& 	(status & (ERROR_SEVERITY_ERROR | ERROR_SEVERITY_WARNING))) 
		{
			return (tb_size_t) (status & 0xffff);
		}
		else
		{
			tb_trace("ntstatus: unknown: %lx", status);
			return WSAEINVAL;
		}
	}
}


#endif
