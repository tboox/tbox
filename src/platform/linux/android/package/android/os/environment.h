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
 * @file        environment.h
 * @ingroup     platform
 */
#ifndef TB_PLATFORM_LINUX_ANDROID_PACKAGE_ANDROID_OS_ENVIRONMENT_H
#define TB_PLATFORM_LINUX_ANDROID_PACKAGE_ANDROID_OS_ENVIRONMENT_H 

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the android.os.Environment object type
typedef struct __tb_android_package_android_os_environment_t
{
    /// the object
    jobject                 object;

}tb_android_package_android_os_environment_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the android.os.Environment object
 *
 * @return the object
 */
tb_android_package_android_os_environment_t* tb_android_package_android_os_environment(tb_noarg_t);

#endif
