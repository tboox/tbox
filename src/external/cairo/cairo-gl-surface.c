/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2009 Eric Anholt
 * Copyright © 2009 Chris Wilson
 * Copyright © 2005 Red Hat, Inc
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
 *	Carl Worth <cworth@cworth.org>
 */

#include "cairoint.h"

#include "cairo-composite-rectangles-private.h"
#include "cairo-error-private.h"
#include "cairo-gl-private.h"

static cairo_int_status_t
_cairo_gl_surface_fill_rectangles (void			   *abstract_surface,
				   cairo_operator_t	    op,
				   const cairo_color_t     *color,
				   cairo_rectangle_int_t   *rects,
				   int			    num_rects);

#define BIAS .375

static inline float
int_as_float (uint32_t val)
{
    union fi {
	float f;
	uint32_t u;
    } fi;

    fi.u = val;
    return fi.f;
}

static cairo_bool_t _cairo_surface_is_gl (cairo_surface_t *surface)
{
    return surface->backend == &_cairo_gl_surface_backend;
}

static void
_gl_lock (void *device)
{
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) device;

    ctx->acquire (ctx);
}

static void
_gl_unlock (void *device)
{
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) device;

    ctx->release (ctx);
    ctx->current_target = NULL;
}

static void
_gl_destroy (void *device)
{
    cairo_gl_context_t *ctx = device;
    cairo_scaled_font_t *scaled_font, *next_scaled_font;
    int n;

    ctx->destroy (ctx);

    cairo_list_foreach_entry_safe (scaled_font,
				   next_scaled_font,
				   cairo_scaled_font_t,
				   &ctx->fonts,
				   link)
    {
	_cairo_scaled_font_revoke_ownership (scaled_font);
    }

    for (n = 0; n < ARRAY_LENGTH (ctx->glyph_cache); n++)
	_cairo_gl_glyph_cache_fini (ctx, &ctx->glyph_cache[n]);

    tb_free (ctx);
}

static const cairo_device_backend_t _cairo_gl_device_backend = {
    CAIRO_DEVICE_TYPE_GL,

    _gl_lock,
    _gl_unlock,

    NULL, /* flush */
    NULL, /* finish */
    _gl_destroy,
};

cairo_status_t
_cairo_gl_context_init (cairo_gl_context_t *ctx)
{
    int n;

    _cairo_device_init (&ctx->base, &_cairo_gl_device_backend);

    memset (ctx->glyph_cache, 0, sizeof (ctx->glyph_cache));
    cairo_list_init (&ctx->fonts);

    if (glewInit () != GLEW_OK)
	return _cairo_error (CAIRO_STATUS_INVALID_FORMAT); /* XXX */

    if (! GLEW_EXT_framebuffer_object ||
	! GLEW_ARB_texture_env_combine ||
	! GLEW_EXT_bgra)
    {
	fprintf (stderr,
		 "Required GL extensions not available:\n");
	if (! GLEW_EXT_framebuffer_object)
	    fprintf (stderr, "    GL_EXT_framebuffer_object\n");
	if (! GLEW_ARB_texture_env_combine)
	    fprintf (stderr, "    GL_ARB_texture_env_combine\n");
	/* EXT_bgra is used in two places:
	 * - draw_image to upload common pixman formats without hand-swizzling.
	 * - get_image to download common pixman formats without hand-swizzling.
	 */
	if (! GLEW_EXT_bgra)
	    fprintf (stderr, "    GL_EXT_bgra\n");

	return _cairo_error (CAIRO_STATUS_INVALID_FORMAT); /* XXX */
    }

    if (! GLEW_ARB_texture_non_power_of_two &&
	! GLEW_ARB_texture_rectangle ) {
	fprintf (stderr,
		 "Required GL extensions not available:\n");
	fprintf (stderr, "    GL_ARB_texture_non_power_of_two, GL_ARB_texture_rectangle\n");
    }

    if (!GLEW_ARB_texture_non_power_of_two)
	ctx->tex_target = GL_TEXTURE_RECTANGLE_EXT;
    else
	ctx->tex_target = GL_TEXTURE_2D;


    if (GLEW_VERSION_2_0 ||
	(GLEW_ARB_fragment_shader &&
	 GLEW_ARB_vertex_shader &&
	 GLEW_ARB_shader_objects))
    {
	ctx->using_glsl = TRUE;
    }

    init_shader_program (&ctx->fill_rectangles_shader);

    /* Set up the dummy texture for tex_env_combine with constant color. */
    glGenTextures (1, &ctx->dummy_tex);
    glBindTexture (ctx->tex_target, ctx->dummy_tex);
    glTexImage2D (ctx->tex_target, 0, GL_RGBA, 1, 1, 0,
		  GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    /* PBO for any sort of texture upload */
    glGenBuffersARB (1, &ctx->texture_load_pbo);
    glGenBuffersARB (1, &ctx->vbo);

    ctx->max_framebuffer_size = 0;
    glGetIntegerv (GL_MAX_RENDERBUFFER_SIZE, &ctx->max_framebuffer_size);
    ctx->max_texture_size = 0;
    glGetIntegerv (GL_MAX_TEXTURE_SIZE, &ctx->max_texture_size);

    for (n = 0; n < ARRAY_LENGTH (ctx->glyph_cache); n++)
	_cairo_gl_glyph_cache_init (&ctx->glyph_cache[n]);

    return CAIRO_STATUS_SUCCESS;
}

cairo_bool_t
_cairo_gl_get_image_format_and_type (pixman_format_code_t pixman_format,
				     GLenum *internal_format, GLenum *format,
				     GLenum *type, cairo_bool_t *has_alpha)
{
    *has_alpha = TRUE;

    switch (pixman_format) {
    case PIXMAN_a8r8g8b8:
	*internal_format = GL_RGBA;
	*format = GL_BGRA;
	*type = GL_UNSIGNED_INT_8_8_8_8_REV;
	return TRUE;
    case PIXMAN_x8r8g8b8:
	*internal_format = GL_RGB;
	*format = GL_BGRA;
	*type = GL_UNSIGNED_INT_8_8_8_8_REV;
	*has_alpha = FALSE;
	return TRUE;
    case PIXMAN_a8b8g8r8:
	*internal_format = GL_RGBA;
	*format = GL_RGBA;
	*type = GL_UNSIGNED_INT_8_8_8_8_REV;
	return TRUE;
    case PIXMAN_x8b8g8r8:
	*internal_format = GL_RGB;
	*format = GL_RGBA;
	*type = GL_UNSIGNED_INT_8_8_8_8_REV;
	*has_alpha = FALSE;
	return TRUE;
    case PIXMAN_b8g8r8a8:
	*internal_format = GL_RGBA;
	*format = GL_BGRA;
	*type = GL_UNSIGNED_INT_8_8_8_8;
	return TRUE;
    case PIXMAN_b8g8r8x8:
	*internal_format = GL_RGB;
	*format = GL_BGRA;
	*type = GL_UNSIGNED_INT_8_8_8_8;
	*has_alpha = FALSE;
	return TRUE;
    case PIXMAN_r8g8b8:
	*internal_format = GL_RGB;
	*format = GL_RGB;
	*type = GL_UNSIGNED_BYTE;
	return TRUE;
    case PIXMAN_b8g8r8:
	*internal_format = GL_RGB;
	*format = GL_BGR;
	*type = GL_UNSIGNED_BYTE;
	return TRUE;
    case PIXMAN_r5g6b5:
	*internal_format = GL_RGB;
	*format = GL_RGB;
	*type = GL_UNSIGNED_SHORT_5_6_5;
	return TRUE;
    case PIXMAN_b5g6r5:
	*internal_format = GL_RGB;
	*format = GL_RGB;
	*type = GL_UNSIGNED_SHORT_5_6_5_REV;
	return TRUE;
    case PIXMAN_a1r5g5b5:
	*internal_format = GL_RGBA;
	*format = GL_BGRA;
	*type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
	return TRUE;
    case PIXMAN_x1r5g5b5:
	*internal_format = GL_RGB;
	*format = GL_BGRA;
	*type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
	*has_alpha = FALSE;
	return TRUE;
    case PIXMAN_a1b5g5r5:
	*internal_format = GL_RGBA;
	*format = GL_RGBA;
	*type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
	return TRUE;
    case PIXMAN_x1b5g5r5:
	*internal_format = GL_RGB;
	*format = GL_RGBA;
	*type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
	*has_alpha = FALSE;
	return TRUE;
    case PIXMAN_a8:
	*internal_format = GL_ALPHA;
	*format = GL_ALPHA;
	*type = GL_UNSIGNED_BYTE;
	return TRUE;

    case PIXMAN_a2b10g10r10:
    case PIXMAN_x2b10g10r10:
    case PIXMAN_a4r4g4b4:
    case PIXMAN_x4r4g4b4:
    case PIXMAN_a4b4g4r4:
    case PIXMAN_x4b4g4r4:
    case PIXMAN_r3g3b2:
    case PIXMAN_b2g3r3:
    case PIXMAN_a2r2g2b2:
    case PIXMAN_a2b2g2r2:
    case PIXMAN_c8:
    case PIXMAN_x4a4:
    /* case PIXMAN_x4c4: */
    case PIXMAN_x4g4:
    case PIXMAN_a4:
    case PIXMAN_r1g2b1:
    case PIXMAN_b1g2r1:
    case PIXMAN_a1r1g1b1:
    case PIXMAN_a1b1g1r1:
    case PIXMAN_c4:
    case PIXMAN_g4:
    case PIXMAN_a1:
    case PIXMAN_g1:
    case PIXMAN_yuy2:
    case PIXMAN_yv12:
    case PIXMAN_x2r10g10b10:
    case PIXMAN_a2r10g10b10:
    default:
	return FALSE;
    }
}

void
_cairo_gl_set_destination (cairo_gl_surface_t *surface)
{
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) surface->base.device;

    if (ctx->current_target != surface) {
	ctx->current_target = surface;

	if (surface->fb) {
	    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, surface->fb);
	    glDrawBuffer (GL_COLOR_ATTACHMENT0_EXT);
	    glReadBuffer (GL_COLOR_ATTACHMENT0_EXT);
	} else {
	    ctx->make_current (ctx, surface);
	    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	    glDrawBuffer (GL_BACK_LEFT);
	    glReadBuffer (GL_BACK_LEFT);
	}
    }

    glViewport (0, 0, surface->width, surface->height);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
    if (surface->fb)
	glOrtho (0, surface->width, 0, surface->height, -1.0, 1.0);
    else
	glOrtho (0, surface->width, surface->height, 0, -1.0, 1.0);

    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
}

cairo_bool_t
_cairo_gl_operator_is_supported (cairo_operator_t op)
{
    return op < CAIRO_OPERATOR_SATURATE;
}

void
_cairo_gl_set_operator (cairo_gl_surface_t *dst, cairo_operator_t op,
			cairo_bool_t component_alpha)
{
    struct {
	GLenum src;
	GLenum dst;
    } blend_factors[] = {
	{ GL_ZERO, GL_ZERO }, /* Clear */
	{ GL_ONE, GL_ZERO }, /* Source */
	{ GL_ONE, GL_ONE_MINUS_SRC_ALPHA }, /* Over */
	{ GL_DST_ALPHA, GL_ZERO }, /* In */
	{ GL_ONE_MINUS_DST_ALPHA, GL_ZERO }, /* Out */
	{ GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA }, /* Atop */

	{ GL_ZERO, GL_ONE }, /* Dest */
	{ GL_ONE_MINUS_DST_ALPHA, GL_ONE }, /* DestOver */
	{ GL_ZERO, GL_SRC_ALPHA }, /* DestIn */
	{ GL_ZERO, GL_ONE_MINUS_SRC_ALPHA }, /* DestOut */
	{ GL_ONE_MINUS_DST_ALPHA, GL_SRC_ALPHA }, /* DestAtop */

	{ GL_ONE_MINUS_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA }, /* Xor */
	{ GL_ONE, GL_ONE }, /* Add */
    };
    GLenum src_factor, dst_factor;

    assert (op < ARRAY_LENGTH (blend_factors));

    src_factor = blend_factors[op].src;
    dst_factor = blend_factors[op].dst;

    /* Even when the user requests CAIRO_CONTENT_COLOR, we use GL_RGBA
     * due to texture filtering of GL_CLAMP_TO_BORDER.  So fix those
     * bits in that case.
     */
    if (dst->base.content == CAIRO_CONTENT_COLOR) {
	if (src_factor == GL_ONE_MINUS_DST_ALPHA)
	    src_factor = GL_ZERO;
	if (src_factor == GL_DST_ALPHA)
	    src_factor = GL_ONE;
    }

    if (component_alpha) {
	if (dst_factor == GL_ONE_MINUS_SRC_ALPHA)
	    dst_factor = GL_ONE_MINUS_SRC_COLOR;
	if (dst_factor == GL_SRC_ALPHA)
	    dst_factor = GL_SRC_COLOR;
    }

    glEnable (GL_BLEND);
    glBlendFunc (src_factor, dst_factor);
}

static void
_cairo_gl_set_texture_surface (int tex_unit, GLuint tex,
			       cairo_surface_attributes_t *attributes,
			       GLint tex_target)
{

    if (tex_target == GL_TEXTURE_RECTANGLE_EXT) {
	assert (attributes->extend != CAIRO_EXTEND_REPEAT &&
		attributes->extend != CAIRO_EXTEND_REFLECT);
    }

    glActiveTexture (GL_TEXTURE0 + tex_unit);
    glBindTexture (tex_target, tex);
    switch (attributes->extend) {
    case CAIRO_EXTEND_NONE:
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	break;
    case CAIRO_EXTEND_PAD:
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	break;
    case CAIRO_EXTEND_REPEAT:
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_T, GL_REPEAT);
	break;
    case CAIRO_EXTEND_REFLECT:
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri (tex_target, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	break;
    }
    switch (attributes->filter) {
    case CAIRO_FILTER_FAST:
    case CAIRO_FILTER_NEAREST:
	glTexParameteri (tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	break;
    case CAIRO_FILTER_GOOD:
    case CAIRO_FILTER_BEST:
    case CAIRO_FILTER_BILINEAR:
	glTexParameteri (tex_target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri (tex_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	break;
    default:
    case CAIRO_FILTER_GAUSSIAN:
	ASSERT_NOT_REACHED;
    }
    glEnable (tex_target);
}

void
_cairo_gl_surface_init (cairo_device_t *device,
			cairo_gl_surface_t *surface,
			cairo_content_t content,
			int width, int height)
{
    _cairo_surface_init (&surface->base,
			 &_cairo_gl_surface_backend,
			 device,
			 content);

    surface->width = width;
    surface->height = height;
}

static cairo_surface_t *
_cairo_gl_surface_create_scratch (cairo_gl_context_t   *ctx,
				  cairo_content_t	content,
				  int			width,
				  int			height)
{
    cairo_gl_surface_t *surface;
    GLenum err, format;
    cairo_status_t status;

    assert (width <= ctx->max_framebuffer_size && height <= ctx->max_framebuffer_size);

    surface = tb_calloc (1, sizeof (cairo_gl_surface_t));
    if (unlikely (surface == NULL))
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_NO_MEMORY));

    _cairo_gl_surface_init (&ctx->base, surface, content, width, height);

    /* adjust the texture size after setting our real extents */
    if (width < 1)
	width = 1;
    if (height < 1)
	height = 1;

    switch (content) {
    default:
	ASSERT_NOT_REACHED;
    case CAIRO_CONTENT_COLOR_ALPHA:
	format = GL_RGBA;
	break;
    case CAIRO_CONTENT_ALPHA:
	/* We want to be trying GL_ALPHA framebuffer objects here. */
	format = GL_RGBA;
	break;
    case CAIRO_CONTENT_COLOR:
	/* GL_RGB is almost what we want here -- sampling 1 alpha when
	 * texturing, using 1 as destination alpha factor in blending,
	 * etc.  However, when filtering with GL_CLAMP_TO_BORDER, the
	 * alpha channel of the border color will also be clamped to
	 * 1, when we actually want the border color we explicitly
	 * specified.  So, we have to store RGBA, and fill the alpha
	 * channel with 1 when blending.
	 */
	format = GL_RGBA;
	break;
    }

    /* Create the texture used to store the surface's data. */
    glGenTextures (1, &surface->tex);
    glBindTexture (ctx->tex_target, surface->tex);
    glTexParameteri (ctx->tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri (ctx->tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D (ctx->tex_target, 0, format, width, height, 0,
		  format, GL_UNSIGNED_BYTE, NULL);

    /* Create a framebuffer object wrapping the texture so that we can render
     * to it.
     */
    glGenFramebuffersEXT (1, &surface->fb);
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, surface->fb);
    glFramebufferTexture2DEXT (GL_FRAMEBUFFER_EXT,
			       GL_COLOR_ATTACHMENT0_EXT,
			       ctx->tex_target,
			       surface->tex,
			       0);
    ctx->current_target = NULL;

    while ((err = glGetError ())) {
	fprintf (stderr, "GL error in surface create: 0x%08x\n", err);
    }

    status = glCheckFramebufferStatusEXT (GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	fprintf (stderr, "destination is framebuffer incomplete\n");

    return &surface->base;
}

cairo_status_t
_cairo_gl_surface_clear (cairo_gl_surface_t *surface)
{
    cairo_gl_context_t *ctx;
    cairo_status_t status;

    status = _cairo_gl_context_acquire (surface->base.device, &ctx);
    if (unlikely (status))
	return status;

    _cairo_gl_set_destination (surface);
    if (surface->base.content == CAIRO_CONTENT_COLOR)
	glClearColor (0.0, 0.0, 0.0, 1.0);
    else
	glClearColor (0.0, 0.0, 0.0, 0.0);
    glClear (GL_COLOR_BUFFER_BIT);
    _cairo_gl_context_release (ctx);

    surface->base.is_clear = TRUE;

    return CAIRO_STATUS_SUCCESS;
}

cairo_surface_t *
cairo_gl_surface_create (cairo_device_t		*abstract_device,
			 cairo_content_t	 content,
			 int			 width,
			 int			 height)
{
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) abstract_device;
    cairo_gl_surface_t *surface;
    cairo_status_t status;

    if (! CAIRO_CONTENT_VALID (content))
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_INVALID_CONTENT));

    if (ctx == NULL) {
	return cairo_image_surface_create (_cairo_format_from_content (content),
					   width, height);
    }

    if (ctx->base.backend->type != CAIRO_DEVICE_TYPE_GL)
	return _cairo_surface_create_in_error (_cairo_error (CAIRO_STATUS_SURFACE_TYPE_MISMATCH));

    status = _cairo_gl_context_acquire (abstract_device, &ctx);
    if (unlikely (status))
	return _cairo_surface_create_in_error (status);

    surface = (cairo_gl_surface_t *)
	_cairo_gl_surface_create_scratch (ctx, content, width, height);
    if (unlikely (surface->base.status)) {
	_cairo_gl_context_release (ctx);
	return &surface->base;
    }

    /* Cairo surfaces start out initialized to transparent (black) */
    status = _cairo_gl_surface_clear (surface);
    if (unlikely (status)) {
	cairo_surface_destroy (&surface->base);
	_cairo_gl_context_release (ctx);
	return _cairo_surface_create_in_error (status);
    }

    _cairo_gl_context_release (ctx);

    return &surface->base;
}
slim_hidden_def (cairo_gl_surface_create);

void
cairo_gl_surface_set_size (cairo_surface_t *abstract_surface,
			   int              width,
			   int              height)
{
    cairo_gl_surface_t *surface = (cairo_gl_surface_t *) abstract_surface;
    cairo_status_t status;

    if (unlikely (abstract_surface->status))
	return;

    if (! _cairo_surface_is_gl (abstract_surface) || surface->fb) {
	status = _cairo_surface_set_error (abstract_surface,
		                           CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return;
    }

    surface->width = width;
    surface->height = height;
}

int
cairo_gl_surface_get_width (cairo_surface_t *abstract_surface)
{
    cairo_gl_surface_t *surface = (cairo_gl_surface_t *) abstract_surface;

    if (! _cairo_surface_is_gl (abstract_surface))
	return 0;

    return surface->width;
}

int
cairo_gl_surface_get_height (cairo_surface_t *abstract_surface)
{
    cairo_gl_surface_t *surface = (cairo_gl_surface_t *) abstract_surface;

    if (! _cairo_surface_is_gl (abstract_surface))
	return 0;

    return surface->height;
}

void
cairo_gl_surface_swapbuffers (cairo_surface_t *abstract_surface)
{
    cairo_gl_surface_t *surface = (cairo_gl_surface_t *) abstract_surface;
    cairo_status_t status;

    if (unlikely (abstract_surface->status))
	return;

    if (! _cairo_surface_is_gl (abstract_surface)) {
	status = _cairo_surface_set_error (abstract_surface,
		                           CAIRO_STATUS_SURFACE_TYPE_MISMATCH);
	return;
    }

    if (! surface->fb) {
	cairo_gl_context_t *ctx;
        
        if (_cairo_gl_context_acquire (surface->base.device, &ctx))
            return;

	ctx->swap_buffers (ctx, surface);

        _cairo_gl_context_release (ctx);
    }
}

static cairo_surface_t *
_cairo_gl_surface_create_similar (void		 *abstract_surface,
				  cairo_content_t  content,
				  int		  width,
				  int		  height)
{
    cairo_surface_t *surface = abstract_surface;
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) surface->device;
    cairo_status_t status;

    if (width > ctx->max_framebuffer_size ||
	height > ctx->max_framebuffer_size)
    {
	return NULL;
    }

    status = _cairo_gl_context_acquire (surface->device, &ctx);
    if (unlikely (status))
	return _cairo_surface_create_in_error (status);

    surface = _cairo_gl_surface_create_scratch (ctx, content, width, height);
    _cairo_gl_context_release (ctx);

    return surface;
}

cairo_status_t
_cairo_gl_surface_draw_image (cairo_gl_surface_t *dst,
			      cairo_image_surface_t *src,
			      int src_x, int src_y,
			      int width, int height,
			      int dst_x, int dst_y)
{
    GLenum internal_format, format, type;
    cairo_bool_t has_alpha;
    cairo_image_surface_t *clone = NULL;
    cairo_gl_context_t *ctx;
    int cpp;
    cairo_status_t status = CAIRO_STATUS_SUCCESS;

    if (! _cairo_gl_get_image_format_and_type (src->pixman_format,
					       &internal_format,
					       &format,
					       &type,
					       &has_alpha))
    {
	cairo_bool_t is_supported;

	clone = _cairo_image_surface_coerce (src);
	if (unlikely (clone->base.status))
	    return clone->base.status;

	is_supported =
	    _cairo_gl_get_image_format_and_type (clone->pixman_format,
		                                 &internal_format,
						 &format,
						 &type,
						 &has_alpha);
	assert (is_supported);
	src = clone;
    }

    cpp = PIXMAN_FORMAT_BPP (src->pixman_format) / 8;

    status = _cairo_gl_context_acquire (dst->base.device, &ctx);
    if (unlikely (status))
	return status;

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei (GL_UNPACK_ROW_LENGTH, src->stride / cpp);
    if (dst->fb) {
	glBindTexture (ctx->tex_target, dst->tex);
	glTexParameteri (ctx->tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (ctx->tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexSubImage2D (ctx->tex_target, 0,
			 dst_x, dst_y, width, height,
			 format, type,
			 src->data + src_y * src->stride + src_x * cpp);

	/* If we just treated some rgb-only data as rgba, then we have to
	 * go back and fix up the alpha channel where we filled in this
	 * texture data.
	 */
	if (!has_alpha) {
	    cairo_rectangle_int_t rect;
	    cairo_color_t color;

	    rect.x = dst_x;
	    rect.y = dst_y;
	    rect.width = width;
	    rect.height = height;

	    color.red = 0.0;
	    color.green = 0.0;
	    color.blue = 0.0;
	    color.alpha = 1.0;

	    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	    _cairo_gl_surface_fill_rectangles (dst,
					       CAIRO_OPERATOR_SOURCE,
					       &color,
					       &rect, 1);
	    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
    } else {
	GLuint tex;
	GLfloat vertices[8], texcoords[8];

	if (ctx->using_glsl) {
	    cairo_gl_shader_program_t *program;

	    status = _cairo_gl_get_program (ctx,
					    CAIRO_GL_SHADER_SOURCE_TEXTURE,
					    CAIRO_GL_SHADER_MASK_NONE,
					    CAIRO_GL_SHADER_IN_NORMAL,
					    &program);
	    if (_cairo_status_is_error (status)) {
		_cairo_gl_context_release (ctx);
		goto fail;
	    }

	    _cairo_gl_use_program (program);
	} else {
	    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

	_cairo_gl_set_destination (dst);

	glGenTextures (1, &tex);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (ctx->tex_target, tex);
	glTexParameteri (ctx->tex_target, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (ctx->tex_target, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D (ctx->tex_target, 0, internal_format, width, height, 0,
		      format, type, src->data + src_y * src->stride + src_x * cpp);

	glEnable (ctx->tex_target);
	glDisable (GL_BLEND);

	vertices[0] = dst_x;
	vertices[1] = dst_y;
	vertices[2] = dst_x + width;
	vertices[3] = dst_y;
	vertices[4] = dst_x + width;
	vertices[5] = dst_y + height;
	vertices[6] = dst_x;
	vertices[7] = dst_y + height;

	if (ctx->tex_target != GL_TEXTURE_RECTANGLE_EXT) {
	    texcoords[0] = 0;
	    texcoords[1] = 0;
	    texcoords[2] = 1;
	    texcoords[3] = 0;
	    texcoords[4] = 1;
	    texcoords[5] = 1;
	    texcoords[6] = 0;
	    texcoords[7] = 1;
	} else {
	    texcoords[0] = 0;
	    texcoords[1] = 0;
	    texcoords[2] = width;
	    texcoords[3] = 0;
	    texcoords[4] = width;
	    texcoords[5] = height;
	    texcoords[6] = 0;
	    texcoords[7] = height;
	}

        glVertexPointer (2, GL_FLOAT, sizeof (GLfloat) * 2, vertices);
	glEnableClientState (GL_VERTEX_ARRAY);

	glClientActiveTexture (GL_TEXTURE0);
	glTexCoordPointer (2, GL_FLOAT, sizeof (GLfloat) * 2, texcoords);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);

	glDrawArrays (GL_QUADS, 0, 4);

	glDisableClientState (GL_COLOR_ARRAY);
	glDisableClientState (GL_VERTEX_ARRAY);
	glDisableClientState (GL_TEXTURE_COORD_ARRAY);

	if (ctx->using_glsl)
	    _cairo_gl_use_program (NULL);
	glDeleteTextures (1, &tex);
	glDisable (ctx->tex_target);
    }

fail:
    glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);

    _cairo_gl_context_release (ctx);

    if (clone)
        cairo_surface_destroy (&clone->base);

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_gl_surface_get_image (cairo_gl_surface_t      *surface,
			     cairo_rectangle_int_t   *interest,
			     cairo_image_surface_t  **image_out,
			     cairo_rectangle_int_t   *rect_out)
{
    cairo_image_surface_t *image;
    cairo_gl_context_t *ctx;
    GLenum err;
    GLenum format, type;
    cairo_format_t cairo_format;
    unsigned int cpp;
    cairo_status_t status;

    /* Want to use a switch statement here but the compiler gets whiny. */
    if (surface->base.content == CAIRO_CONTENT_COLOR_ALPHA) {
	format = GL_BGRA;
	cairo_format = CAIRO_FORMAT_ARGB32;
	type = GL_UNSIGNED_INT_8_8_8_8_REV;
	cpp = 4;
    } else if (surface->base.content == CAIRO_CONTENT_COLOR) {
	format = GL_BGRA;
	cairo_format = CAIRO_FORMAT_RGB24;
	type = GL_UNSIGNED_INT_8_8_8_8_REV;
	cpp = 4;
    } else if (surface->base.content == CAIRO_CONTENT_ALPHA) {
	format = GL_ALPHA;
	cairo_format = CAIRO_FORMAT_A8;
	type = GL_UNSIGNED_BYTE;
	cpp = 1;
    } else {
	ASSERT_NOT_REACHED;
	return CAIRO_INT_STATUS_UNSUPPORTED;
    }

    image = (cairo_image_surface_t*)
	cairo_image_surface_create (cairo_format,
				    interest->width, interest->height);
    if (unlikely (image->base.status))
	return image->base.status;

    /* This is inefficient, as we'd rather just read the thing without making
     * it the destination.  But then, this is the fallback path, so let's not
     * fall back instead.
     */
    status = _cairo_gl_context_acquire (surface->base.device, &ctx);
    if (unlikely (status))
        return status;
    _cairo_gl_set_destination (surface);

    glPixelStorei (GL_PACK_ALIGNMENT, 1);
    glPixelStorei (GL_PACK_ROW_LENGTH, image->stride / cpp);
    if (surface->fb == 0 && GLEW_MESA_pack_invert)
	glPixelStorei (GL_PACK_INVERT_MESA, 1);
    glReadPixels (interest->x, interest->y,
		  interest->width, interest->height,
		  format, type, image->data);
    if (surface->fb == 0 && GLEW_MESA_pack_invert)
	glPixelStorei (GL_PACK_INVERT_MESA, 0);

    while ((err = glGetError ()))
	fprintf (stderr, "GL error 0x%08x\n", (int) err);

    _cairo_gl_context_release (ctx);

    *image_out = image;
    if (rect_out != NULL)
	*rect_out = *interest;
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_gl_surface_finish (void *abstract_surface)
{
    cairo_gl_surface_t *surface = abstract_surface;
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) surface->base.device;

    glDeleteFramebuffersEXT (1, &surface->fb);
    glDeleteTextures (1, &surface->tex);

    if (ctx->current_target == surface)
	ctx->current_target = NULL;

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_gl_surface_acquire_source_image (void		       *abstract_surface,
					cairo_image_surface_t **image_out,
					void		      **image_extra)
{
    cairo_gl_surface_t *surface = abstract_surface;
    cairo_rectangle_int_t extents;

    *image_extra = NULL;

    extents.x = extents.y = 0;
    extents.width = surface->width;
    extents.height = surface->height;
    return _cairo_gl_surface_get_image (surface, &extents, image_out, NULL);
}

static void
_cairo_gl_surface_release_source_image (void		      *abstract_surface,
					cairo_image_surface_t *image,
					void		      *image_extra)
{
    cairo_surface_destroy (&image->base);
}

static cairo_status_t
_cairo_gl_surface_acquire_dest_image (void		      *abstract_surface,
				      cairo_rectangle_int_t   *interest_rect,
				      cairo_image_surface_t  **image_out,
				      cairo_rectangle_int_t   *image_rect_out,
				      void		     **image_extra)
{
    cairo_gl_surface_t *surface = abstract_surface;

    *image_extra = NULL;
    return _cairo_gl_surface_get_image (surface, interest_rect, image_out,
					image_rect_out);
}

static void
_cairo_gl_surface_release_dest_image (void		      *abstract_surface,
				      cairo_rectangle_int_t   *interest_rect,
				      cairo_image_surface_t   *image,
				      cairo_rectangle_int_t   *image_rect,
				      void		      *image_extra)
{
    cairo_status_t status;

    status = _cairo_gl_surface_draw_image (abstract_surface, image,
					   0, 0,
					   image->width, image->height,
					   image_rect->x, image_rect->y);
    /* as we created the image, its format should be directly applicable */
    assert (status == CAIRO_STATUS_SUCCESS);

    cairo_surface_destroy (&image->base);
}

static cairo_status_t
_cairo_gl_surface_clone_similar (void		     *abstract_surface,
				 cairo_surface_t     *src,
				 int                  src_x,
				 int                  src_y,
				 int                  width,
				 int                  height,
				 int                 *clone_offset_x,
				 int                 *clone_offset_y,
				 cairo_surface_t    **clone_out)
{
    cairo_gl_surface_t *surface = abstract_surface;

    if (src->backend == surface->base.backend) {
	*clone_offset_x = 0;
	*clone_offset_y = 0;
	*clone_out = cairo_surface_reference (src);

	return CAIRO_STATUS_SUCCESS;
    } else if (_cairo_surface_is_image (src)) {
	cairo_image_surface_t *image_src = (cairo_image_surface_t *)src;
	cairo_gl_surface_t *clone;
	cairo_status_t status;

	clone = (cairo_gl_surface_t *)
	    _cairo_gl_surface_create_similar (&surface->base,
		                              src->content,
					      width, height);
	if (clone == NULL)
	    return UNSUPPORTED ("create_similar failed");
	if (clone->base.status)
	    return clone->base.status;

	status = _cairo_gl_surface_draw_image (clone, image_src,
					       src_x, src_y,
					       width, height,
					       0, 0);
	if (status) {
	    cairo_surface_destroy (&clone->base);
	    return status;
	}

	*clone_out = &clone->base;
	*clone_offset_x = src_x;
	*clone_offset_y = src_y;

	return CAIRO_STATUS_SUCCESS;
    }

    return UNSUPPORTED ("unknown src surface type in clone_similar");
}

/** Creates a cairo-gl pattern surface for the given trapezoids */
static cairo_status_t
_cairo_gl_get_traps_pattern (cairo_gl_surface_t *dst,
			     int dst_x, int dst_y,
			     int width, int height,
			     cairo_trapezoid_t *traps,
			     int num_traps,
			     cairo_antialias_t antialias,
			     cairo_surface_pattern_t *pattern)
{
    pixman_format_code_t pixman_format;
    pixman_image_t *image;
    cairo_surface_t *surface;
    int i;

    pixman_format = antialias != CAIRO_ANTIALIAS_NONE ? PIXMAN_a8 : PIXMAN_a1,
    image = pixman_image_create_bits (pixman_format, width, height, NULL, 0);
    if (unlikely (image == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    for (i = 0; i < num_traps; i++) {
	pixman_trapezoid_t trap;

	trap.top = _cairo_fixed_to_16_16 (traps[i].top);
	trap.bottom = _cairo_fixed_to_16_16 (traps[i].bottom);

	trap.left.p1.x = _cairo_fixed_to_16_16 (traps[i].left.p1.x);
	trap.left.p1.y = _cairo_fixed_to_16_16 (traps[i].left.p1.y);
	trap.left.p2.x = _cairo_fixed_to_16_16 (traps[i].left.p2.x);
	trap.left.p2.y = _cairo_fixed_to_16_16 (traps[i].left.p2.y);

	trap.right.p1.x = _cairo_fixed_to_16_16 (traps[i].right.p1.x);
	trap.right.p1.y = _cairo_fixed_to_16_16 (traps[i].right.p1.y);
	trap.right.p2.x = _cairo_fixed_to_16_16 (traps[i].right.p2.x);
	trap.right.p2.y = _cairo_fixed_to_16_16 (traps[i].right.p2.y);

	pixman_rasterize_trapezoid (image, &trap, -dst_x, -dst_y);
    }

    surface = _cairo_image_surface_create_for_pixman_image (image,
							    pixman_format);
    if (unlikely (surface->status)) {
	pixman_image_unref (image);
	return surface->status;
    }

    _cairo_pattern_init_for_surface (pattern, surface);
    cairo_surface_destroy (surface);

    return CAIRO_STATUS_SUCCESS;
}

static int
_cairo_gl_gradient_sample_width (const cairo_gradient_pattern_t *gradient)
{
    unsigned int n;
    int width;

    width = 8;
    for (n = 1; n < gradient->n_stops; n++) {
	double dx = gradient->stops[n].offset - gradient->stops[n-1].offset;
	double delta, max;
	int ramp;

	if (dx == 0)
	    continue;

	max = gradient->stops[n].color.red -
	      gradient->stops[n-1].color.red;

	delta = gradient->stops[n].color.green -
	        gradient->stops[n-1].color.green;
	if (delta > max)
	    max = delta;

	delta = gradient->stops[n].color.blue -
	        gradient->stops[n-1].color.blue;
	if (delta > max)
	    max = delta;

	delta = gradient->stops[n].color.alpha -
	        gradient->stops[n-1].color.alpha;
	if (delta > max)
	    max = delta;

	ramp = 128 * max / dx;
	if (ramp > width)
	    width = ramp;
    }

    width = (width + 7) & -8;
    return MIN (width, 1024);
}

static cairo_status_t
_render_gradient (const cairo_gl_context_t *ctx,
		  cairo_gradient_pattern_t *pattern,
		  void *bytes,
		  int width)
{
    pixman_image_t *gradient, *image;
    pixman_gradient_stop_t pixman_stops_stack[32];
    pixman_gradient_stop_t *pixman_stops;
    pixman_point_fixed_t p1, p2;
    unsigned int i;

    pixman_stops = pixman_stops_stack;
    if (unlikely (pattern->n_stops > ARRAY_LENGTH (pixman_stops_stack))) {
	pixman_stops = _cairo_malloc_ab (pattern->n_stops,
					 sizeof (pixman_gradient_stop_t));
	if (unlikely (pixman_stops == NULL))
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);
    }

    for (i = 0; i < pattern->n_stops; i++) {
	pixman_stops[i].x = _cairo_fixed_16_16_from_double (pattern->stops[i].offset);
	pixman_stops[i].color.red   = pattern->stops[i].color.red_short;
	pixman_stops[i].color.green = pattern->stops[i].color.green_short;
	pixman_stops[i].color.blue  = pattern->stops[i].color.blue_short;
	pixman_stops[i].color.alpha = pattern->stops[i].color.alpha_short;
    }

    p1.x = 0;
    p1.y = 0;
    p2.x = width << 16;
    p2.y = 0;

    gradient = pixman_image_create_linear_gradient (&p1, &p2,
						    pixman_stops,
						    pattern->n_stops);
    if (pixman_stops != pixman_stops_stack)
	tb_free (pixman_stops);

    if (unlikely (gradient == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    pixman_image_set_filter (gradient, PIXMAN_FILTER_BILINEAR, NULL, 0);
    pixman_image_set_repeat (gradient, PIXMAN_REPEAT_PAD);

    image = pixman_image_create_bits (PIXMAN_a8r8g8b8, width, 1,
				      bytes, sizeof(uint32_t)*width);
    if (unlikely (image == NULL)) {
	pixman_image_unref (gradient);
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);
    }

    pixman_image_composite (PIXMAN_OP_SRC,
			    gradient, NULL, image,
			    0, 0,
			    0, 0,
			    0, 0,
			    width, 1);

    pixman_image_unref (gradient);
    pixman_image_unref (image);
    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_gl_create_gradient_texture (const cairo_gl_context_t *ctx,
				   cairo_gl_surface_t *surface,
				   cairo_gradient_pattern_t *pattern,
				   GLuint *tex)
{
    int tex_width;
    GLubyte *data;

    assert (pattern->n_stops != 0);

    tex_width = _cairo_gl_gradient_sample_width (pattern);

    glBindBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB, ctx->texture_load_pbo);
    glBufferDataARB (GL_PIXEL_UNPACK_BUFFER_ARB, tex_width * sizeof (uint32_t), 0, GL_STREAM_DRAW);
    data = glMapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);

    _render_gradient (ctx, pattern, data, tex_width);

    glUnmapBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB);

    glGenTextures (1, tex);
    glBindTexture (GL_TEXTURE_1D, *tex);
    glTexImage1D (GL_TEXTURE_1D, 0, GL_RGBA8, tex_width, 0,
                  GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, 0);

    glBindBufferARB (GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    switch (pattern->base.extend) {
    case CAIRO_EXTEND_NONE:
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	break;
    case CAIRO_EXTEND_PAD:
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	break;
    case CAIRO_EXTEND_REPEAT:
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	break;
    case CAIRO_EXTEND_REFLECT:
	glTexParameteri (GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	break;
    }
}

/**
 * Like cairo_pattern_acquire_surface(), but returns a matrix that transforms
 * from dest to src coords.
 */
static cairo_status_t
_cairo_gl_pattern_texture_setup (cairo_gl_composite_operand_t *operand,
				 const cairo_pattern_t *src,
				 cairo_gl_surface_t *dst,
				 int src_x, int src_y,
				 int dst_x, int dst_y,
				 int width, int height)
{
    cairo_status_t status;
    cairo_matrix_t m;
    cairo_gl_surface_t *surface;
    cairo_surface_attributes_t *attributes;
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) dst->base.device;
    attributes = &operand->operand.texture.attributes;

    status = _cairo_pattern_acquire_surface (src, &dst->base,
					     src_x, src_y,
					     width, height,
					     CAIRO_PATTERN_ACQUIRE_NONE,
					     (cairo_surface_t **)
					     &surface,
					     attributes);
    if (unlikely (status))
	return status;

    if (ctx->tex_target == GL_TEXTURE_RECTANGLE_EXT &&
	(attributes->extend == CAIRO_EXTEND_REPEAT ||
	 attributes->extend == CAIRO_EXTEND_REFLECT))
    {
	_cairo_pattern_release_surface (operand->pattern,
					&surface->base,
					attributes);
	return UNSUPPORTED ("EXT_texture_rectangle with repeat/reflect");
    }

    assert (surface->base.backend == &_cairo_gl_surface_backend);

    operand->operand.texture.surface = surface;
    operand->operand.texture.tex = surface->tex;
    if (surface->base.content != CAIRO_CONTENT_ALPHA) {
	operand->source = CAIRO_GL_SHADER_SOURCE_TEXTURE;
	operand->mask = CAIRO_GL_SHADER_MASK_TEXTURE;
    } else {
	operand->source = CAIRO_GL_SHADER_SOURCE_TEXTURE_ALPHA;
	operand->mask = CAIRO_GL_SHADER_MASK_TEXTURE_ALPHA;
    }
    /* Translate the matrix from
     * (unnormalized src -> unnormalized src) to
     * (unnormalized dst -> unnormalized src)
     */
    cairo_matrix_init_translate (&m,
				 src_x - dst_x + attributes->x_offset,
				 src_y - dst_y + attributes->y_offset);
    cairo_matrix_multiply (&attributes->matrix,
			   &m,
			   &attributes->matrix);


    /* Translate the matrix from
     * (unnormalized dst -> unnormalized src) to
     * (unnormalized dst -> normalized src)
     */
    if (ctx->tex_target == GL_TEXTURE_RECTANGLE_EXT) {
	cairo_matrix_init_scale (&m,
				 1.0,
				 1.0);
    } else {
	cairo_matrix_init_scale (&m,
				 1.0 / surface->width,
				 1.0 / surface->height);
    }
    cairo_matrix_multiply (&attributes->matrix,
			   &attributes->matrix,
			   &m);

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_gl_solid_operand_init (cairo_gl_composite_operand_t *operand,
	                      const cairo_color_t *color)
{
    operand->type = OPERAND_CONSTANT;
    operand->source = CAIRO_GL_SHADER_SOURCE_CONSTANT;
    operand->mask = CAIRO_GL_SHADER_MASK_CONSTANT;
    operand->operand.constant.color[0] = color->red   * color->alpha;
    operand->operand.constant.color[1] = color->green * color->alpha;
    operand->operand.constant.color[2] = color->blue  * color->alpha;
    operand->operand.constant.color[3] = color->alpha;
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_gl_gradient_operand_init(cairo_gl_composite_operand_t *operand,
				cairo_gl_surface_t *dst)
{
    const cairo_gl_context_t *ctx = (cairo_gl_context_t *) dst->base.device;
    cairo_gradient_pattern_t *gradient = (cairo_gradient_pattern_t *)operand->pattern;

    /* Fast path for gradients with less than 2 color stops.
     * Required to prevent _cairo_pattern_acquire_surface() returning
     * a solid color which is cached beyond the life of the context.
     */
    if (gradient->n_stops < 2) {
	if (gradient->n_stops) {
	    return _cairo_gl_solid_operand_init (operand,
						 &gradient->stops->color);
	} else {
	    return _cairo_gl_solid_operand_init (operand,
						 CAIRO_COLOR_TRANSPARENT);
	}
    } else {
	unsigned int i;

	/* Is the gradient a uniform colour?
	 * Happens more often than you would believe.
	 */
	for (i = 1; i < gradient->n_stops; i++) {
	    if (! _cairo_color_equal (&gradient->stops[0].color,
				      &gradient->stops[i].color))
		{
		    break;
		}
	}
	if (i == gradient->n_stops) {
	    return _cairo_gl_solid_operand_init (operand,
						 &gradient->stops->color);
	}
    }

    if (!ctx->using_glsl)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (gradient->base.type == CAIRO_PATTERN_TYPE_LINEAR) {
	cairo_linear_pattern_t *linear = (cairo_linear_pattern_t *) gradient;
        double x0, y0, x1, y1;

	x0 = _cairo_fixed_to_double (linear->p1.x);
	x1 = _cairo_fixed_to_double (linear->p2.x);
	y0 = _cairo_fixed_to_double (linear->p1.y);
	y1 = _cairo_fixed_to_double (linear->p2.y);

        if ((unsigned int)ctx->max_texture_size / 2 <= gradient->n_stops) {
            return CAIRO_INT_STATUS_UNSUPPORTED;
        }

        _cairo_gl_create_gradient_texture (ctx,
					   dst,
					   gradient,
					   &operand->operand.linear.tex);

	/* Translation matrix from the destination fragment coordinates
	 * (pixels from lower left = 0,0) to the coordinates in the
	 */
	cairo_matrix_init_translate (&operand->operand.linear.m, -x0, -y0);
	cairo_matrix_multiply (&operand->operand.linear.m,
			       &operand->pattern->matrix,
			       &operand->operand.linear.m);
	cairo_matrix_translate (&operand->operand.linear.m, 0, dst->height);
	cairo_matrix_scale (&operand->operand.linear.m, 1.0, -1.0);

	operand->operand.linear.segment_x = x1 - x0;
	operand->operand.linear.segment_y = y1 - y0;

	operand->type = OPERAND_LINEAR_GRADIENT;
	operand->source = CAIRO_GL_SHADER_SOURCE_LINEAR_GRADIENT;
	operand->mask = CAIRO_GL_SHADER_MASK_LINEAR_GRADIENT;
        return CAIRO_STATUS_SUCCESS;
    } else {
	cairo_radial_pattern_t *radial = (cairo_radial_pattern_t *) gradient;
        double x0, y0, r0, x1, y1, r1;

	x0 = _cairo_fixed_to_double (radial->c1.x);
	x1 = _cairo_fixed_to_double (radial->c2.x);
	y0 = _cairo_fixed_to_double (radial->c1.y);
	y1 = _cairo_fixed_to_double (radial->c2.y);
	r0 = _cairo_fixed_to_double (radial->r1);
	r1 = _cairo_fixed_to_double (radial->r2);

        if ((unsigned int)ctx->max_texture_size / 2 <= gradient->n_stops)
            return CAIRO_INT_STATUS_UNSUPPORTED;

        _cairo_gl_create_gradient_texture (ctx,
					   dst,
					   gradient,
					   &operand->operand.radial.tex);

	/* Translation matrix from the destination fragment coordinates
	 * (pixels from lower left = 0,0) to the coordinates in the
	 */
	cairo_matrix_init_translate (&operand->operand.radial.m, -x0, -y0);
	cairo_matrix_multiply (&operand->operand.radial.m,
			       &operand->pattern->matrix,
			       &operand->operand.radial.m);
	cairo_matrix_translate (&operand->operand.radial.m, 0, dst->height);
	cairo_matrix_scale (&operand->operand.radial.m, 1.0, -1.0);

	operand->operand.radial.circle_1_x = x1 - x0;
	operand->operand.radial.circle_1_y = y1 - y0;
	operand->operand.radial.radius_0 = r0;
	operand->operand.radial.radius_1 = r1;

	operand->type = OPERAND_RADIAL_GRADIENT;
	operand->source = CAIRO_GL_SHADER_SOURCE_RADIAL_GRADIENT;
	operand->mask = CAIRO_GL_SHADER_MASK_RADIAL_GRADIENT;
        return CAIRO_STATUS_SUCCESS;
    }

    return CAIRO_INT_STATUS_UNSUPPORTED;
}

cairo_int_status_t
_cairo_gl_operand_init (cairo_gl_composite_operand_t *operand,
		       const cairo_pattern_t *pattern,
		       cairo_gl_surface_t *dst,
		       int src_x, int src_y,
		       int dst_x, int dst_y,
		       int width, int height)
{
    cairo_status_t status;

    operand->pattern = pattern;

    switch (pattern->type) {
    case CAIRO_PATTERN_TYPE_SOLID:
	return _cairo_gl_solid_operand_init (operand,
		                             &((cairo_solid_pattern_t *) pattern)->color);
    case CAIRO_PATTERN_TYPE_LINEAR:
    case CAIRO_PATTERN_TYPE_RADIAL:
	status = _cairo_gl_gradient_operand_init (operand, dst);
	if (!_cairo_status_is_error (status))
	    return status;

	/* fall through */

    default:
    case CAIRO_PATTERN_TYPE_SURFACE:
	operand->type = OPERAND_TEXTURE;
	return _cairo_gl_pattern_texture_setup (operand,
						pattern, dst,
						src_x, src_y,
						dst_x, dst_y,
						width, height);
    }
}

void
_cairo_gl_operand_destroy (cairo_gl_composite_operand_t *operand)
{
    switch (operand->type) {
    case OPERAND_CONSTANT:
	break;
    case OPERAND_LINEAR_GRADIENT:
	glDeleteTextures (1, &operand->operand.linear.tex);
	break;
    case OPERAND_RADIAL_GRADIENT:
	glDeleteTextures (1, &operand->operand.radial.tex);
	break;
    case OPERAND_TEXTURE:
	if (operand->operand.texture.surface != NULL) {
	    cairo_gl_surface_t *surface = operand->operand.texture.surface;

	    _cairo_pattern_release_surface (operand->pattern,
					    &surface->base,
					    &operand->operand.texture.attributes);
	}
	break;
    }
}

static void
_cairo_gl_set_tex_combine_constant_color (cairo_gl_context_t *ctx,
					  cairo_gl_composite_setup_t *setup,
					  int tex_unit,
					  GLfloat *color)
{
    if (setup->shader) {
	const char *uniform_name;
        cairo_status_t status;

	if (tex_unit == 0)
	    uniform_name = "constant_source";
	else
	    uniform_name = "constant_mask";

	status = bind_vec4_to_shader(setup->shader->program,
                                     uniform_name,
                                     color[0],
                                     color[1],
                                     color[2],
                                     color[3]);
        assert (! _cairo_status_is_error (status));

	return;
    }

    /* Fall back to fixed function */
    glActiveTexture (GL_TEXTURE0 + tex_unit);
    /* Have to have a dummy texture bound in order to use the combiner unit. */
    glBindTexture (ctx->tex_target, ctx->dummy_tex);
    glEnable (ctx->tex_target);

    glTexEnvfv (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, color);
    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    if (tex_unit == 0) {
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);
    } else {
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);
    }
    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_CONSTANT);
    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_CONSTANT);
    if (tex_unit == 0) {
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
    } else {
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_ALPHA);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

	glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
	glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PREVIOUS);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    }
}

void
_cairo_gl_set_src_operand (cairo_gl_context_t *ctx,
			   cairo_gl_composite_setup_t *setup)
{
    cairo_surface_attributes_t *src_attributes;
    GLfloat constant_color[4] = {0.0, 0.0, 0.0, 0.0};
    cairo_status_t status;

    src_attributes = &setup->src.operand.texture.attributes;

    switch (setup->src.type) {
    case OPERAND_CONSTANT:
	_cairo_gl_set_tex_combine_constant_color (ctx, setup, 0,
						  setup->src.operand.constant.color);
	break;
    case OPERAND_TEXTURE:
	_cairo_gl_set_texture_surface (0, setup->src.operand.texture.tex,
				       src_attributes, ctx->tex_target);
	if (!setup->shader) {
	    /* Set up the constant color we use to set color to 0 if needed. */
	    glTexEnvfv (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constant_color);
	    /* Set up the combiner to just set color to the sampled texture. */
	    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	    glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	    glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);

	    /* Force the src color to 0 if the surface should be
	     * alpha-only.  We may have a teximage with color bits if
	     * the implementation doesn't support GL_ALPHA FBOs.
	     */
	    if (setup->src.operand.texture.surface->base.content !=
		CAIRO_CONTENT_ALPHA)
		glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE0);
	    else
		glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_CONSTANT);
	    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE0);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	}
	break;

    case OPERAND_LINEAR_GRADIENT:
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_1D, setup->src.operand.linear.tex);
	glEnable (GL_TEXTURE_1D);

	status = bind_matrix_to_shader (setup->shader->program,
					"source_matrix",
					&setup->src.operand.linear.m);
	assert (!_cairo_status_is_error (status));

	status = bind_vec2_to_shader (setup->shader->program,
				      "source_segment",
				      setup->src.operand.linear.segment_x,
				      setup->src.operand.linear.segment_y);
	assert (!_cairo_status_is_error (status));
	break;

    case OPERAND_RADIAL_GRADIENT:
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_1D, setup->src.operand.linear.tex);
	glEnable (GL_TEXTURE_1D);

        status = bind_matrix_to_shader (setup->shader->program,
					"source_matrix",
					&setup->src.operand.radial.m);
	assert (!_cairo_status_is_error (status));

        status = bind_vec2_to_shader (setup->shader->program,
				      "source_circle_1",
				      setup->src.operand.radial.circle_1_x,
				      setup->src.operand.radial.circle_1_y);
	assert (!_cairo_status_is_error (status));

        status = bind_float_to_shader (setup->shader->program,
				       "source_radius_0",
				       setup->src.operand.radial.radius_0);
	assert (!_cairo_status_is_error (status));

        status = bind_float_to_shader (setup->shader->program,
				       "source_radius_1",
				       setup->src.operand.radial.radius_1);
	assert (!_cairo_status_is_error (status));
	break;
    }
}

/* This is like _cairo_gl_set_src_operand, but instead swizzles the source
 * for creating the "source alpha" value (src.aaaa * mask.argb) required by
 * component alpha rendering.
 */
void
_cairo_gl_set_src_alpha_operand (cairo_gl_context_t *ctx,
				 cairo_gl_composite_setup_t *setup)
{
    GLfloat constant_color[4] = {0.0, 0.0, 0.0, 0.0};
    cairo_surface_attributes_t *src_attributes;

    src_attributes = &setup->src.operand.texture.attributes;

    switch (setup->src.type) {
    case OPERAND_CONSTANT:
	constant_color[0] = setup->src.operand.constant.color[3];
	constant_color[1] = setup->src.operand.constant.color[3];
	constant_color[2] = setup->src.operand.constant.color[3];
	constant_color[3] = setup->src.operand.constant.color[3];
	_cairo_gl_set_tex_combine_constant_color (ctx, setup, 0,
						  constant_color);
	break;
    case OPERAND_TEXTURE:
	_cairo_gl_set_texture_surface (0, setup->src.operand.texture.tex,
				       src_attributes, ctx->tex_target);
	if (!setup->shader) {
	    /* Set up the combiner to just set color to the sampled texture. */
	    glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	    glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
	    glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_REPLACE);

	    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE0);
	    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE0);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_ALPHA);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	}
	break;
    case OPERAND_LINEAR_GRADIENT:
    case OPERAND_RADIAL_GRADIENT:
	assert(0);
    }
}

static void
_cairo_gl_set_linear_gradient_mask_operand (cairo_gl_composite_setup_t *setup)
{
    cairo_status_t status;

    assert(setup->shader);

    glActiveTexture (GL_TEXTURE1);
    glBindTexture (GL_TEXTURE_1D, setup->mask.operand.linear.tex);
    glEnable (GL_TEXTURE_1D);

    status = bind_matrix_to_shader (setup->shader->program,
			   "mask_matrix", &setup->mask.operand.linear.m);
    assert (!_cairo_status_is_error (status));

    status = bind_vec2_to_shader (setup->shader->program,
			 "mask_segment",
			 setup->mask.operand.linear.segment_x,
			 setup->mask.operand.linear.segment_y);
    assert (!_cairo_status_is_error (status));
}

static void
_cairo_gl_set_radial_gradient_mask_operand (cairo_gl_composite_setup_t *setup)
{
    cairo_status_t status;

    assert(setup->shader);

    glActiveTexture (GL_TEXTURE1);
    glBindTexture (GL_TEXTURE_1D, setup->mask.operand.radial.tex);
    glEnable (GL_TEXTURE_1D);

    status = bind_matrix_to_shader (setup->shader->program,
				    "mask_matrix",
				    &setup->mask.operand.radial.m);
    assert (!_cairo_status_is_error (status));

    status = bind_vec2_to_shader (setup->shader->program,
				  "mask_circle_1",
				  setup->mask.operand.radial.circle_1_x,
				  setup->mask.operand.radial.circle_1_y);
    assert (!_cairo_status_is_error (status));

    status = bind_float_to_shader (setup->shader->program,
				   "mask_radius_0",
				   setup->mask.operand.radial.radius_0);
    assert (!_cairo_status_is_error (status));

    status = bind_float_to_shader (setup->shader->program,
				   "mask_radius_1",
				   setup->mask.operand.radial.radius_1);
    assert (!_cairo_status_is_error (status));
}

/* This is like _cairo_gl_set_src_alpha_operand, for component alpha setup
 * of the mask part of IN to produce a "source alpha" value.
 */
static void
_cairo_gl_set_component_alpha_mask_operand (cairo_gl_context_t *ctx,
					    cairo_gl_composite_setup_t *setup)
{
    cairo_surface_attributes_t *mask_attributes;
    GLfloat constant_color[4] = {0.0, 0.0, 0.0, 0.0};

    mask_attributes = &setup->mask.operand.texture.attributes;

    if (!setup->shader) {
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

	glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);
	glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PREVIOUS);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    }

    switch (setup->mask.type) {
    case OPERAND_CONSTANT:
	/* Have to have a dummy texture bound in order to use the combiner unit. */
	if (setup->shader) {
            cairo_status_t status;
	    status = bind_vec4_to_shader(setup->shader->program,
                                         "constant_mask",
                                         setup->src.operand.constant.color[0],
                                         setup->src.operand.constant.color[1],
                                         setup->src.operand.constant.color[2],
                                         setup->src.operand.constant.color[3]);
            assert (! _cairo_status_is_error (status));
	} else {
	    glBindTexture (ctx->tex_target, ctx->dummy_tex);
	    glActiveTexture (GL_TEXTURE1);
	    glEnable (ctx->tex_target);

	    glTexEnvfv (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR,
			setup->mask.operand.constant.color);

	    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_CONSTANT);
	    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_CONSTANT);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	}
	break;
    case OPERAND_TEXTURE:
	_cairo_gl_set_texture_surface (1, setup->mask.operand.texture.tex,
				       mask_attributes, ctx->tex_target);
	if (!setup->shader) {
	    /* Set up the constant color we use to set color to 0 if needed. */
	    glTexEnvfv (GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constant_color);

	    /* Force the mask color to 0 if the surface should be
	     * alpha-only.  We may have a teximage with color bits if
	     * the implementation doesn't support GL_ALPHA FBOs.
	     */
	    if (setup->mask.operand.texture.surface->base.content !=
		CAIRO_CONTENT_ALPHA)
		glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE1);
	    else
		glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_CONSTANT);
	    glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_TEXTURE1);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	    glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);
	}
	break;

    case OPERAND_LINEAR_GRADIENT:
	_cairo_gl_set_linear_gradient_mask_operand (setup);
	break;

    case OPERAND_RADIAL_GRADIENT:
	_cairo_gl_set_radial_gradient_mask_operand (setup);
	break;
    }
}

/**
 * implements component-alpha %CAIRO_OPERATOR_SOURCE using two passes of
 * the simpler operations %CAIRO_OPERATOR_DEST_OUT and %CAIRO_OPERATOR_ADD.
 *
 * From http://anholt.livejournal.com/32058.html:
 *
 * The trouble is that component-alpha rendering requires two different sources
 * for blending: one for the source value to the blender, which is the
 * per-channel multiplication of source and mask, and one for the source alpha
 * for multiplying with the destination channels, which is the multiplication
 * of the source channels by the mask alpha. So the equation for Over is:
 *
 * dst.A = src.A * mask.A + (1 - (src.A * mask.A)) * dst.A
 * dst.R = src.R * mask.R + (1 - (src.A * mask.R)) * dst.R
 * dst.G = src.G * mask.G + (1 - (src.A * mask.G)) * dst.G
 * dst.B = src.B * mask.B + (1 - (src.A * mask.B)) * dst.B
 *
 * But we can do some simpler operations, right? How about PictOpOutReverse,
 * which has a source factor of 0 and dest factor of (1 - source alpha). We
 * can get the source alpha value (srca.X = src.A * mask.X) out of the texture
 * blenders pretty easily. So we can do a component-alpha OutReverse, which
 * gets us:
 *
 * dst.A = 0 + (1 - (src.A * mask.A)) * dst.A
 * dst.R = 0 + (1 - (src.A * mask.R)) * dst.R
 * dst.G = 0 + (1 - (src.A * mask.G)) * dst.G
 * dst.B = 0 + (1 - (src.A * mask.B)) * dst.B
 *
 * OK. And if an op doesn't use the source alpha value for the destination
 * factor, then we can do the channel multiplication in the texture blenders
 * to get the source value, and ignore the source alpha that we wouldn't use.
 * We've supported this in the Radeon driver for a long time. An example would
 * be PictOpAdd, which does:
 *
 * dst.A = src.A * mask.A + dst.A
 * dst.R = src.R * mask.R + dst.R
 * dst.G = src.G * mask.G + dst.G
 * dst.B = src.B * mask.B + dst.B
 *
 * Hey, this looks good! If we do a PictOpOutReverse and then a PictOpAdd right
 * after it, we get:
 *
 * dst.A = src.A * mask.A + ((1 - (src.A * mask.A)) * dst.A)
 * dst.R = src.R * mask.R + ((1 - (src.A * mask.R)) * dst.R)
 * dst.G = src.G * mask.G + ((1 - (src.A * mask.G)) * dst.G)
 * dst.B = src.B * mask.B + ((1 - (src.A * mask.B)) * dst.B)
 *
 * This two-pass trickery could be avoided using a new GL extension that
 * lets two values come out of the shader and into the blend unit.
 */
static cairo_int_status_t
_cairo_gl_surface_composite_component_alpha (cairo_operator_t op,
					     const cairo_pattern_t *src,
					     const cairo_pattern_t *mask,
					     void *abstract_dst,
					     int src_x,
					     int src_y,
					     int mask_x,
					     int mask_y,
					     int dst_x,
					     int dst_y,
					     unsigned int width,
					     unsigned int height,
					     cairo_region_t *clip_region)
{
    cairo_gl_surface_t	*dst = abstract_dst;
    cairo_surface_attributes_t *src_attributes, *mask_attributes = NULL;
    cairo_gl_context_t *ctx;
    struct gl_point {
	GLfloat x, y;
    } vertices_stack[8], texcoord_src_stack[8], texcoord_mask_stack[8];
    struct gl_point *vertices = vertices_stack;
    struct gl_point *texcoord_src = texcoord_src_stack;
    struct gl_point *texcoord_mask = texcoord_mask_stack;
    cairo_status_t status;
    int num_vertices, i;
    GLenum err;
    cairo_gl_composite_setup_t setup;
    cairo_gl_shader_program_t *ca_source_program = NULL;
    cairo_gl_shader_program_t *ca_source_alpha_program = NULL;

    if (op != CAIRO_OPERATOR_OVER && op != CAIRO_OPERATOR_ADD)
	return UNSUPPORTED ("unsupported component alpha operator");

    memset (&setup, 0, sizeof (setup));

    status = _cairo_gl_operand_init (&setup.src, src, dst,
				     src_x, src_y,
				     dst_x, dst_y,
				     width, height);
    if (unlikely (status))
	return status;
    src_attributes = &setup.src.operand.texture.attributes;

    status = _cairo_gl_operand_init (&setup.mask, mask, dst,
				     mask_x, mask_y,
				     dst_x, dst_y,
				     width, height);
    if (unlikely (status)) {
	_cairo_gl_operand_destroy (&setup.src);
	return status;
    }
    mask_attributes = &setup.mask.operand.texture.attributes;

    status = _cairo_gl_context_acquire (dst->base.device, &ctx);
    if (unlikely (status)) {
	_cairo_gl_operand_destroy (&setup.src);
	_cairo_gl_operand_destroy (&setup.mask);
	return status;
    }

    status = _cairo_gl_get_program (ctx,
				    setup.src.source,
				    setup.mask.mask,
				    CAIRO_GL_SHADER_IN_CA_SOURCE,
				    &ca_source_program);
    if (!_cairo_status_is_error (status)) {
	status = _cairo_gl_get_program (ctx,
					setup.src.source,
					setup.mask.mask,
					CAIRO_GL_SHADER_IN_CA_SOURCE_ALPHA,
					&ca_source_alpha_program);
	if (_cairo_status_is_error (status)) {
	    /* We'll fall back to fixed function instead. */
	    ca_source_program = NULL;
	    status = CAIRO_STATUS_SUCCESS;
	}
    } else {
	/* We'll fall back to fixed function instead. */
	status = CAIRO_STATUS_SUCCESS;
    }

    _cairo_gl_set_destination (dst);

    if (clip_region != NULL) {
	int num_rectangles;

	num_rectangles = cairo_region_num_rectangles (clip_region);
	if (num_rectangles * 4 > ARRAY_LENGTH (vertices_stack)) {
	    vertices = _cairo_malloc_ab (num_rectangles,
					 4*3*sizeof (vertices[0]));
	    if (unlikely (vertices == NULL)) {
		status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
		goto CONTEXT_RELEASE;
	    }

	    texcoord_src = vertices + num_rectangles * 4;
	    texcoord_mask = texcoord_src + num_rectangles * 4;
	}

	for (i = 0; i < num_rectangles; i++) {
	    cairo_rectangle_int_t rect;

	    cairo_region_get_rectangle (clip_region, i, &rect);
	    vertices[4*i + 0].x = rect.x;
	    vertices[4*i + 0].y = rect.y;
	    vertices[4*i + 1].x = rect.x + rect.width;
	    vertices[4*i + 1].y = rect.y;
	    vertices[4*i + 2].x = rect.x + rect.width;
	    vertices[4*i + 2].y = rect.y + rect.height;
	    vertices[4*i + 3].x = rect.x;
	    vertices[4*i + 3].y = rect.y + rect.height;
	}

	num_vertices = 4 * num_rectangles;
    } else {
	vertices[0].x = dst_x;
	vertices[0].y = dst_y;
	vertices[1].x = dst_x + width;
	vertices[1].y = dst_y;
	vertices[2].x = dst_x + width;
	vertices[2].y = dst_y + height;
	vertices[3].x = dst_x;
	vertices[3].y = dst_y + height;

	num_vertices = 4;
    }

    glVertexPointer (2, GL_FLOAT, sizeof (GLfloat) * 2, vertices);
    glEnableClientState (GL_VERTEX_ARRAY);

    if (setup.src.type == OPERAND_TEXTURE) {
	for (i = 0; i < num_vertices; i++) {
	    double s, t;

	    s = vertices[i].x;
	    t = vertices[i].y;
	    cairo_matrix_transform_point (&src_attributes->matrix, &s, &t);
	    texcoord_src[i].x = s;
	    texcoord_src[i].y = t;
	}

	glClientActiveTexture (GL_TEXTURE0);
	glTexCoordPointer (2, GL_FLOAT, sizeof (GLfloat)*2, texcoord_src);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    }

    if (setup.mask.type == OPERAND_TEXTURE) {
	for (i = 0; i < num_vertices; i++) {
	    double s, t;

	    s = vertices[i].x;
	    t = vertices[i].y;
	    cairo_matrix_transform_point (&mask_attributes->matrix, &s, &t);
	    texcoord_mask[i].x = s;
	    texcoord_mask[i].y = t;
	}

	glClientActiveTexture (GL_TEXTURE1);
	glTexCoordPointer (2, GL_FLOAT, sizeof (GLfloat)*2, texcoord_mask);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    }

    if (op == CAIRO_OPERATOR_OVER) {
	setup.shader = ca_source_alpha_program;
	_cairo_gl_use_program (setup.shader);
	_cairo_gl_set_operator (dst, CAIRO_OPERATOR_DEST_OUT, TRUE);
	_cairo_gl_set_src_alpha_operand (ctx, &setup);
	_cairo_gl_set_component_alpha_mask_operand (ctx, &setup);
	glDrawArrays (GL_QUADS, 0, num_vertices);
    }

    setup.shader = ca_source_program;
    _cairo_gl_use_program (setup.shader);
    _cairo_gl_set_operator (dst, CAIRO_OPERATOR_ADD, TRUE);
    _cairo_gl_set_src_operand (ctx, &setup);
    _cairo_gl_set_component_alpha_mask_operand (ctx, &setup);
    glDrawArrays (GL_QUADS, 0, num_vertices);

    glDisable (GL_BLEND);
    _cairo_gl_use_program (NULL);

    glDisableClientState (GL_VERTEX_ARRAY);

    glClientActiveTexture (GL_TEXTURE0);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glActiveTexture (GL_TEXTURE0);
    glDisable (GL_TEXTURE_1D);
    glDisable (ctx->tex_target);

    glClientActiveTexture (GL_TEXTURE1);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glActiveTexture (GL_TEXTURE1);
    glDisable (GL_TEXTURE_1D);
    glDisable (ctx->tex_target);

    while ((err = glGetError ()))
	fprintf (stderr, "GL error 0x%08x\n", (int) err);

  CONTEXT_RELEASE:
    _cairo_gl_context_release (ctx);

    _cairo_gl_operand_destroy (&setup.src);
    if (mask != NULL)
	_cairo_gl_operand_destroy (&setup.mask);

    if (vertices != vertices_stack)
	tb_free (vertices);

    return status;
}

static cairo_int_status_t
_cairo_gl_surface_composite (cairo_operator_t		  op,
			     const cairo_pattern_t	 *src,
			     const cairo_pattern_t	 *mask,
			     void			 *abstract_dst,
			     int			  src_x,
			     int			  src_y,
			     int			  mask_x,
			     int			  mask_y,
			     int			  dst_x,
			     int			  dst_y,
			     unsigned int		  width,
			     unsigned int		  height,
			     cairo_region_t		 *clip_region)
{
    cairo_gl_surface_t	*dst = abstract_dst;
    cairo_surface_attributes_t *src_attributes, *mask_attributes = NULL;
    cairo_gl_context_t *ctx;
    struct gl_point {
	GLfloat x, y;
    } vertices_stack[8], texcoord_src_stack[8], texcoord_mask_stack[8];
    struct gl_point *vertices = vertices_stack;
    struct gl_point *texcoord_src = texcoord_src_stack;
    struct gl_point *texcoord_mask = texcoord_mask_stack;
    cairo_status_t status;
    int num_vertices, i;
    GLenum err;
    cairo_gl_composite_setup_t setup;

    if (! _cairo_gl_operator_is_supported (op))
	return UNSUPPORTED ("unsupported operator");

    if (mask && mask->has_component_alpha) {
	/* Try two-pass component alpha support, or bail. */
	return _cairo_gl_surface_composite_component_alpha(op,
							   src,
							   mask,
							   abstract_dst,
							   src_x,
							   src_y,
							   mask_x,
							   mask_y,
							   dst_x,
							   dst_y,
							   width,
							   height,
							   clip_region);
    }

    memset (&setup, 0, sizeof (setup));

    status = _cairo_gl_operand_init (&setup.src, src, dst,
				     src_x, src_y,
				     dst_x, dst_y,
				     width, height);
    if (unlikely (status))
	return status;
    src_attributes = &setup.src.operand.texture.attributes;

    if (mask != NULL) {
	status = _cairo_gl_operand_init (&setup.mask, mask, dst,
					 mask_x, mask_y,
					 dst_x, dst_y,
					 width, height);
	if (unlikely (status)) {
	    _cairo_gl_operand_destroy (&setup.src);
	    return status;
	}
	mask_attributes = &setup.mask.operand.texture.attributes;
    } else {
	setup.mask.mask = CAIRO_GL_SHADER_MASK_NONE;
    }

    status = _cairo_gl_context_acquire (dst->base.device, &ctx);
    if (unlikely (status))
	goto CLEANUP_SHADER;

    status = _cairo_gl_get_program (ctx,
				    setup.src.source,
				    setup.mask.mask,
				    CAIRO_GL_SHADER_IN_NORMAL,
				    &setup.shader);
    if (_cairo_status_is_error (status)) {
	/* We'll fall back to fixed function instead. */
	setup.shader = NULL;
	status = CAIRO_STATUS_SUCCESS;
    }

    _cairo_gl_set_destination (dst);
    _cairo_gl_set_operator (dst, op, FALSE);

    _cairo_gl_use_program (setup.shader);
    _cairo_gl_set_src_operand (ctx, &setup);

    if (mask != NULL) {
	switch (setup.mask.type) {
	case OPERAND_CONSTANT:
	    _cairo_gl_set_tex_combine_constant_color (ctx, &setup, 1,
						      setup.mask.operand.constant.color);
	    break;

	case OPERAND_TEXTURE:
	    _cairo_gl_set_texture_surface (1, setup.mask.operand.texture.tex,
					   mask_attributes, ctx->tex_target);

	    if (!setup.shader) {
		glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
		glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
		glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

		glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
		glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS);
		glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
		glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

		/* IN: dst.argb = src.argb * mask.aaaa */
		glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_RGB, GL_TEXTURE1);
		glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_TEXTURE1);
		glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_ALPHA);
		glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
	    }
	    break;
	case OPERAND_LINEAR_GRADIENT:
	    _cairo_gl_set_linear_gradient_mask_operand (&setup);
	    break;
	case OPERAND_RADIAL_GRADIENT:
	    _cairo_gl_set_radial_gradient_mask_operand (&setup);
	    break;
	}
    }

    if (clip_region != NULL) {
	int num_rectangles;

	num_rectangles = cairo_region_num_rectangles (clip_region);
	if (num_rectangles * 4 > ARRAY_LENGTH (vertices_stack)) {
	    vertices = _cairo_malloc_ab (num_rectangles,
					 4*3*sizeof (vertices[0]));
	    if (unlikely (vertices == NULL)) {
		status = _cairo_error (CAIRO_STATUS_NO_MEMORY);
		goto CONTEXT_RELEASE;
	    }

	    texcoord_src = vertices + num_rectangles * 4;
	    texcoord_mask = texcoord_src + num_rectangles * 4;
	}

	for (i = 0; i < num_rectangles; i++) {
	    cairo_rectangle_int_t rect;

	    cairo_region_get_rectangle (clip_region, i, &rect);
	    vertices[4*i + 0].x = rect.x;
	    vertices[4*i + 0].y = rect.y;
	    vertices[4*i + 1].x = rect.x + rect.width;
	    vertices[4*i + 1].y = rect.y;
	    vertices[4*i + 2].x = rect.x + rect.width;
	    vertices[4*i + 2].y = rect.y + rect.height;
	    vertices[4*i + 3].x = rect.x;
	    vertices[4*i + 3].y = rect.y + rect.height;
	}

	num_vertices = 4 * num_rectangles;
    } else {
	vertices[0].x = dst_x;
	vertices[0].y = dst_y;
	vertices[1].x = dst_x + width;
	vertices[1].y = dst_y;
	vertices[2].x = dst_x + width;
	vertices[2].y = dst_y + height;
	vertices[3].x = dst_x;
	vertices[3].y = dst_y + height;

	num_vertices = 4;
    }

    glVertexPointer (2, GL_FLOAT, sizeof (GLfloat) * 2, vertices);
    glEnableClientState (GL_VERTEX_ARRAY);

    if (setup.src.type == OPERAND_TEXTURE) {
	for (i = 0; i < num_vertices; i++) {
	    double s, t;

	    s = vertices[i].x;
	    t = vertices[i].y;
	    cairo_matrix_transform_point (&src_attributes->matrix, &s, &t);
	    texcoord_src[i].x = s;
	    texcoord_src[i].y = t;
	}

	glClientActiveTexture (GL_TEXTURE0);
	glTexCoordPointer (2, GL_FLOAT, sizeof (GLfloat)*2, texcoord_src);
	glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    }

    if (mask != NULL) {
	if (setup.mask.type == OPERAND_TEXTURE) {
	    for (i = 0; i < num_vertices; i++) {
		double s, t;

		s = vertices[i].x;
		t = vertices[i].y;
		cairo_matrix_transform_point (&mask_attributes->matrix, &s, &t);
		texcoord_mask[i].x = s;
		texcoord_mask[i].y = t;
	    }

	    glClientActiveTexture (GL_TEXTURE1);
	    glTexCoordPointer (2, GL_FLOAT, sizeof (GLfloat)*2, texcoord_mask);
	    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	}
    }

    glDrawArrays (GL_QUADS, 0, num_vertices);

    _cairo_gl_use_program (NULL);
    glDisable (GL_BLEND);

    glDisableClientState (GL_VERTEX_ARRAY);

    glClientActiveTexture (GL_TEXTURE0);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glActiveTexture (GL_TEXTURE0);
    glDisable (GL_TEXTURE_1D);
    glDisable (ctx->tex_target);

    glClientActiveTexture (GL_TEXTURE1);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glActiveTexture (GL_TEXTURE1);
    glDisable (GL_TEXTURE_1D);
    glDisable (ctx->tex_target);

    while ((err = glGetError ()))
	fprintf (stderr, "GL error 0x%08x\n", (int) err);

  CONTEXT_RELEASE:
    _cairo_gl_context_release (ctx);

  CLEANUP_SHADER:
    _cairo_gl_operand_destroy (&setup.src);
    if (mask != NULL)
	_cairo_gl_operand_destroy (&setup.mask);

    if (vertices != vertices_stack)
	tb_free (vertices);

    return status;
}

static cairo_int_status_t
_cairo_gl_surface_composite_trapezoids (cairo_operator_t op,
					const cairo_pattern_t *pattern,
					void *abstract_dst,
					cairo_antialias_t antialias,
					int src_x, int src_y,
					int dst_x, int dst_y,
					unsigned int width,
					unsigned int height,
					cairo_trapezoid_t *traps,
					int num_traps,
					cairo_region_t *clip_region)
{
    cairo_gl_surface_t *dst = abstract_dst;
    cairo_surface_pattern_t traps_pattern;
    cairo_int_status_t status;

    if (! _cairo_gl_operator_is_supported (op))
	return UNSUPPORTED ("unsupported operator");

    if (_cairo_surface_check_span_renderer (op,pattern,&dst->base, antialias)) {
	status =
	    _cairo_surface_composite_trapezoids_as_polygon (&dst->base,
							    op, pattern,
							    antialias,
							    src_x, src_y,
							    dst_x, dst_y,
							    width, height,
							    traps, num_traps,
							    clip_region);
	if (status != CAIRO_INT_STATUS_UNSUPPORTED)
	    return status;
    }

    status = _cairo_gl_get_traps_pattern (dst,
					  dst_x, dst_y, width, height,
					  traps, num_traps, antialias,
					  &traps_pattern);
    if (unlikely (status))
	return status;

    status = _cairo_gl_surface_composite (op,
					  pattern, &traps_pattern.base, dst,
					  src_x, src_y,
					  0, 0,
					  dst_x, dst_y,
					  width, height,
					  clip_region);

    _cairo_pattern_fini (&traps_pattern.base);

    return status;
}

static cairo_int_status_t
_cairo_gl_surface_fill_rectangles_fixed (void			 *abstract_surface,
					 cairo_operator_t	  op,
					 const cairo_color_t     *color,
					 cairo_rectangle_int_t   *rects,
					 int			  num_rects)
{
#define N_STACK_RECTS 4
    cairo_gl_surface_t *surface = abstract_surface;
    GLfloat vertices_stack[N_STACK_RECTS*4*2];
    GLfloat colors_stack[N_STACK_RECTS*4*4];
    cairo_gl_context_t *ctx;
    int i;
    GLfloat *vertices;
    GLfloat *colors;
    cairo_status_t status;

    if (! _cairo_gl_operator_is_supported (op))
	return UNSUPPORTED ("unsupported operator");

    status = _cairo_gl_context_acquire (surface->base.device, &ctx);
    if (unlikely (status))
	return status;

    _cairo_gl_set_destination (surface);
    _cairo_gl_set_operator (surface, op, FALSE);

    if (num_rects > N_STACK_RECTS) {
	vertices = _cairo_malloc_ab (num_rects, sizeof (GLfloat) * 4 * 2);
	colors = _cairo_malloc_ab (num_rects, sizeof (GLfloat) * 4 * 4);
	if (!vertices || !colors) {
	    _cairo_gl_context_release (ctx);
	    tb_free (vertices);
	    tb_free (colors);
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);
	}
    } else {
	vertices = vertices_stack;
	colors = colors_stack;
    }

    /* This should be loaded in as either a blend constant and an operator
     * setup specific to this, or better, a fragment shader constant.
     */
    colors[0] = color->red * color->alpha;
    colors[1] = color->green * color->alpha;
    colors[2] = color->blue * color->alpha;
    colors[3] = color->alpha;
    for (i = 1; i < num_rects * 4; i++) {
	colors[i*4 + 0] = colors[0];
	colors[i*4 + 1] = colors[1];
	colors[i*4 + 2] = colors[2];
	colors[i*4 + 3] = colors[3];
    }

    for (i = 0; i < num_rects; i++) {
	vertices[i * 8 + 0] = rects[i].x;
	vertices[i * 8 + 1] = rects[i].y;
	vertices[i * 8 + 2] = rects[i].x + rects[i].width;
	vertices[i * 8 + 3] = rects[i].y;
	vertices[i * 8 + 4] = rects[i].x + rects[i].width;
	vertices[i * 8 + 5] = rects[i].y + rects[i].height;
	vertices[i * 8 + 6] = rects[i].x;
	vertices[i * 8 + 7] = rects[i].y + rects[i].height;
    }

    glVertexPointer (2, GL_FLOAT, sizeof (GLfloat)*2, vertices);
    glEnableClientState (GL_VERTEX_ARRAY);
    glColorPointer (4, GL_FLOAT, sizeof (GLfloat)*4, colors);
    glEnableClientState (GL_COLOR_ARRAY);

    glDrawArrays (GL_QUADS, 0, 4 * num_rects);

    glDisableClientState (GL_COLOR_ARRAY);
    glDisableClientState (GL_VERTEX_ARRAY);
    glDisable (GL_BLEND);

    _cairo_gl_context_release (ctx);
    if (vertices != vertices_stack)
	tb_free (vertices);
    if (colors != colors_stack)
	tb_free (colors);

    return CAIRO_STATUS_SUCCESS;
#undef N_STACK_RECTS
}

static cairo_int_status_t
_cairo_gl_surface_fill_rectangles_glsl (void                  *abstract_surface,
					cairo_operator_t       op,
					const cairo_color_t   *color,
					cairo_rectangle_int_t *rects,
					int                    num_rects)
{
#define N_STACK_RECTS 4
    cairo_gl_surface_t *surface = abstract_surface;
    GLfloat vertices_stack[N_STACK_RECTS*4*2];
    cairo_gl_context_t *ctx;
    int i;
    GLfloat *vertices;
    static const char *fill_vs_source =
	"void main()\n"
	"{\n"
	"	gl_Position = ftransform();\n"
	"}\n";
    static const char *fill_fs_source =
	"uniform vec4 color;\n"
	"void main()\n"
	"{\n"
	"	gl_FragColor = color;\n"
	"}\n";
    cairo_status_t status;

    if (! _cairo_gl_operator_is_supported (op))
	return UNSUPPORTED ("unsupported operator");

    status = _cairo_gl_context_acquire (surface->base.device, &ctx);
    if (unlikely (status))
	return status;

    status = create_shader_program (&ctx->fill_rectangles_shader,
				    fill_vs_source,
				    fill_fs_source);
    if (unlikely (status)) {
	_cairo_gl_context_release (ctx);
	return status;
    }

    if (num_rects > N_STACK_RECTS) {
	vertices = _cairo_malloc_ab (num_rects, sizeof (GLfloat) * 4 * 2);
	if (!vertices) {
	    _cairo_gl_context_release (ctx);
	    tb_free (vertices);
	    return _cairo_error (CAIRO_STATUS_NO_MEMORY);
	}
    } else {
	vertices = vertices_stack;
    }

    _cairo_gl_use_program (&ctx->fill_rectangles_shader);

    _cairo_gl_set_destination (surface);
    _cairo_gl_set_operator (surface, op, FALSE);

    status = bind_vec4_to_shader (ctx->fill_rectangles_shader.program,
                                  "color",
                                  color->red * color->alpha,
                                  color->green * color->alpha,
                                  color->blue * color->alpha,
                                  color->alpha);
    assert (! _cairo_status_is_error (status));

    for (i = 0; i < num_rects; i++) {
	vertices[i * 8 + 0] = rects[i].x;
	vertices[i * 8 + 1] = rects[i].y;
	vertices[i * 8 + 2] = rects[i].x + rects[i].width;
	vertices[i * 8 + 3] = rects[i].y;
	vertices[i * 8 + 4] = rects[i].x + rects[i].width;
	vertices[i * 8 + 5] = rects[i].y + rects[i].height;
	vertices[i * 8 + 6] = rects[i].x;
	vertices[i * 8 + 7] = rects[i].y + rects[i].height;
    }

    glVertexPointer (2, GL_FLOAT, sizeof (GLfloat)*2, vertices);
    glEnableClientState (GL_VERTEX_ARRAY);

    glDrawArrays (GL_QUADS, 0, 4 * num_rects);

    glDisableClientState (GL_VERTEX_ARRAY);
    glDisable (GL_BLEND);
    glUseProgramObjectARB (0);

    _cairo_gl_context_release (ctx);
    if (vertices != vertices_stack)
	tb_free (vertices);

    return CAIRO_STATUS_SUCCESS;
#undef N_STACK_RECTS
}


static cairo_int_status_t
_cairo_gl_surface_fill_rectangles (void			   *abstract_surface,
				   cairo_operator_t	    op,
				   const cairo_color_t     *color,
				   cairo_rectangle_int_t   *rects,
				   int			    num_rects)
{
    cairo_gl_surface_t *surface = abstract_surface;
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) surface->base.device;

    if (ctx->using_glsl) {
	return _cairo_gl_surface_fill_rectangles_glsl(abstract_surface,
						      op,
						      color,
						      rects,
						      num_rects);
    } else {
	return _cairo_gl_surface_fill_rectangles_fixed(abstract_surface,
						       op,
						       color,
						       rects,
						       num_rects);
    }
}

typedef struct _cairo_gl_surface_span_renderer {
    cairo_span_renderer_t base;

    cairo_gl_composite_setup_t setup;

    int xmin, xmax;

    cairo_operator_t op;
    cairo_antialias_t antialias;

    cairo_gl_surface_t *dst;
    cairo_gl_context_t *ctx;
    cairo_region_t *clip;

    void *vbo_base;
    unsigned int vbo_size;
    unsigned int vbo_offset;
    unsigned int vertex_size;
} cairo_gl_surface_span_renderer_t;

static void
_cairo_gl_span_renderer_flush (cairo_gl_surface_span_renderer_t *renderer)
{
    int count;

    if (renderer->vbo_offset == 0)
	return;

    glUnmapBufferARB (GL_ARRAY_BUFFER_ARB);

    count = renderer->vbo_offset / renderer->vertex_size;
    renderer->vbo_offset = 0;

    if (renderer->clip) {
	int i, num_rectangles = cairo_region_num_rectangles (renderer->clip);

	glEnable (GL_SCISSOR_TEST);
	for (i = 0; i < num_rectangles; i++) {
	    cairo_rectangle_int_t rect;

	    cairo_region_get_rectangle (renderer->clip, i, &rect);

	    glScissor (rect.x, rect.y, rect.width, rect.height);
	    glDrawArrays (GL_QUADS, 0, count);
	}
	glDisable (GL_SCISSOR_TEST);
    } else {
	glDrawArrays (GL_QUADS, 0, count);
    }
}

static void *
_cairo_gl_span_renderer_get_vbo (cairo_gl_surface_span_renderer_t *renderer,
				 unsigned int num_vertices)
{
    unsigned int offset;

    if (renderer->vbo_size == 0) {
	renderer->vbo_size = 16384;
	glBindBufferARB (GL_ARRAY_BUFFER_ARB, renderer->ctx->vbo);

	if (renderer->setup.src.type == OPERAND_TEXTURE)
	    renderer->vertex_size = 4 * sizeof (float) + sizeof (uint32_t);
	else
	    renderer->vertex_size = 2 * sizeof (float) + sizeof (uint32_t);

	glVertexPointer (2, GL_FLOAT, renderer->vertex_size, 0);
	glEnableClientState (GL_VERTEX_ARRAY);

	glColorPointer (4, GL_UNSIGNED_BYTE, renderer->vertex_size,
			(void *) (uintptr_t) (2 * sizeof (float)));
	glEnableClientState (GL_COLOR_ARRAY);

	if (renderer->setup.src.type == OPERAND_TEXTURE) {
	    glClientActiveTexture (GL_TEXTURE0);
	    glTexCoordPointer (2, GL_FLOAT, renderer->vertex_size,
			       (void *) (uintptr_t) (2 * sizeof (float) +
						     sizeof (uint32_t)));
	    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
	}
    }

    if (renderer->vbo_offset + num_vertices * renderer->vertex_size >
	renderer->vbo_size) {
	_cairo_gl_span_renderer_flush (renderer);
    }

    if (renderer->vbo_offset == 0) {
	/* We'll only be using these vertices once. */
	glBufferDataARB (GL_ARRAY_BUFFER_ARB, renderer->vbo_size, NULL,
		      GL_STREAM_DRAW_ARB);
	renderer->vbo_base = glMapBufferARB (GL_ARRAY_BUFFER_ARB,
					     GL_WRITE_ONLY_ARB);
    }

    offset = renderer->vbo_offset;
    renderer->vbo_offset += num_vertices * renderer->vertex_size;

    return (char *) renderer->vbo_base + offset;
}

static void
_cairo_gl_emit_span_vertex (cairo_gl_surface_span_renderer_t *renderer,
			    int dst_x, int dst_y, uint8_t alpha,
			    float *vertices)
{
    cairo_surface_attributes_t *src_attributes;
    int v = 0;

    src_attributes = &renderer->setup.src.operand.texture.attributes;

    vertices[v++] = dst_x + BIAS;
    vertices[v++] = dst_y + BIAS;
    vertices[v++] = int_as_float (alpha << 24);
    if (renderer->setup.src.type == OPERAND_TEXTURE) {
	double s, t;

	s = dst_x + BIAS;
	t = dst_y + BIAS;
	cairo_matrix_transform_point (&src_attributes->matrix, &s, &t);
	vertices[v++] = s;
	vertices[v++] = t;
    }
}

static void
_cairo_gl_emit_rectangle (cairo_gl_surface_span_renderer_t *renderer,
			  int x1, int y1,
			  int x2, int y2,
			  int coverage)
{
    float *vertices = _cairo_gl_span_renderer_get_vbo (renderer, 4);
    int vsize = renderer->vertex_size / 4;

    _cairo_gl_emit_span_vertex (renderer, x1, y1, coverage,
				vertices + vsize * 0);
    _cairo_gl_emit_span_vertex (renderer, x1, y2, coverage,
				vertices + vsize * 1);
    _cairo_gl_emit_span_vertex (renderer, x2, y2, coverage,
				vertices + vsize * 2);
    _cairo_gl_emit_span_vertex (renderer, x2, y1, coverage,
				vertices + vsize * 3);
}

static cairo_status_t
_cairo_gl_render_bounded_spans (void *abstract_renderer,
				int y, int height,
				const cairo_half_open_span_t *spans,
				unsigned num_spans)
{
    cairo_gl_surface_span_renderer_t *renderer = abstract_renderer;

    if (num_spans == 0)
	return CAIRO_STATUS_SUCCESS;

    do {
	if (spans[0].coverage) {
	    _cairo_gl_emit_rectangle (renderer,
				      spans[0].x, y,
				      spans[1].x, y + height,
				      spans[0].coverage);
	}

	spans++;
    } while (--num_spans > 1);

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
_cairo_gl_render_unbounded_spans (void *abstract_renderer,
				  int y, int height,
				  const cairo_half_open_span_t *spans,
				  unsigned num_spans)
{
    cairo_gl_surface_span_renderer_t *renderer = abstract_renderer;

    if (num_spans == 0) {
	_cairo_gl_emit_rectangle (renderer,
				  renderer->xmin, y,
				  renderer->xmax, y + height,
				  0);
	return CAIRO_STATUS_SUCCESS;
    }

    if (spans[0].x != renderer->xmin) {
	_cairo_gl_emit_rectangle (renderer,
				  renderer->xmin, y,
				  spans[0].x, y + height,
				  0);
    }

    do {
	_cairo_gl_emit_rectangle (renderer,
				  spans[0].x, y,
				  spans[1].x, y + height,
				  spans[0].coverage);
	spans++;
    } while (--num_spans > 1);

    if (spans[0].x != renderer->xmax) {
	_cairo_gl_emit_rectangle (renderer,
				  spans[0].x, y,
				  renderer->xmax, y + height,
				  0);
    }

    return CAIRO_STATUS_SUCCESS;
}

static void
_cairo_gl_surface_span_renderer_destroy (void *abstract_renderer)
{
    cairo_gl_surface_span_renderer_t *renderer = abstract_renderer;

    if (!renderer)
	return;

    _cairo_gl_operand_destroy (&renderer->setup.src);
    _cairo_gl_context_release (renderer->ctx);

    tb_free (renderer);
}

static cairo_status_t
_cairo_gl_surface_span_renderer_finish (void *abstract_renderer)
{
    cairo_gl_surface_span_renderer_t *renderer = abstract_renderer;

    _cairo_gl_span_renderer_flush (renderer);

    glBindBufferARB (GL_ARRAY_BUFFER_ARB, 0);
    glDisableClientState (GL_VERTEX_ARRAY);
    glDisableClientState (GL_COLOR_ARRAY);

    glClientActiveTexture (GL_TEXTURE0);
    glDisableClientState (GL_TEXTURE_COORD_ARRAY);
    glActiveTexture (GL_TEXTURE0);
    glDisable (GL_TEXTURE_1D);
    glDisable (renderer->ctx->tex_target);

    if (!renderer->setup.shader) {
	glActiveTexture (GL_TEXTURE1);
	glDisable (renderer->ctx->tex_target);
    }

    glDisable (GL_BLEND);
    _cairo_gl_use_program (NULL);

    return CAIRO_STATUS_SUCCESS;
}

static cairo_bool_t
_cairo_gl_surface_check_span_renderer (cairo_operator_t	  op,
				       const cairo_pattern_t  *pattern,
				       void			 *abstract_dst,
				       cairo_antialias_t	  antialias)
{
    if (! _cairo_gl_operator_is_supported (op))
	return FALSE;

    if (! GLEW_ARB_vertex_buffer_object)
	return FALSE;

    return TRUE;

    (void) pattern;
    (void) abstract_dst;
    (void) antialias;
}

static cairo_span_renderer_t *
_cairo_gl_surface_create_span_renderer (cairo_operator_t	 op,
					const cairo_pattern_t	*src,
					void			*abstract_dst,
					cairo_antialias_t	 antialias,
					const cairo_composite_rectangles_t *rects,
					cairo_region_t		*clip_region)
{
    cairo_gl_surface_t *dst = abstract_dst;
    cairo_gl_context_t *ctx = (cairo_gl_context_t *) dst->base.device;
    cairo_gl_surface_span_renderer_t *renderer;
    cairo_status_t status;
    cairo_surface_attributes_t *src_attributes;
    const cairo_rectangle_int_t *extents;
    GLenum err;

    renderer = tb_calloc (1, sizeof (*renderer));
    if (unlikely (renderer == NULL))
	return _cairo_span_renderer_create_in_error (CAIRO_STATUS_NO_MEMORY);

    renderer->base.destroy = _cairo_gl_surface_span_renderer_destroy;
    renderer->base.finish = _cairo_gl_surface_span_renderer_finish;
    if (rects->is_bounded) {
	renderer->base.render_rows = _cairo_gl_render_bounded_spans;
	extents = &rects->bounded;
    } else {
	renderer->base.render_rows = _cairo_gl_render_unbounded_spans;
	extents = &rects->unbounded;
    }
    renderer->xmin = extents->x;
    renderer->xmax = extents->x + extents->width;
    renderer->op = op;
    renderer->antialias = antialias;
    renderer->dst = dst;
    renderer->clip = clip_region;

    status = _cairo_gl_operand_init (&renderer->setup.src, src, dst,
				     rects->source.x, rects->source.y,
				     extents->x, extents->y,
				     extents->width, extents->height);
    if (unlikely (status)) {
	tb_free (renderer);
	return _cairo_span_renderer_create_in_error (status);
    }

    status = _cairo_gl_context_acquire (dst->base.device, &renderer->ctx);
    if (unlikely (status)) {
	_cairo_gl_operand_destroy (&renderer->setup.src);
	tb_free (renderer);
	return _cairo_span_renderer_create_in_error (status);
    }
    _cairo_gl_set_destination (dst);

    status = _cairo_gl_get_program (renderer->ctx,
				    renderer->setup.src.source,
				    CAIRO_GL_SHADER_MASK_SPANS,
				    CAIRO_GL_SHADER_IN_NORMAL,
				    &renderer->setup.shader);
    if (_cairo_status_is_error (status)) {
	_cairo_gl_operand_destroy (&renderer->setup.src);
	_cairo_gl_context_release (renderer->ctx);
	tb_free (renderer);
	return _cairo_span_renderer_create_in_error (status);
}

    src_attributes = &renderer->setup.src.operand.texture.attributes;

    _cairo_gl_use_program (renderer->setup.shader);
    _cairo_gl_set_operator (dst, op, FALSE);
    _cairo_gl_set_src_operand (renderer->ctx, &renderer->setup);

    if (!renderer->setup.shader) {
	/* Set up the mask to source from the incoming vertex color. */
	glActiveTexture (GL_TEXTURE1);
	/* Have to have a dummy texture bound in order to use the combiner unit. */
	glBindTexture (ctx->tex_target, renderer->ctx->dummy_tex);
	glEnable (ctx->tex_target);
	glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
	glTexEnvi (GL_TEXTURE_ENV, GL_COMBINE_ALPHA, GL_MODULATE);

	glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_RGB, GL_PREVIOUS);
	glTexEnvi (GL_TEXTURE_ENV, GL_SRC0_ALPHA, GL_PREVIOUS);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND0_ALPHA, GL_SRC_ALPHA);

	glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PRIMARY_COLOR);
	glTexEnvi (GL_TEXTURE_ENV, GL_SRC1_ALPHA, GL_PRIMARY_COLOR);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_ALPHA);
	glTexEnvi (GL_TEXTURE_ENV, GL_OPERAND1_ALPHA, GL_SRC_ALPHA);
    }

    while ((err = glGetError ()))
	fprintf (stderr, "GL error 0x%08x\n", (int) err);

    return &renderer->base;
}

static cairo_bool_t
_cairo_gl_surface_get_extents (void		     *abstract_surface,
			       cairo_rectangle_int_t *rectangle)
{
    cairo_gl_surface_t *surface = abstract_surface;

    rectangle->x = 0;
    rectangle->y = 0;
    rectangle->width  = surface->width;
    rectangle->height = surface->height;

    return TRUE;
}

static void
_cairo_gl_surface_get_font_options (void                  *abstract_surface,
				    cairo_font_options_t  *options)
{
    _cairo_font_options_init_default (options);

    cairo_font_options_set_hint_metrics (options, CAIRO_HINT_METRICS_ON);
}


static cairo_int_status_t
_cairo_gl_surface_paint (void *abstract_surface,
			 cairo_operator_t	 op,
			 const cairo_pattern_t *source,
			 cairo_clip_t	    *clip)
{
    /* simplify the common case of clearing the surface */
    if (op == CAIRO_OPERATOR_CLEAR && clip == NULL)
	return _cairo_gl_surface_clear (abstract_surface);

    return CAIRO_INT_STATUS_UNSUPPORTED;
}

const cairo_surface_backend_t _cairo_gl_surface_backend = {
    CAIRO_SURFACE_TYPE_GL,
    _cairo_gl_surface_create_similar,
    _cairo_gl_surface_finish,

    _cairo_gl_surface_acquire_source_image,
    _cairo_gl_surface_release_source_image,
    _cairo_gl_surface_acquire_dest_image,
    _cairo_gl_surface_release_dest_image,

    _cairo_gl_surface_clone_similar,
    _cairo_gl_surface_composite,
    _cairo_gl_surface_fill_rectangles,
    _cairo_gl_surface_composite_trapezoids,
    _cairo_gl_surface_create_span_renderer,
    _cairo_gl_surface_check_span_renderer,

    NULL, /* copy_page */
    NULL, /* show_page */
    _cairo_gl_surface_get_extents,
    NULL, /* old_show_glyphs */
    _cairo_gl_surface_get_font_options,
    NULL, /* flush */
    NULL, /* mark_dirty_rectangle */
    _cairo_gl_surface_scaled_font_fini,
    _cairo_gl_surface_scaled_glyph_fini,
    _cairo_gl_surface_paint,
    NULL, /* mask */
    NULL, /* stroke */
    NULL, /* fill */
    _cairo_gl_surface_show_glyphs, /* show_glyphs */
    NULL  /* snapshot */
};

/** Call glFinish(), used for accurate performance testing. */
cairo_status_t
cairo_gl_surface_glfinish (cairo_surface_t *surface)
{
    glFinish ();

    return CAIRO_STATUS_SUCCESS;
}
