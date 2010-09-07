/* Cairo - a vector graphics library with display and print output
 *
 * Copyright © 2009 Chris Wilson
 * Copyright © 2009 Eric Anholt
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
 * The Initial Developer of the Original Code is Chris Wilson.
 */

#include "cairoint.h"

#include "cairo-drm-private.h"

#include <dlfcn.h>

#include <state_tracker/drm_api.h>
#include <pipe/p_inlines.h>
#include <pipe/p_screen.h>
#include <pipe/p_context.h>

typedef struct _gallium_surface gallium_surface_t;
typedef struct _gallium_device gallium_device_t;

struct _gallium_device {
    cairo_drm_device_t base;
    cairo_mutex_t mutex;

    void *dlhandle;
    struct drm_api *api;

    struct pipe_screen *screen;
    struct pipe_context *pipe;

    int max_size;
};

struct _gallium_surface {
    cairo_drm_surface_t base;

    struct pipe_buffer *buffer;
    enum pipe_format pipe_format;

    struct pipe_texture *texture;

    cairo_surface_t *fallback;
};

static cairo_surface_t *
gallium_surface_create_internal (gallium_device_t *device,
				 cairo_content_t content,
				 enum pipe_format format,
				 int width, int height);

static gallium_device_t *
gallium_device_acquire (cairo_drm_device_t *base_dev)
{
    gallium_device_t *device = (gallium_device_t *) base_dev;
    CAIRO_MUTEX_LOCK (device->mutex);
    return device;
}

static void
gallium_device_release (gallium_device_t *device)
{
    CAIRO_MUTEX_UNLOCK (device->mutex);
}

static cairo_format_t
_cairo_format_from_pipe_format (enum pipe_format format)
{
    switch ((int) format) {
    case PIPE_FORMAT_A8_UNORM:
	return CAIRO_FORMAT_A8;
    case PIPE_FORMAT_A8R8G8B8_UNORM:
	return CAIRO_FORMAT_ARGB32;
    default:
	return CAIRO_FORMAT_INVALID;
    }
}

static enum pipe_format
pipe_format_from_content (cairo_content_t content)
{
    if (content == CAIRO_CONTENT_ALPHA)
	return PIPE_FORMAT_A8_UNORM;
    else
	return PIPE_FORMAT_A8R8G8B8_UNORM;
}

static cairo_bool_t
format_is_supported_destination (gallium_device_t *device,
	                         enum pipe_format format)
{
    return device->screen->is_format_supported (device->screen,
					        format,
						0,
						PIPE_TEXTURE_USAGE_RENDER_TARGET,
						0);
}

static cairo_bool_t
format_is_supported_source (gallium_device_t *device,
	                    enum pipe_format format)
{
    return device->screen->is_format_supported (device->screen,
					        format,
						0,
						PIPE_TEXTURE_USAGE_SAMPLER,
						0);
}

static cairo_surface_t *
gallium_surface_create_similar (void			*abstract_src,
				cairo_content_t		 content,
				int			 width,
				int			 height)
{
    gallium_surface_t *other = abstract_src;
    gallium_device_t *device;
    enum pipe_format pipe_format;
    cairo_surface_t *surface = NULL;

    device = gallium_device_acquire (other->base.device);

    if (MAX (width, height) > device->max_size)
	goto RELEASE;

    pipe_format = pipe_format_from_content (content);

    if (! format_is_supported_destination (device, pipe_format))
	goto RELEASE;

    surface = gallium_surface_create_internal (device,
					       content, pipe_format,
					       width, height);

RELEASE:
    gallium_device_release (device);

    return surface;
}

static cairo_status_t
gallium_surface_finish (void *abstract_surface)
{
    gallium_surface_t *surface = abstract_surface;
    gallium_device_t *device;

    device = gallium_device_acquire (surface->base.device);
    device->screen->buffer_destroy (surface->buffer);
    gallium_device_release (device);

    return _cairo_drm_surface_finish (&surface->base);
}

static void
gallium_surface_unmap (void *closure)
{
    gallium_surface_t *surface = closure;
    gallium_device_t *device;

    device = gallium_device_acquire (surface->base.device);
    pipe_buffer_unmap (device->screen, surface->buffer);
    gallium_device_release (device);
}

static cairo_status_t
gallium_surface_acquire_source_image (void *abstract_surface,
				      cairo_image_surface_t **image_out,
				      void **image_extra)
{
    gallium_surface_t *surface = abstract_surface;
    gallium_device_t *device;
    cairo_format_t format;
    cairo_image_surface_t *image;
    cairo_status_t status;
    void *ptr;

    if (surface->fallback != NULL) {
	*image_out = (cairo_image_surface_t *)
	    cairo_surface_reference (surface->fallback);
	*image_extra = NULL;
	return CAIRO_STATUS_SUCCESS;
    }

    if (unlikely (surface->base.width == 0 || surface->base.height == 0)) {
	image = (cairo_image_surface_t *)
	    cairo_image_surface_create (surface->base.format, 0, 0);
	status = image->base.status;
	if (unlikely (status))
	    return status;

	*image_out = image;
	*image_extra = NULL;
	return CAIRO_STATUS_SUCCESS;
    }

    format = _cairo_format_from_pipe_format (surface->pipe_format);
    if (format == CAIRO_FORMAT_INVALID)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    device = gallium_device_acquire (surface->base.device);
    ptr = pipe_buffer_map (device->screen, surface->buffer,
			   PIPE_BUFFER_USAGE_CPU_READ);
    gallium_device_release (device);

    image = (cairo_image_surface_t *)
	cairo_image_surface_create_for_data (ptr, format,
					     surface->base.width,
					     surface->base.height,
					     surface->base.stride);
    if (unlikely (image->base.status))
	return image->base.status;

    status = _cairo_user_data_array_set_data (&image->base.user_data,
					      (cairo_user_data_key_t *) &surface->fallback,
					      surface,
					      gallium_surface_unmap);
    if (unlikely (status)) {
	cairo_surface_destroy (&image->base);
	return status;
    }

    *image_out = (cairo_image_surface_t *) image;
    *image_extra = NULL;
    return CAIRO_STATUS_SUCCESS;
}

static void
gallium_surface_release_source_image (void *abstract_surface,
				      cairo_image_surface_t *image,
				      void *image_extra)
{
    cairo_surface_destroy (&image->base);
}

static cairo_status_t
gallium_surface_acquire_dest_image (void *abstract_surface,
				    cairo_rectangle_int_t *interest_rect,
				    cairo_image_surface_t **image_out,
				    cairo_rectangle_int_t *image_rect_out,
				    void **image_extra)
{
    gallium_surface_t *surface = abstract_surface;
    gallium_device_t *device;
    cairo_surface_t *image;
    cairo_format_t format;
    cairo_status_t status;
    void *ptr;

    assert (surface->fallback == NULL);

    format = _cairo_format_from_pipe_format (surface->pipe_format);
    if (format == CAIRO_FORMAT_INVALID)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    device = gallium_device_acquire (surface->base.device);
    ptr = pipe_buffer_map (device->screen, surface->buffer,
			   PIPE_BUFFER_USAGE_CPU_READ_WRITE);
    gallium_device_release (device);

    image = cairo_image_surface_create_for_data (ptr, format,
						 surface->base.width,
						 surface->base.height,
						 surface->base.stride);
    if (unlikely (image->status))
	return image->status;

    status = _cairo_user_data_array_set_data (&image->user_data,
					      (cairo_user_data_key_t *) &surface->fallback,
					      surface,
					      gallium_surface_unmap);
    if (unlikely (status)) {
	cairo_surface_destroy (image);
	return status;
    }

    surface->fallback = cairo_surface_reference (image);

    *image_out = (cairo_image_surface_t *) image;
    *image_extra = NULL;

    image_rect_out->x = 0;
    image_rect_out->y = 0;
    image_rect_out->width  = surface->base.width;
    image_rect_out->height = surface->base.height;

    return CAIRO_STATUS_SUCCESS;
}

static void
gallium_surface_release_dest_image (void                    *abstract_surface,
				    cairo_rectangle_int_t   *interest_rect,
				    cairo_image_surface_t   *image,
				    cairo_rectangle_int_t   *image_rect,
				    void                    *image_extra)
{
    /* Keep the fallback until we flush, either explicitly or at the
     * end of this device. The idea is to avoid excess migration of
     * the buffer between GPU and CPU domains.
     */
    cairo_surface_destroy (&image->base);
}

static cairo_status_t
gallium_surface_flush (void *abstract_surface)
{
    gallium_surface_t *surface = abstract_surface;
    gallium_device_t *device;
    cairo_status_t status;

    if (surface->fallback == NULL)
	return CAIRO_STATUS_SUCCESS;

    /* kill any outstanding maps */
    cairo_surface_finish (surface->fallback);

    device = gallium_device_acquire (surface->base.device);
    pipe_buffer_flush_mapped_range (device->screen,
				    surface->buffer,
				    0,
				    surface->base.stride * surface->base.height);
    gallium_device_release (device);

    status = cairo_surface_status (surface->fallback);
    cairo_surface_destroy (surface->fallback);
    surface->fallback = NULL;

    return status;
}

static const cairo_surface_backend_t gallium_surface_backend = {
    CAIRO_SURFACE_TYPE_DRM,
    gallium_surface_create_similar,
    gallium_surface_finish,

    gallium_surface_acquire_source_image,
    gallium_surface_release_source_image,
    gallium_surface_acquire_dest_image,
    gallium_surface_release_dest_image,

    NULL, //gallium_surface_clone_similar,
    NULL, //gallium_surface_composite,
    NULL, //gallium_surface_fill_rectangles,
    NULL, //gallium_surface_composite_trapezoids,
    NULL, //gallium_surface_create_span_renderer,
    NULL, //gallium_surface_check_span_renderer,
    NULL, /* copy_page */
    NULL, /* show_page */
    _cairo_drm_surface_get_extents,
    NULL, /* old_show_glyphs */
    _cairo_drm_surface_get_font_options,
    gallium_surface_flush,
    NULL, /* mark_dirty_rectangle */
    NULL, //gallium_surface_scaled_font_fini,
    NULL, //gallium_surface_scaled_glyph_fini,

    _cairo_drm_surface_paint,
    _cairo_drm_surface_mask,
    _cairo_drm_surface_stroke,
    _cairo_drm_surface_fill,
    _cairo_drm_surface_show_glyphs,

    NULL, /* snapshot */

    NULL, /* is_similar */

    NULL, /* reset */
};

static int
gallium_format_stride_for_width (enum pipe_format format, int width)
{
    int stride;

    stride = 1024; /* XXX fugly */
    while (stride < width)
	stride *= 2;

    if (format == PIPE_FORMAT_A8R8G8B8_UNORM)
	stride *= 4;

    return stride;
}

static cairo_drm_bo_t *
_gallium_fake_bo_create (uint32_t size, uint32_t name)
{
    cairo_drm_bo_t *bo;

    bo = tb_malloc (sizeof (cairo_drm_bo_t));

    CAIRO_REFERENCE_COUNT_INIT (&bo->ref_count, 1);
    bo->name = name;
    bo->handle = 0;
    bo->size = size;

    return bo;
}

static void
_gallium_fake_bo_release (void *dev, void *bo)
{
    tb_free (bo);
}

static cairo_surface_t *
gallium_surface_create_internal (gallium_device_t *device,
				 cairo_content_t content,
				 enum pipe_format pipe_format,
				 int width, int height)
{
    gallium_surface_t *surface;
    cairo_status_t status;
    int stride, size;

    surface = tb_malloc (sizeof (gallium_surface_t));
    if (unlikely (surface == NULL))
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));

    _cairo_surface_init (&surface->base.base,
			 &gallium_surface_backend,
			 content);
    _cairo_drm_surface_init (&surface->base, &device->base);

    stride = gallium_format_stride_for_width (pipe_format, width);
    size = stride * height;

    surface->base.width = width;
    surface->base.height = height;
    surface->base.stride = stride;
    surface->base.bo = _gallium_fake_bo_create (size, 0);

    surface->buffer = pipe_buffer_create (device->screen,
					  0,
					  PIPE_BUFFER_USAGE_GPU_READ_WRITE |
					  PIPE_BUFFER_USAGE_CPU_READ_WRITE,
					  size);
    if (unlikely (surface->buffer == NULL)) {
	status = _cairo_drm_surface_finish (&surface->base);
	tb_free (surface);
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));
    }

    surface->pipe_format = pipe_format;
    surface->texture = NULL;

    return &surface->base.base;
}

static cairo_surface_t *
gallium_surface_create (cairo_drm_device_t *base_dev,
			cairo_content_t content,
			int width, int height)
{
    gallium_device_t *device;
    cairo_surface_t *surface;
    enum pipe_format pipe_format;

    device = gallium_device_acquire (base_dev);

    if (MAX (width, height) > device->max_size) {
	surface = _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_INVALID_SIZE));
	goto RELEASE;
    }

    pipe_format = pipe_format_from_content (content);

    if (! format_is_supported_destination (device, pipe_format)) {
	surface = _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_INVALID_FORMAT));
	goto RELEASE;
    }

    surface = gallium_surface_create_internal (device,
					       content, pipe_format,
					       width, height);

RELEASE:
    gallium_device_release (device);

    return surface;
}

static cairo_surface_t *
gallium_surface_create_for_name (cairo_drm_device_t *base_dev,
				 unsigned int name,
				 cairo_format_t format,
				 int width, int height, int stride)
{
    gallium_device_t *device;
    gallium_surface_t *surface;
    cairo_status_t status;
    cairo_content_t content;

    surface = tb_malloc (sizeof (gallium_surface_t));
    if (unlikely (surface == NULL))
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));

    switch (format) {
    default:
    case CAIRO_FORMAT_INVALID:
    case CAIRO_FORMAT_A1:
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_INVALID_FORMAT));
    case CAIRO_FORMAT_A8:
	surface->pipe_format = PIPE_FORMAT_A8_UNORM;
	break;
    case CAIRO_FORMAT_RGB24:
    case CAIRO_FORMAT_ARGB32:
	surface->pipe_format = PIPE_FORMAT_A8R8G8B8_UNORM;
	break;
    }

    device = gallium_device_acquire (base_dev);

    if (MAX (width, height) > device->max_size) {
	gallium_device_release (device);
	tb_free (surface);
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_INVALID_SIZE));
    }

    if (! format_is_supported_destination (device, surface->pipe_format)) {
	gallium_device_release (device);
	tb_free (surface);
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_INVALID_FORMAT));
    }

    content = _cairo_content_from_format (format);
    _cairo_surface_init (&surface->base.base,
			 &gallium_surface_backend,
			 content);
    _cairo_drm_surface_init (&surface->base, base_dev);

    surface->base.bo = _gallium_fake_bo_create (height * stride, name);

    surface->base.width  = width;
    surface->base.height = height;
    surface->base.stride = stride;

    surface->buffer = device->api->buffer_from_handle (device->api,
						       device->screen,
						       "cairo-gallium alien",
						       name);
    if (unlikely (surface->buffer == NULL)) {
	status = _cairo_drm_surface_finish (&surface->base);
	gallium_device_release (device);
	tb_free (surface);
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));
    }

    surface->texture = NULL;

    surface->fallback = NULL;

    gallium_device_release (device);

    return &surface->base.base;
}

static cairo_int_status_t
gallium_surface_flink (void *abstract_surface)
{
    gallium_surface_t *surface = abstract_surface;
    gallium_device_t *device;
    cairo_status_t status = CAIRO_STATUS_SUCCESS;

    device = gallium_device_acquire (surface->base.device);
    if (! device->api->global_handle_from_buffer (device->api,
						  device->screen,
						  surface->buffer,
						  &surface->base.bo->name))
    {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
    }
    gallium_device_release (device);

    return status;
}

static void
gallium_device_destroy (void *abstract_device)
{
    gallium_device_t *device = abstract_device;

    device->pipe->destroy (device->pipe);
    device->screen->destroy (device->screen);
    device->api->destroy (device->api);

    CAIRO_MUTEX_FINI (device->mutex);

    dlclose (device->dlhandle);
    tb_free (device);
}

cairo_drm_device_t *
_cairo_drm_gallium_device_create (int fd, dev_t dev, int vendor_id, int chip_id)
{
    gallium_device_t *device;
    cairo_status_t status;
    void *handle;
    const char *libdir;
    char buf[4096];
    struct drm_api *(*ctor) (void);

    /* XXX need search path + probe */
    libdir = getenv ("CAIRO_GALLIUM_LIBDIR");
    if (libdir == NULL)
	libdir = "/usr/lib/dri";
    buf[snprintf (buf, sizeof (buf)-1, "%s/i915_dri.so", libdir)] = '\0';

    handle = dlopen (buf, RTLD_LAZY);
    if (handle == NULL)
	return NULL;

    ctor = dlsym (handle, "drm_api_create");
    if (ctor == NULL) {
	dlclose (handle);
	return NULL;
    }

    device = tb_malloc (sizeof (gallium_device_t));
    if (device == NULL) {
	dlclose (handle);
	return _cairo_drm_device_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));
    }

    device->dlhandle = handle;

    CAIRO_MUTEX_INIT (device->mutex);

    device->base.status = CAIRO_STATUS_SUCCESS;

    device->base.surface.create = gallium_surface_create;
    device->base.surface.create_for_name = gallium_surface_create_for_name;
    device->base.surface.enable_scan_out = NULL;
    device->base.surface.flink = gallium_surface_flink;

    device->base.device.flush = NULL;
    device->base.device.throttle = NULL;
    device->base.device.destroy = gallium_device_destroy;

    device->base.bo.release = _gallium_fake_bo_release;

    device->api = ctor ();
    if (device->api == NULL) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP;
    }

    device->screen = device->api->create_screen (device->api, fd, NULL);
    if (device->screen == NULL) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP_API;
    }

    device->max_size = 1 << device->screen->get_param (device->screen,
						       PIPE_CAP_MAX_TEXTURE_2D_LEVELS);

    device->pipe = device->api->create_context (device->api, device->screen);
    if (device->pipe == NULL) {
	status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	goto CLEANUP_SCREEN;
    }

    return _cairo_drm_device_init (&device->base, fd, dev, device->max_size);

CLEANUP_SCREEN:
    device->screen->destroy (device->screen);
CLEANUP_API:
    device->api->destroy (device->api);
CLEANUP:
    tb_free (device);
    dlclose (handle);
    return _cairo_drm_device_create_in_error (status);
}
