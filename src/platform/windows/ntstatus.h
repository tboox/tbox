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
 * @file        ntstatus.h
 *
 */
#ifndef TB_PLATFORM_WINDOWS_NTSTATUS_H
#define TB_PLATFORM_WINDOWS_NTSTATUS_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"
#include <windows.h>
#include <ntdef.h>

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

#ifndef FACILITY_NTWIN32
#   define FACILITY_NTWIN32                 (0x7)
#endif

#ifndef STATUS_SUCCESS
#   define STATUS_SUCCESS                   ((NTSTATUS)0x00000000)
#endif

#ifndef STATUS_PENDING
#   define STATUS_PENDING                   ((NTSTATUS)0x00000103)
#endif

#ifndef STATUS_END_OF_FILE
#   define STATUS_END_OF_FILE               ((NTSTATUS)0xC0000011)
#endif

#ifndef STATUS_INVALID_HANDLE
#   define STATUS_INVALID_HANDLE            ((NTSTATUS)0xC0000008)
#endif

#ifndef STATUS_OBJECT_TYPE_MISMATCH
#   define STATUS_OBJECT_TYPE_MISMATCH      ((NTSTATUS)0xC0000024)
#endif

#ifndef STATUS_INSUFFICIENT_RESOURCES
#   define STATUS_INSUFFICIENT_RESOURCES    ((NTSTATUS)0xC000009A)
#endif

#ifndef STATUS_PAGEFILE_QUOTA
#   define STATUS_PAGEFILE_QUOTA            ((NTSTATUS)0xC0000007)
#endif

#ifndef STATUS_COMMITMENT_LIMIT
#   define STATUS_COMMITMENT_LIMIT          ((NTSTATUS)0xC000012D)
#endif

#ifndef STATUS_WORKING_SET_QUOTA
#   define STATUS_WORKING_SET_QUOTA         ((NTSTATUS)0xC00000A1)
#endif

#ifndef STATUS_NO_MEMORY
#   define STATUS_NO_MEMORY                 ((NTSTATUS)0xC0000017)
#endif

#ifndef STATUS_CONFLICTING_ADDRESSES
#   define STATUS_CONFLICTING_ADDRESSES     ((NTSTATUS)0xC0000018)
#endif

#ifndef STATUS_QUOTA_EXCEEDED
#   define STATUS_QUOTA_EXCEEDED            ((NTSTATUS)0xC0000044)
#endif

#ifndef STATUS_TOO_MANY_PAGING_FILES
#   define STATUS_TOO_MANY_PAGING_FILES     ((NTSTATUS)0xC0000097)
#endif

#ifndef STATUS_REMOTE_RESOURCES
#   define STATUS_REMOTE_RESOURCES          ((NTSTATUS)0xC000013D)
#endif

#ifndef STATUS_TOO_MANY_ADDRESSES
#   define STATUS_TOO_MANY_ADDRESSES        ((NTSTATUS)0xC0000209)
#endif

#ifndef STATUS_SHARING_VIOLATION
#   define STATUS_SHARING_VIOLATION         ((NTSTATUS)0xC0000043)
#endif

#ifndef STATUS_HOPLIMIT_EXCEEDED
#   define STATUS_HOPLIMIT_EXCEEDED         ((NTSTATUS)0xC000A012L)
#endif

#ifndef STATUS_ADDRESS_ALREADY_EXISTS
#   define STATUS_ADDRESS_ALREADY_EXISTS    ((NTSTATUS)0xC000020A)
#endif

#ifndef STATUS_LINK_TIMEOUT
#   define STATUS_LINK_TIMEOUT              ((NTSTATUS)0xC000013F)
#endif

#ifndef STATUS_IO_TIMEOUT
#   define STATUS_IO_TIMEOUT                ((NTSTATUS)0xC00000B5)
#endif

#ifndef STATUS_TIMEOUT
#   define STATUS_TIMEOUT                   ((NTSTATUS)0x00000102)
#endif

#ifndef STATUS_GRACEFUL_DISCONNECT
#   define STATUS_GRACEFUL_DISCONNECT       ((NTSTATUS)0xC0000237)
#endif

#ifndef STATUS_REMOTE_DISCONNECT
#   define STATUS_REMOTE_DISCONNECT         ((NTSTATUS)0xC000013C)
#endif

#ifndef STATUS_CONNECTION_RESET
#   define STATUS_CONNECTION_RESET          ((NTSTATUS)0xC000020D)
#endif

#ifndef STATUS_LINK_FAILED
#   define STATUS_LINK_FAILED               ((NTSTATUS)0xC000013E)
#endif

#ifndef STATUS_CONNECTION_DISCONNECTED
#   define STATUS_CONNECTION_DISCONNECTED   ((NTSTATUS)0xC000020C)
#endif

#ifndef STATUS_PORT_UNREACHABLE
#   define STATUS_PORT_UNREACHABLE          ((NTSTATUS)0xC000023F)
#endif

#ifndef STATUS_HOPLIMIT_EXCEEDED
#   define STATUS_HOPLIMIT_EXCEEDED         ((NTSTATUS)0xC000A012)
#endif

#ifndef STATUS_LOCAL_DISCONNECT
#   define STATUS_LOCAL_DISCONNECT          ((NTSTATUS)0xC000013B)
#endif

#ifndef STATUS_FLTRSACTION_ABORTED
//#     define STATUS_FLTRSACTION_ABORTED       ((NTSTATUS)0x00000000)
#endif

#ifndef STATUS_CONNECTION_ABORTED
#   define STATUS_CONNECTION_ABORTED        ((NTSTATUS)0xC0000241)
#endif

#ifndef STATUS_BAD_NETWORK_PATH
#   define STATUS_BAD_NETWORK_PATH          ((NTSTATUS)0xC00000BE)
#endif

#ifndef STATUS_NETWORK_UNREACHABLE
#   define STATUS_NETWORK_UNREACHABLE       ((NTSTATUS)0xC000023C)
#endif

#ifndef STATUS_PROTOCOL_UNREACHABLE
#   define STATUS_PROTOCOL_UNREACHABLE      ((NTSTATUS)0xC000023E)
#endif

#ifndef STATUS_HOST_UNREACHABLE
#   define STATUS_HOST_UNREACHABLE          ((NTSTATUS)0xC000023D)
#endif

#ifndef STATUS_CANCELLED
#   define STATUS_CANCELLED                 ((NTSTATUS)0xC0000120)
#endif

#ifndef STATUS_REQUEST_ABORTED
#   define STATUS_REQUEST_ABORTED           ((NTSTATUS)0xC0000240)
#endif

#ifndef STATUS_BUFFER_OVERFLOW
#   define STATUS_BUFFER_OVERFLOW           ((NTSTATUS)0x80000005)
#endif

#ifndef STATUS_INVALID_BUFFER_SIZE
#   define STATUS_INVALID_BUFFER_SIZE       ((NTSTATUS)0xC0000206)
#endif

#ifndef STATUS_BUFFER_TOO_SMALL
#   define STATUS_BUFFER_TOO_SMALL          ((NTSTATUS)0xC0000023)
#endif

#ifndef STATUS_ACCESS_VIOLATION
#   define STATUS_ACCESS_VIOLATION          ((NTSTATUS)0xC0000005)
#endif

#ifndef STATUS_DEVICE_NOT_READY
#   define STATUS_DEVICE_NOT_READY          ((NTSTATUS)0xC00000A3)
#endif

#ifndef STATUS_REQUEST_NOT_ACCEPTED
#   define STATUS_REQUEST_NOT_ACCEPTED      ((NTSTATUS)0xC00000D0)
#endif

#ifndef STATUS_INVALID_NETWORK_RESPONSE
#   define STATUS_INVALID_NETWORK_RESPONSE  ((NTSTATUS)0xC00000C3)
#endif

#ifndef STATUS_NETWORK_BUSY
#   define STATUS_NETWORK_BUSY              ((NTSTATUS)0xC00000BF)
#endif

#ifndef STATUS_NO_SUCH_DEVICE
#   define STATUS_NO_SUCH_DEVICE            ((NTSTATUS)0xC000000E)
#endif

#ifndef STATUS_NO_SUCH_FILE
#   define STATUS_NO_SUCH_FILE              ((NTSTATUS)0xC000000F)
#endif

#ifndef STATUS_OBJECT_PATH_NOT_FOUND
#   define STATUS_OBJECT_PATH_NOT_FOUND     ((NTSTATUS)0xC000003A)
#endif

#ifndef STATUS_OBJECT_NAME_NOT_FOUND
#   define STATUS_OBJECT_NAME_NOT_FOUND     ((NTSTATUS)0xC0000034)
#endif

#ifndef STATUS_UNEXPECTED_NETWORK_ERROR
#   define STATUS_UNEXPECTED_NETWORK_ERROR  ((NTSTATUS)0xC00000C4)
#endif

#ifndef STATUS_INVALID_CONNECTION
#   define STATUS_INVALID_CONNECTION        ((NTSTATUS)0xC0000140)
#endif

#ifndef STATUS_REMOTE_NOT_LISTENING
#   define STATUS_REMOTE_NOT_LISTENING      ((NTSTATUS)0xC00000BC)
#endif

#ifndef STATUS_CONNECTION_REFUSED
#   define STATUS_CONNECTION_REFUSED        ((NTSTATUS)0xC0000236)
#endif

#ifndef STATUS_PIPE_DISCONNECTED
#   define STATUS_PIPE_DISCONNECTED         ((NTSTATUS)0xC00000B0)
#endif

#ifndef STATUS_INVALID_ADDRESS
#   define STATUS_INVALID_ADDRESS           ((NTSTATUS)0xC0000141)
#endif

#ifndef STATUS_INVALID_ADDRESS_COMPONENT
#   define STATUS_INVALID_ADDRESS_COMPONENT ((NTSTATUS)0xC0000207)
#endif

#ifndef STATUS_NOT_SUPPORTED
#   define STATUS_NOT_SUPPORTED             ((NTSTATUS)0xC00000BB)
#endif

#ifndef STATUS_NOT_IMPLEMENTED
#   define STATUS_NOT_IMPLEMENTED           ((NTSTATUS)0xC0000002)
#endif

#ifndef STATUS_ACCESS_DENIED
#   define STATUS_ACCESS_DENIED             ((NTSTATUS)0xC0000022)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * inlines
 */

static __tb_inline__ tb_size_t tb_ntstatus_to_winerror(tb_size_t status)
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
//  case STATUS_FLTRSACTION_ABORTED:
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
        if (    (status & (FACILITY_NTWIN32 << 16)) == (FACILITY_NTWIN32 << 16) 
            &&  (status & (ERROR_SEVERITY_ERROR | ERROR_SEVERITY_WARNING))) 
        {
            return (tb_size_t) (status & 0xffff);
        }
        else
        {
            tb_trace_e("ntstatus: unknown: %lx", status);
            return WSAEINVAL;
        }
    }
}


#endif
