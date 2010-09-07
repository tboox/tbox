/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2005 Red Hat, Inc
 * Copyright © 2007 Adrian Johnson
 * Copyright © 2009 Chris Wilson
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
 * The Initial Developer of the Original Code is Red Hat, Inc.
 *
 * Contributor(s):
 *      Chris Wilson <chris@chris-wilson.co.uk>
 */

#include "cairoint.h"

#include "cairo-error-private.h"
#include "cairo-surface-wrapper-private.h"

/* A collection of routines to facilitate surface wrapping */

static void
_copy_transformed_pattern (cairo_pattern_t *pattern,
			   const cairo_pattern_t *original,
			   const cairo_matrix_t  *ctm_inverse)
{
    _cairo_pattern_init_static_copy (pattern, original);

    /* apply device_transform first so that it is transformed by ctm_inverse */
    if (original->type == CAIRO_PATTERN_TYPE_SURFACE) {
	cairo_surface_pattern_t *surface_pattern;
	cairo_surface_t *surface;

        surface_pattern = (cairo_surface_pattern_t *) original;
        surface = surface_pattern->surface;

	if (_cairo_surface_has_device_transform (surface))
	    _cairo_pattern_transform (pattern, &surface->device_transform);
    }

    if (! _cairo_matrix_is_identity (ctm_inverse))
	_cairo_pattern_transform (pattern, ctm_inverse);
}

static inline cairo_bool_t
_cairo_surface_wrapper_needs_device_transform (cairo_surface_wrapper_t *wrapper)
{
    return ! _cairo_matrix_is_identity (&wrapper->target->device_transform);
}

cairo_status_t
_cairo_surface_wrapper_acquire_source_image (cairo_surface_wrapper_t *wrapper,
					     cairo_image_surface_t  **image_out,
					     void                   **image_extra)
{
    if (unlikely (wrapper->target->status))
	return wrapper->target->status;

    return _cairo_surface_acquire_source_image (wrapper->target,
						image_out, image_extra);
}

void
_cairo_surface_wrapper_release_source_image (cairo_surface_wrapper_t *wrapper,
					     cairo_image_surface_t  *image,
					     void                   *image_extra)
{
    _cairo_surface_release_source_image (wrapper->target, image, image_extra);
}

cairo_status_t
_cairo_surface_wrapper_paint (cairo_surface_wrapper_t *wrapper,
			      cairo_operator_t	 op,
			      const cairo_pattern_t *source,
			      cairo_clip_t	    *clip)
{
    cairo_status_t status;
    cairo_clip_t clip_copy, *dev_clip = clip;
    cairo_pattern_union_t source_copy;

    if (unlikely (wrapper->target->status))
	return wrapper->target->status;

    if (clip && clip->all_clipped)
	return CAIRO_STATUS_SUCCESS;

    if (_cairo_surface_wrapper_needs_device_transform (wrapper)) {
	if (clip != NULL) {
	    status = _cairo_clip_init_copy_transformed (&clip_copy, clip,
							&wrapper->target->device_transform);
	    if (unlikely (status))
		goto FINISH;

	    dev_clip = &clip_copy;
	}

	_copy_transformed_pattern (&source_copy.base, source, &wrapper->target->device_transform_inverse);
	source = &source_copy.base;
    }

    status = _cairo_surface_paint (wrapper->target, op, source, dev_clip);

  FINISH:
    if (dev_clip != clip)
	_cairo_clip_reset (dev_clip);
    return status;
}

cairo_status_t
_cairo_surface_wrapper_mask (cairo_surface_wrapper_t *wrapper,
			     cairo_operator_t	 op,
			     const cairo_pattern_t *source,
			     const cairo_pattern_t *mask,
			     cairo_clip_t	    *clip)
{
    cairo_status_t status;
    cairo_clip_t clip_copy, *dev_clip = clip;
    cairo_pattern_union_t source_copy;
    cairo_pattern_union_t mask_copy;

    if (unlikely (wrapper->target->status))
	return wrapper->target->status;

    if (clip && clip->all_clipped)
	return CAIRO_STATUS_SUCCESS;

    if (_cairo_surface_wrapper_needs_device_transform (wrapper)) {
	if (clip != NULL) {
	    status = _cairo_clip_init_copy_transformed (&clip_copy, clip,
							&wrapper->target->device_transform);
	    if (unlikely (status))
		goto FINISH;

	    dev_clip = &clip_copy;
	}

	_copy_transformed_pattern (&source_copy.base, source, &wrapper->target->device_transform_inverse);
	source = &source_copy.base;

	_copy_transformed_pattern (&mask_copy.base, mask, &wrapper->target->device_transform_inverse);
	mask = &mask_copy.base;
    }

    status = _cairo_surface_mask (wrapper->target, op, source, mask, dev_clip);

  FINISH:
    if (dev_clip != clip)
	_cairo_clip_reset (dev_clip);
    return status;
}

cairo_status_t
_cairo_surface_wrapper_stroke (cairo_surface_wrapper_t *wrapper,
			       cairo_operator_t		 op,
			       const cairo_pattern_t	*source,
			       cairo_path_fixed_t	*path,
			       const cairo_stroke_style_t	*stroke_style,
			       const cairo_matrix_t		*ctm,
			       const cairo_matrix_t		*ctm_inverse,
			       double			 tolerance,
			       cairo_antialias_t	 antialias,
			       cairo_clip_t		*clip)
{
    cairo_status_t status;
    cairo_path_fixed_t path_copy, *dev_path = path;
    cairo_clip_t clip_copy, *dev_clip = clip;
    cairo_matrix_t dev_ctm = *ctm;
    cairo_matrix_t dev_ctm_inverse = *ctm_inverse;
    cairo_pattern_union_t source_copy;

    if (unlikely (wrapper->target->status))
	return wrapper->target->status;

    if (clip && clip->all_clipped)
	return CAIRO_STATUS_SUCCESS;

    if (_cairo_surface_wrapper_needs_device_transform (wrapper)) {
	status = _cairo_path_fixed_init_copy (&path_copy, dev_path);
	if (unlikely (status))
	    goto FINISH;

	_cairo_path_fixed_transform (&path_copy, &wrapper->target->device_transform);
	dev_path = &path_copy;

	if (clip != NULL) {
	    status = _cairo_clip_init_copy_transformed (&clip_copy, clip,
							&wrapper->target->device_transform);
	    if (unlikely (status))
		goto FINISH;

	    dev_clip = &clip_copy;
	}

	cairo_matrix_multiply (&dev_ctm, &dev_ctm, &wrapper->target->device_transform);
	cairo_matrix_multiply (&dev_ctm_inverse,
			       &wrapper->target->device_transform_inverse,
			       &dev_ctm_inverse);

	_copy_transformed_pattern (&source_copy.base, source, &wrapper->target->device_transform_inverse);
	source = &source_copy.base;
    } else {
	if (clip != NULL) {
	    dev_clip = &clip_copy;
	    _cairo_clip_init_copy (&clip_copy, clip);
	}
    }

    status = _cairo_surface_stroke (wrapper->target, op, source,
				    dev_path, stroke_style,
				    &dev_ctm, &dev_ctm_inverse,
				    tolerance, antialias,
				    dev_clip);

 FINISH:
    if (dev_path != path)
	_cairo_path_fixed_fini (dev_path);
    if (dev_clip != clip)
	_cairo_clip_reset (dev_clip);
    return status;
}

cairo_status_t
_cairo_surface_wrapper_fill_stroke (cairo_surface_wrapper_t *wrapper,
				    cairo_operator_t	     fill_op,
				    const cairo_pattern_t   *fill_source,
				    cairo_fill_rule_t	     fill_rule,
				    double		     fill_tolerance,
				    cairo_antialias_t	     fill_antialias,
				    cairo_path_fixed_t	    *path,
				    cairo_operator_t	     stroke_op,
				    const cairo_pattern_t   *stroke_source,
				    const cairo_stroke_style_t    *stroke_style,
				    const cairo_matrix_t	    *stroke_ctm,
				    const cairo_matrix_t	    *stroke_ctm_inverse,
				    double		     stroke_tolerance,
				    cairo_antialias_t	     stroke_antialias,
				    cairo_clip_t	    *clip)
{
    cairo_status_t status;
    cairo_path_fixed_t path_copy, *dev_path = path;
    cairo_clip_t clip_copy, *dev_clip = clip;
    cairo_matrix_t dev_ctm = *stroke_ctm;
    cairo_matrix_t dev_ctm_inverse = *stroke_ctm_inverse;
    cairo_pattern_union_t stroke_source_copy;
    cairo_pattern_union_t fill_source_copy;

    if (unlikely (wrapper->target->status))
	return wrapper->target->status;

    if (clip && clip->all_clipped)
	return CAIRO_STATUS_SUCCESS;

    if (_cairo_surface_wrapper_needs_device_transform (wrapper)) {
	status = _cairo_path_fixed_init_copy (&path_copy, dev_path);
	if (unlikely (status))
	    goto FINISH;

	_cairo_path_fixed_transform (&path_copy, &wrapper->target->device_transform);
	dev_path = &path_copy;

	if (clip != NULL) {
	    status = _cairo_clip_init_copy_transformed (&clip_copy, clip,
							&wrapper->target->device_transform);
	    if (unlikely (status))
		goto FINISH;

	    dev_clip = &clip_copy;
	}

	cairo_matrix_multiply (&dev_ctm, &dev_ctm, &wrapper->target->device_transform);
	cairo_matrix_multiply (&dev_ctm_inverse,
			       &wrapper->target->device_transform_inverse,
			       &dev_ctm_inverse);

	_copy_transformed_pattern (&stroke_source_copy.base, stroke_source, &wrapper->target->device_transform_inverse);
	stroke_source = &stroke_source_copy.base;

	_copy_transformed_pattern (&fill_source_copy.base, fill_source, &wrapper->target->device_transform_inverse);
	fill_source = &fill_source_copy.base;
    } else {
	if (clip != NULL) {
	    dev_clip = &clip_copy;
	    _cairo_clip_init_copy (&clip_copy, clip);
	}
    }

    status = _cairo_surface_fill_stroke (wrapper->target,
					 fill_op, fill_source, fill_rule,
					 fill_tolerance, fill_antialias,
					 dev_path,
					 stroke_op, stroke_source,
					 stroke_style,
					 &dev_ctm, &dev_ctm_inverse,
					 stroke_tolerance, stroke_antialias,
					 dev_clip);

  FINISH:
    if (dev_path != path)
	_cairo_path_fixed_fini (dev_path);
    if (dev_clip != clip)
	_cairo_clip_reset (dev_clip);
    return status;
}

cairo_status_t
_cairo_surface_wrapper_fill (cairo_surface_wrapper_t	*wrapper,
			     cairo_operator_t	 op,
			     const cairo_pattern_t *source,
			     cairo_path_fixed_t	*path,
			     cairo_fill_rule_t	 fill_rule,
			     double		 tolerance,
			     cairo_antialias_t	 antialias,
			     cairo_clip_t	*clip)
{
    cairo_status_t status;
    cairo_path_fixed_t path_copy, *dev_path = path;
    cairo_clip_t clip_copy, *dev_clip = clip;
    cairo_pattern_union_t source_copy;

    if (unlikely (wrapper->target->status))
	return wrapper->target->status;

    if (clip && clip->all_clipped)
	return CAIRO_STATUS_SUCCESS;

    if (_cairo_surface_wrapper_needs_device_transform (wrapper)) {
	status = _cairo_path_fixed_init_copy (&path_copy, dev_path);
	if (unlikely (status))
	    goto FINISH;

	_cairo_path_fixed_transform (&path_copy, &wrapper->target->device_transform);
	dev_path = &path_copy;

	if (clip != NULL) {
	    status = _cairo_clip_init_copy_transformed (&clip_copy, clip,
							&wrapper->target->device_transform);
	    if (unlikely (status))
		goto FINISH;

	    dev_clip = &clip_copy;
	}

	_copy_transformed_pattern (&source_copy.base, source, &wrapper->target->device_transform_inverse);
	source = &source_copy.base;
    } else {
	if (clip != NULL) {
	    dev_clip = &clip_copy;
	    _cairo_clip_init_copy (&clip_copy, clip);
	}
    }

    status = _cairo_surface_fill (wrapper->target, op, source,
				  dev_path, fill_rule,
				  tolerance, antialias,
				  dev_clip);

 FINISH:
    if (dev_path != path)
	_cairo_path_fixed_fini (dev_path);
    if (dev_clip != clip)
	_cairo_clip_reset (dev_clip);
    return status;
}

cairo_status_t
_cairo_surface_wrapper_show_text_glyphs (cairo_surface_wrapper_t *wrapper,
					 cairo_operator_t	     op,
					 const cairo_pattern_t	    *source,
					 const char		    *utf8,
					 int			     utf8_len,
					 cairo_glyph_t		    *glyphs,
					 int			     num_glyphs,
					 const cairo_text_cluster_t *clusters,
					 int			     num_clusters,
					 cairo_text_cluster_flags_t  cluster_flags,
					 cairo_scaled_font_t	    *scaled_font,
					 cairo_clip_t		    *clip)
{
    cairo_status_t status;
    cairo_clip_t clip_copy, *dev_clip = clip;
    cairo_glyph_t *dev_glyphs = glyphs;
    cairo_pattern_union_t source_copy;

    if (unlikely (wrapper->target->status))
	return wrapper->target->status;

    if (glyphs == NULL || num_glyphs == 0)
	return CAIRO_STATUS_SUCCESS;

    if (clip && clip->all_clipped)
	return CAIRO_STATUS_SUCCESS;

    if (_cairo_surface_wrapper_needs_device_transform (wrapper)) {
	int i;

	if (clip != NULL) {
	    status = _cairo_clip_init_copy_transformed (&clip_copy, clip,
							&wrapper->target->device_transform);
	    if (unlikely (status))
		goto FINISH;

	    dev_clip = &clip_copy;
	}

	dev_glyphs = _cairo_malloc_ab (num_glyphs, sizeof (cairo_glyph_t));
	if (dev_glyphs == NULL) {
	    status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
	    goto FINISH;
	}

	for (i = 0; i < num_glyphs; i++) {
	    dev_glyphs[i] = glyphs[i];
	    cairo_matrix_transform_point (&wrapper->target->device_transform,
					  &dev_glyphs[i].x,
					  &dev_glyphs[i].y);
	}

	_copy_transformed_pattern (&source_copy.base, source, &wrapper->target->device_transform_inverse);
	source = &source_copy.base;
    } else {
	if (clip != NULL) {
	    dev_clip = &clip_copy;
	    _cairo_clip_init_copy (&clip_copy, clip);
	}
    }

    status = _cairo_surface_show_text_glyphs (wrapper->target, op, source,
					      utf8, utf8_len,
					      dev_glyphs, num_glyphs,
					      clusters, num_clusters,
					      cluster_flags,
					      scaled_font,
					      dev_clip);
 FINISH:
    if (dev_clip != clip)
	_cairo_clip_reset (dev_clip);
    if (dev_glyphs != glyphs)
	tb_free (dev_glyphs);
    return status;
}

cairo_surface_t *
_cairo_surface_wrapper_create_similar (cairo_surface_wrapper_t *wrapper,
				       cairo_content_t	content,
				       int		width,
				       int		height)
{
    return _cairo_surface_create_similar_scratch (wrapper->target,
						  content, width, height);
}

cairo_bool_t
_cairo_surface_wrapper_get_extents (cairo_surface_wrapper_t *wrapper,
				    cairo_rectangle_int_t   *extents)
{
    return _cairo_surface_get_extents (wrapper->target, extents);
}

void
_cairo_surface_wrapper_get_font_options (cairo_surface_wrapper_t    *wrapper,
					 cairo_font_options_t	    *options)
{
    cairo_surface_get_font_options (wrapper->target, options);
}

cairo_surface_t *
_cairo_surface_wrapper_snapshot (cairo_surface_wrapper_t *wrapper)
{
    return _cairo_surface_snapshot (wrapper->target);
}

cairo_bool_t
_cairo_surface_wrapper_has_show_text_glyphs (cairo_surface_wrapper_t *wrapper)
{
    return cairo_surface_has_show_text_glyphs (wrapper->target);
}

void
_cairo_surface_wrapper_init (cairo_surface_wrapper_t *wrapper,
			     cairo_surface_t *target)
{
    wrapper->target = cairo_surface_reference (target);
}

void
_cairo_surface_wrapper_fini (cairo_surface_wrapper_t *wrapper)
{
    cairo_surface_destroy (wrapper->target);
}
