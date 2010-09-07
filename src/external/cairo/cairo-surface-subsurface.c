/* cairo - a vector graphics library with display and print output
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
 * Contributor(s):
 *      Chris Wilson <chris@chris-wilson.co.uk>
 */

#include "cairoint.h"

#include "cairo-error-private.h"
#include "cairo-surface-offset-private.h"
#include "cairo-surface-subsurface-private.h"

static cairo_status_t
_cairo_surface_subsurface_finish (void *abstract_surface)
{
    cairo_surface_subsurface_t *surface = abstract_surface;

    cairo_surface_destroy (surface->target);

    return CAIRO_STATUS_SUCCESS;
}

static cairo_surface_t *
_cairo_surface_subsurface_create_similar (void *other,
					  cairo_content_t content,
					  int width, int height)
{
    cairo_surface_subsurface_t *surface = other;
    return surface->target->backend->create_similar (surface->target, content, width, height);
}

static cairo_int_status_t
_cairo_surface_subsurface_paint (void *abstract_surface,
				 cairo_operator_t op,
				 const cairo_pattern_t *source,
				 cairo_clip_t *clip)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_rectangle_int_t rect = { 0, 0, surface->extents.width, surface->extents.height };
    cairo_status_t status;
    cairo_clip_t target_clip;

    _cairo_clip_init_copy (&target_clip, clip);
    status = _cairo_clip_rectangle (&target_clip, &rect);
    if (unlikely (status))
	goto CLEANUP;

    status = _cairo_surface_offset_paint (surface->target,
					 -surface->extents.x, -surface->extents.y,
					  op, source, &target_clip);
  CLEANUP:
    _cairo_clip_fini (&target_clip);
    return status;
}

static cairo_int_status_t
_cairo_surface_subsurface_mask (void *abstract_surface,
				cairo_operator_t op,
				const cairo_pattern_t *source,
				const cairo_pattern_t *mask,
				cairo_clip_t *clip)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_rectangle_int_t rect = { 0, 0, surface->extents.width, surface->extents.height };
    cairo_status_t status;
    cairo_clip_t target_clip;

    _cairo_clip_init_copy (&target_clip, clip);
    status = _cairo_clip_rectangle (&target_clip, &rect);
    if (unlikely (status))
	goto CLEANUP;

    status = _cairo_surface_offset_mask (surface->target,
					 -surface->extents.x, -surface->extents.y,
					 op, source, mask, &target_clip);
  CLEANUP:
    _cairo_clip_fini (&target_clip);
    return status;
}

static cairo_int_status_t
_cairo_surface_subsurface_fill (void			*abstract_surface,
				cairo_operator_t	 op,
				const cairo_pattern_t	*source,
				cairo_path_fixed_t	*path,
				cairo_fill_rule_t	 fill_rule,
				double			 tolerance,
				cairo_antialias_t	 antialias,
				cairo_clip_t		*clip)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_rectangle_int_t rect = { 0, 0, surface->extents.width, surface->extents.height };
    cairo_status_t status;
    cairo_clip_t target_clip;

    _cairo_clip_init_copy (&target_clip, clip);
    status = _cairo_clip_rectangle (&target_clip, &rect);
    if (unlikely (status))
	goto CLEANUP;

    status = _cairo_surface_offset_fill (surface->target,
					 -surface->extents.x, -surface->extents.y,
					 op, source, path, fill_rule, tolerance, antialias,
					 &target_clip);
  CLEANUP:
    _cairo_clip_fini (&target_clip);
    return status;
}

static cairo_int_status_t
_cairo_surface_subsurface_stroke (void				*abstract_surface,
				  cairo_operator_t		 op,
				  const cairo_pattern_t		*source,
				  cairo_path_fixed_t		*path,
				  const cairo_stroke_style_t	*stroke_style,
				  const cairo_matrix_t		*ctm,
				  const cairo_matrix_t		*ctm_inverse,
				  double			 tolerance,
				  cairo_antialias_t		 antialias,
				  cairo_clip_t			*clip)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_rectangle_int_t rect = { 0, 0, surface->extents.width, surface->extents.height };
    cairo_status_t status;
    cairo_clip_t target_clip;

    _cairo_clip_init_copy (&target_clip, clip);
    status = _cairo_clip_rectangle (&target_clip, &rect);
    if (unlikely (status))
	goto CLEANUP;

    status = _cairo_surface_offset_stroke (surface->target,
					   -surface->extents.x, -surface->extents.y,
					   op, source, path, stroke_style, ctm, ctm_inverse,
					   tolerance, antialias,
					   &target_clip);
  CLEANUP:
    _cairo_clip_fini (&target_clip);
    return status;
}

static cairo_int_status_t
_cairo_surface_subsurface_glyphs (void			*abstract_surface,
				  cairo_operator_t	 op,
				  const cairo_pattern_t	*source,
				  cairo_glyph_t		*glyphs,
				  int			 num_glyphs,
				  cairo_scaled_font_t	*scaled_font,
				  cairo_clip_t		*clip,
				  int *remaining_glyphs)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_rectangle_int_t rect = { 0, 0, surface->extents.width, surface->extents.height };
    cairo_status_t status;
    cairo_clip_t target_clip;

    _cairo_clip_init_copy (&target_clip, clip);
    status = _cairo_clip_rectangle (&target_clip, &rect);
    if (unlikely (status))
	goto CLEANUP;

    status = _cairo_surface_offset_glyphs (surface->target,
					   -surface->extents.x, -surface->extents.y,
					   op, source,
					   scaled_font, glyphs, num_glyphs,
					   &target_clip);
    *remaining_glyphs = 0;
  CLEANUP:
    _cairo_clip_fini (&target_clip);
    return status;
}

static cairo_status_t
_cairo_surface_subsurface_flush (void *abstract_surface)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_status_t status;

    status = CAIRO_STATUS_SUCCESS;
    if (surface->target->backend->flush != NULL)
	status = surface->target->backend->flush (surface->target);

    return status;
}

static cairo_status_t
_cairo_surface_subsurface_mark_dirty (void *abstract_surface,
				      int x, int y,
				      int width, int height)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_status_t status;

    status = CAIRO_STATUS_SUCCESS;
    if (surface->target->backend->mark_dirty_rectangle != NULL) {
	cairo_rectangle_int_t rect, extents;

	rect.x = x;
	rect.y = y;
	rect.width  = width;
	rect.height = height;

	extents.x = extents.y = 0;
	extents.width  = surface->extents.width;
	extents.height = surface->extents.height;

	if (_cairo_rectangle_intersect (&rect, &extents)) {
	    status = surface->target->backend->mark_dirty_rectangle (surface->target,
								     rect.x + surface->extents.x,
								     rect.y + surface->extents.y,
								     rect.width, rect.height);
	}
    }

    return status;
}

static cairo_bool_t
_cairo_surface_subsurface_get_extents (void *abstract_surface,
				       cairo_rectangle_int_t *extents)
{
    cairo_surface_subsurface_t *surface = abstract_surface;

    extents->x = 0;
    extents->y = 0;
    extents->width  = surface->extents.width;
    extents->height = surface->extents.height;

    return TRUE;
}

static void
_cairo_surface_subsurface_get_font_options (void *abstract_surface,
					    cairo_font_options_t *options)
{
    cairo_surface_subsurface_t *surface = abstract_surface;

    if (surface->target->backend->get_font_options != NULL)
	surface->target->backend->get_font_options (surface->target, options);
}

struct extra {
    cairo_image_surface_t *image;
    void *image_extra;
};

static cairo_status_t
_cairo_surface_subsurface_acquire_source_image (void                    *abstract_surface,
						cairo_image_surface_t  **image_out,
						void                   **extra_out)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_image_surface_t *image;
    cairo_status_t status;
    struct extra *extra;
    uint8_t *data;

    extra = tb_malloc (sizeof (struct extra));
    if (unlikely (extra == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    status = _cairo_surface_acquire_source_image (surface->target, &extra->image, &extra->image_extra);
    if (unlikely (status))
	goto CLEANUP;

    /* only copy if we need to perform sub-byte manipulation */
    if (PIXMAN_FORMAT_BPP (extra->image->pixman_format) > 8) {
	assert ((PIXMAN_FORMAT_BPP (extra->image->pixman_format) % 8) == 0);

	data = extra->image->data + surface->extents.y * extra->image->stride;
	data += PIXMAN_FORMAT_BPP (extra->image->pixman_format) / 8 * surface->extents.x;

	image = (cairo_image_surface_t *)
	    _cairo_image_surface_create_with_pixman_format (data,
							    extra->image->pixman_format,
							    surface->extents.width,
							    surface->extents.height,
							    extra->image->stride);
	if (unlikely ((status = image->base.status)))
	    goto CLEANUP_IMAGE;
    } else {
	image = (cairo_image_surface_t *)
	    _cairo_image_surface_create_with_pixman_format (NULL,
							    extra->image->pixman_format,
							    surface->extents.width,
							    surface->extents.height,
							    0);
	if (unlikely ((status = image->base.status)))
	    goto CLEANUP_IMAGE;

	pixman_image_composite32 (PIXMAN_OP_SRC,
                                  image->pixman_image, NULL, extra->image->pixman_image,
                                  surface->extents.x, surface->extents.y,
                                  0, 0,
                                  0, 0,
                                  surface->extents.width, surface->extents.height);
    }

    *image_out = image;
    *extra_out = extra;
    return CAIRO_STATUS_SUCCESS;

CLEANUP_IMAGE:
    _cairo_surface_release_source_image (surface->target, extra->image, extra->image_extra);
CLEANUP:
    tb_free (extra);
    return status;
}

static void
_cairo_surface_subsurface_release_source_image (void                   *abstract_surface,
						cairo_image_surface_t  *image,
						void                   *abstract_extra)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    struct extra *extra = abstract_extra;

    _cairo_surface_release_source_image (surface->target, extra->image, extra->image_extra);
    tb_free (extra);

    cairo_surface_destroy (&image->base);
}

static cairo_surface_t *
_cairo_surface_subsurface_snapshot (void *abstract_surface)
{
    cairo_surface_subsurface_t *surface = abstract_surface;
    cairo_image_surface_t *image, *clone;
    void *image_extra;
    cairo_status_t status;

    /* XXX Alternatively we could snapshot the target and return a subsurface
     * of that.
     */

    status = _cairo_surface_acquire_source_image (surface->target, &image, &image_extra);
    if (unlikely (status))
	return _cairo_surface_create_in_error (status);

    clone = (cairo_image_surface_t *)
	_cairo_image_surface_create_with_pixman_format (NULL,
							image->pixman_format,
							surface->extents.width,
							surface->extents.height,
							0);
    if (unlikely (clone->base.status))
	return &clone->base;

    pixman_image_composite32 (PIXMAN_OP_SRC,
                              image->pixman_image, NULL, clone->pixman_image,
                              surface->extents.x, surface->extents.y,
                              0, 0,
                              0, 0,
                              surface->extents.width, surface->extents.height);

    _cairo_surface_release_source_image (surface->target, image, image_extra);

    return &clone->base;
}

static const cairo_surface_backend_t _cairo_surface_subsurface_backend = {
    CAIRO_INTERNAL_SURFACE_TYPE_SUBSURFACE,
    _cairo_surface_subsurface_create_similar,
    _cairo_surface_subsurface_finish,

    _cairo_surface_subsurface_acquire_source_image,
    _cairo_surface_subsurface_release_source_image,
    NULL, NULL, /* acquire, release dest */
    NULL, /* clone similar */
    NULL, /* composite */
    NULL, /* fill rectangles */
    NULL, /* composite trapezoids */
    NULL, /* create span renderer */
    NULL, /* check span renderer */
    NULL, /* copy_page */
    NULL, /* show_page */
    _cairo_surface_subsurface_get_extents,
    NULL, /* old_show_glyphs */
    _cairo_surface_subsurface_get_font_options,
    _cairo_surface_subsurface_flush,
    _cairo_surface_subsurface_mark_dirty,
    NULL, /* font_fini */
    NULL, /* glyph_fini */

    _cairo_surface_subsurface_paint,
    _cairo_surface_subsurface_mask,
    _cairo_surface_subsurface_stroke,
    _cairo_surface_subsurface_fill,
    _cairo_surface_subsurface_glyphs,

    _cairo_surface_subsurface_snapshot,
};

cairo_surface_t *
cairo_surface_create_for_region (cairo_surface_t *target,
				 int x, int y,
				 int width, int height)
{
    cairo_surface_subsurface_t *surface;
    cairo_rectangle_int_t target_extents;
    cairo_bool_t is_empty;

    if (unlikely (target->status))
	return _cairo_surface_create_in_error (target->status);

    surface = tb_malloc (sizeof (cairo_surface_subsurface_t));
    if (unlikely (surface == NULL))
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));

    _cairo_surface_init (&surface->base,
			 &_cairo_surface_subsurface_backend,
			 NULL, /* device */
			 target->content);
    surface->base.type = target->type;

    surface->extents.x = x;
    surface->extents.y = y;
    surface->extents.width = width;
    surface->extents.height = height;

    if (_cairo_surface_get_extents (target, &target_extents))
        is_empty = _cairo_rectangle_intersect (&surface->extents, &target_extents);

    if (target->backend->type == CAIRO_INTERNAL_SURFACE_TYPE_SUBSURFACE) {
	/* Maintain subsurfaces as 1-depth */
	cairo_surface_subsurface_t *sub = (cairo_surface_subsurface_t *) target;
	surface->extents.x += sub->extents.x;
	surface->extents.y += sub->extents.y;
	target = sub->target;
    }

    surface->target = cairo_surface_reference (target);

    return &surface->base;
}
