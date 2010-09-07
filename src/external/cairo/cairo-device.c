/* Cairo - a vector graphics library with display and print output
 *
 * Copyright © 2009 Intel Corporation
 *
 * This library is tb_free software; you can redistribute it and/or
 * modify it either under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * (the "LGPL") or, at your option, under the terms of the Mozilla
 * Public License Version 1.1 (the "MPL"). If you do not alter this
 * notice, a recipient may use your version of this file under either
 * the MPL or the LGPL.
 *
 * You should have received a copy of the LGPL along with this library
 * in the file COPYING-LGPL-2.1; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * You should have received a copy of the MPL along with this library
 * in the file COPYING-MPL-1.1
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY
 * OF ANY KIND, either express or implied. See the LGPL or the MPL for
 * the specific language governing rights and limitations.
 *
 * The Original Code is the cairo graphics library.
 *
 * The Initial Developer of the Original Code is Intel Corporation.
 *
 * Contributors(s):
 *	Chris Wilson <chris@chris-wilson.co.uk>
 */

#include "cairoint.h"
#include "cairo-device-private.h"
#include "cairo-error-private.h"

static const cairo_device_t _nil_device = {
    CAIRO_REFERENCE_COUNT_INVALID,
    CAIRO_STATUS_NO_MEMORY,
};

static const cairo_device_t _mismatch_device = {
    CAIRO_REFERENCE_COUNT_INVALID,
    CAIRO_STATUS_DEVICE_TYPE_MISMATCH,
};

static const cairo_device_t _invalid_device = {
    CAIRO_REFERENCE_COUNT_INVALID,
    CAIRO_STATUS_DEVICE_ERROR,
};

cairo_device_t *
_cairo_device_create_in_error (cairo_status_t status)
{
    switch (status) {
    case CAIRO_STATUS_NO_MEMORY:
	return (cairo_device_t *) &_nil_device;
    case CAIRO_STATUS_DEVICE_ERROR:
	return (cairo_device_t *) &_invalid_device;
    case CAIRO_STATUS_DEVICE_TYPE_MISMATCH:
	return (cairo_device_t *) &_mismatch_device;

    case CAIRO_STATUS_SUCCESS:
    case CAIRO_STATUS_LAST_STATUS:
	ASSERT_NOT_REACHED;
	/* fall-through */
    case CAIRO_STATUS_SURFACE_TYPE_MISMATCH:
    case CAIRO_STATUS_INVALID_STATUS:
    case CAIRO_STATUS_INVALID_FORMAT:
    case CAIRO_STATUS_INVALID_VISUAL:
    case CAIRO_STATUS_READ_ERROR:
    case CAIRO_STATUS_WRITE_ERROR:
    case CAIRO_STATUS_FILE_NOT_FOUND:
    case CAIRO_STATUS_TEMP_FILE_ERROR:
    case CAIRO_STATUS_INVALID_STRIDE:
    case CAIRO_STATUS_INVALID_SIZE:
    case CAIRO_STATUS_INVALID_RESTORE:
    case CAIRO_STATUS_INVALID_POP_GROUP:
    case CAIRO_STATUS_NO_CURRENT_POINT:
    case CAIRO_STATUS_INVALID_MATRIX:
    case CAIRO_STATUS_NULL_POINTER:
    case CAIRO_STATUS_INVALID_STRING:
    case CAIRO_STATUS_INVALID_PATH_DATA:
    case CAIRO_STATUS_SURFACE_FINISHED:
    case CAIRO_STATUS_PATTERN_TYPE_MISMATCH:
    case CAIRO_STATUS_INVALID_DASH:
    case CAIRO_STATUS_INVALID_DSC_COMMENT:
    case CAIRO_STATUS_INVALID_INDEX:
    case CAIRO_STATUS_CLIP_NOT_REPRESENTABLE:
    case CAIRO_STATUS_FONT_TYPE_MISMATCH:
    case CAIRO_STATUS_USER_FONT_IMMUTABLE:
    case CAIRO_STATUS_USER_FONT_ERROR:
    case CAIRO_STATUS_NEGATIVE_COUNT:
    case CAIRO_STATUS_INVALID_CLUSTERS:
    case CAIRO_STATUS_INVALID_SLANT:
    case CAIRO_STATUS_INVALID_WEIGHT:
    case CAIRO_STATUS_USER_FONT_NOT_IMPLEMENTED:
    case CAIRO_STATUS_INVALID_CONTENT:
    default:
	_cairo_error_throw (CAIRO_STATUS_NO_MEMORY);
	return (cairo_device_t *) &_nil_device;
    }
}

void
_cairo_device_init (cairo_device_t *device,
		    const cairo_device_backend_t *backend)
{
    CAIRO_REFERENCE_COUNT_INIT (&device->ref_count, 1);
    device->status = CAIRO_STATUS_SUCCESS;

    device->backend = backend;

    CAIRO_RECURSIVE_MUTEX_INIT (device->mutex);
    device->mutex_depth = 0;

    device->finished = FALSE;

    _cairo_user_data_array_init (&device->user_data);
}

cairo_device_t *
cairo_device_reference (cairo_device_t *device)
{
    if (device == NULL ||
	CAIRO_REFERENCE_COUNT_IS_INVALID (&device->ref_count))
    {
	return device;
    }

    assert (CAIRO_REFERENCE_COUNT_HAS_REFERENCE (&device->ref_count));
    _cairo_reference_count_inc (&device->ref_count);

    return device;
}
slim_hidden_def (cairo_device_reference);

cairo_status_t
cairo_device_status (cairo_device_t *device)
{
    if (device == NULL)
	return CAIRO_STATUS_NULL_POINTER;

    return device->status;
}

void
cairo_device_flush (cairo_device_t *device)
{
    if (device == NULL || device->status)
	return;

    if (device->backend->flush != NULL)
	device->backend->flush (device);
}
slim_hidden_def (cairo_device_flush);

void
cairo_device_finish (cairo_device_t *device)
{
    if (device == NULL ||
	CAIRO_REFERENCE_COUNT_IS_INVALID (&device->ref_count))
    {
	return;
    }

    if (device->finished)
	return;

    device->finished = TRUE;

    cairo_device_flush (device);

    if (device->backend->finish != NULL)
	device->backend->finish (device);
}
slim_hidden_def (cairo_device_finish);

void
cairo_device_destroy (cairo_device_t *device)
{
    if (device == NULL ||
	CAIRO_REFERENCE_COUNT_IS_INVALID (&device->ref_count))
    {
	return;
    }

    assert (CAIRO_REFERENCE_COUNT_HAS_REFERENCE (&device->ref_count));
    if (! _cairo_reference_count_dec_and_test (&device->ref_count))
	return;

    cairo_device_finish (device);

    _cairo_user_data_array_fini (&device->user_data);

    assert (device->mutex_depth == 0);
    CAIRO_MUTEX_FINI (device->mutex);

    device->backend->destroy (device);
}
slim_hidden_def (cairo_device_destroy);

cairo_device_type_t
cairo_device_get_type (cairo_device_t *device)
{
    if (device == NULL ||
	CAIRO_REFERENCE_COUNT_IS_INVALID (&device->ref_count))
    {
	return (cairo_device_type_t) -1;
    }

    return device->backend->type;
}

cairo_status_t
cairo_device_acquire (cairo_device_t *device)
{
    if (device == NULL)
	return CAIRO_STATUS_SUCCESS;

    if (unlikely (device->status))
	return device->status;

    if (unlikely (device->finished))
	return _cairo_device_set_error (device, CAIRO_STATUS_SURFACE_FINISHED); /* XXX */

    CAIRO_MUTEX_LOCK (device->mutex);
    if (device->mutex_depth++ == 0) {
	if (device->backend->lock != NULL)
	    device->backend->lock (device);
    }

    return CAIRO_STATUS_SUCCESS;
}
slim_hidden_def (cairo_device_acquire);

void
cairo_device_release (cairo_device_t *device)
{
    if (device == NULL)
	return;

    assert (device->mutex_depth > 0);

    if (--device->mutex_depth == 0) {
	if (device->backend->unlock != NULL)
	    device->backend->unlock (device);
    }

    CAIRO_MUTEX_UNLOCK (device->mutex);
}
slim_hidden_def (cairo_device_release);

cairo_status_t
_cairo_device_set_error (cairo_device_t *device,
			 cairo_status_t  status)
{
    if (status == CAIRO_STATUS_SUCCESS || status >= CAIRO_INT_STATUS_UNSUPPORTED)
	return status;

    /* Don't overwrite an existing error. This preserves the first
     * error, which is the most significant. */
    _cairo_status_set_error (&device->status, status);

    return _cairo_error (status);
}

/**
 * cairo_device_get_reference_count:
 * @device: a #cairo_device_t
 *
 * Returns the current reference count of @device.
 *
 * Return value: the current reference count of @device.  If the
 * object is a nil object, 0 will be returned.
 *
 * Since: 1.10
 **/
unsigned int
cairo_device_get_reference_count (cairo_device_t *device)
{
    if (device == NULL ||
	CAIRO_REFERENCE_COUNT_IS_INVALID (&device->ref_count))
	return 0;

    return CAIRO_REFERENCE_COUNT_GET_VALUE (&device->ref_count);
}

/**
 * cairo_device_get_user_data:
 * @device: a #cairo_device_t
 * @key: the address of the #cairo_user_data_key_t the user data was
 * attached to
 *
 * Return user data previously attached to @device using the
 * specified key.  If no user data has been attached with the given
 * key this function returns %NULL.
 *
 * Return value: the user data previously attached or %NULL.
 *
 * Since: 1.10
 **/
void *
cairo_device_get_user_data (cairo_device_t		 *device,
			    const cairo_user_data_key_t *key)
{
    return _cairo_user_data_array_get_data (&device->user_data,
					    key);
}

/**
 * cairo_device_set_user_data:
 * @device: a #cairo_device_t
 * @key: the address of a #cairo_user_data_key_t to attach the user data to
 * @user_data: the user data to attach to the #cairo_device_t
 * @destroy: a #cairo_destroy_func_t which will be called when the
 * #cairo_t is destroyed or when new user data is attached using the
 * same key.
 *
 * Attach user data to @device.  To remove user data from a surface,
 * call this function with the key that was used to set it and %NULL
 * for @data.
 *
 * Return value: %CAIRO_STATUS_SUCCESS or %CAIRO_STATUS_NO_MEMORY if a
 * slot could not be allocated for the user data.
 *
 * Since: 1.10
 **/
cairo_status_t
cairo_device_set_user_data (cairo_device_t		 *device,
			    const cairo_user_data_key_t *key,
			    void			 *user_data,
			    cairo_destroy_func_t	  destroy)
{
    if (CAIRO_REFERENCE_COUNT_IS_INVALID (&device->ref_count))
	return device->status;

    return _cairo_user_data_array_set_data (&device->user_data,
					    key, user_data, destroy);
}
