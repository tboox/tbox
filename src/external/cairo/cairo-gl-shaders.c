/* cairo - a vector graphics library with display and print output
 *
 * Copyright © 2010 Eric Anholt
 * Copyright © 2009 T. Zachary Laine
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
 * The Initial Developer of the Original Code is T. Zachary Laine.
 */

#include "cairoint.h"
#include "cairo-gl-private.h"
#include "cairo-error-private.h"

typedef struct _shader_impl {
    cairo_status_t
    (*compile_shader) (GLuint *shader, GLenum type, const char *text);

    cairo_status_t
    (*link_shader) (GLuint *program, GLuint vert, GLuint frag);

    void
    (*destroy_shader_program) (cairo_gl_shader_program_t *program);

    cairo_status_t
    (*bind_float_to_shader) (GLuint program, const char *name,
                             float value);

    cairo_status_t
    (*bind_vec2_to_shader) (GLuint program, const char *name,
                            float value0, float value1);

    cairo_status_t
    (*bind_vec3_to_shader) (GLuint program, const char *name,
                            float value0, float value1,
                            float value2);

    cairo_status_t
    (*bind_vec4_to_shader) (GLuint program, const char *name,
                            float value0, float value1,
                            float value2, float value3);

    cairo_status_t
    (*bind_matrix_to_shader) (GLuint program, const char *name, cairo_matrix_t* m);

    cairo_status_t
    (*bind_texture_to_shader) (GLuint program, const char *name, GLuint tex_unit);

    void
    (*use_program) (cairo_gl_shader_program_t *program);
} shader_impl_t;

/* ARB_shader_objects / ARB_vertex_shader / ARB_fragment_shader extensions
   API. */
static cairo_status_t
compile_shader_arb (GLuint *shader, GLenum type, const char *text)
{
    const char* strings[1] = { text };
    GLint gl_status;

    *shader = glCreateShaderObjectARB (type);
    glShaderSourceARB (*shader, 1, strings, 0);
    glCompileShaderARB (*shader);
    glGetObjectParameterivARB (*shader, GL_OBJECT_COMPILE_STATUS_ARB, &gl_status);
    if (gl_status == GL_FALSE) {
        GLint log_size;
        glGetObjectParameterivARB (*shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_size);
        if (0 < log_size) {
            char *log = _cairo_malloc (log_size);
            GLint chars;

            log[log_size - 1] = '\0';
            glGetInfoLogARB (*shader, log_size, &chars, log);
            printf ("OpenGL shader compilation failed.  Shader:\n"
                    "%s\n"
                    "OpenGL compilation log:\n"
                    "%s\n",
                    text, log);

            tb_free (log);
        } else {
            printf ("OpenGL shader compilation failed.\n");
        }

        return CAIRO_INT_STATUS_UNSUPPORTED;
    }

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
link_shader_arb (GLuint *program, GLuint vert, GLuint frag)
{
    GLint gl_status;

    *program = glCreateProgramObjectARB ();
    glAttachObjectARB (*program, vert);
    glAttachObjectARB (*program, frag);
    glLinkProgramARB (*program);
    glGetObjectParameterivARB (*program, GL_OBJECT_LINK_STATUS_ARB, &gl_status);
    if (gl_status == GL_FALSE) {
        GLint log_size;
        glGetObjectParameterivARB (*program, GL_OBJECT_INFO_LOG_LENGTH_ARB, &log_size);
        if (0 < log_size) {
            char *log = _cairo_malloc (log_size);
            GLint chars;

            log[log_size - 1] = '\0';
            glGetInfoLogARB (*program, log_size, &chars, log);
            printf ("OpenGL shader link failed:\n%s\n", log);

            tb_free (log);
        } else {
            printf ("OpenGL shader link failed.\n");
        }

        return CAIRO_INT_STATUS_UNSUPPORTED;
    }

    return CAIRO_STATUS_SUCCESS;
}

static void
destroy_shader_program_arb (cairo_gl_shader_program_t *program)
{
    if (program->vertex_shader)
        glDeleteObjectARB (program->vertex_shader);
    if (program->fragment_shader)
        glDeleteObjectARB (program->fragment_shader);
    if (program->program)
        glDeleteObjectARB (program->program);
}

static cairo_status_t
bind_float_to_shader_arb (GLuint program, const char *name,
                               float value)
{
    GLint location = glGetUniformLocationARB (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform1fARB (location, value);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_vec2_to_shader_arb (GLuint program, const char *name,
                              float value0, float value1)
{
    GLint location = glGetUniformLocationARB (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform2fARB (location, value0, value1);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_vec3_to_shader_arb (GLuint program, const char *name,
                              float value0, float value1,
                              float value2)
{
    GLint location = glGetUniformLocationARB (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform3fARB (location, value0, value1, value2);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_vec4_to_shader_arb (GLuint program, const char *name,
                              float value0, float value1,
                              float value2, float value3)
{
    GLint location = glGetUniformLocationARB (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform4fARB (location, value0, value1, value2, value3);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_matrix_to_shader_arb (GLuint program, const char *name, cairo_matrix_t* m)
{
    GLint location = glGetUniformLocationARB (program, name);
    float gl_m[16] = {
        m->xx, m->xy, 0,     m->x0,
        m->yx, m->yy, 0,     m->y0,
        0,     0,     1,     0,
        0,     0,     0,     1
    };
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniformMatrix4fvARB (location, 1, GL_TRUE, gl_m);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_texture_to_shader_arb (GLuint program, const char *name, GLuint tex_unit)
{
    GLint location = glGetUniformLocationARB (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform1iARB (location, tex_unit);
    return CAIRO_STATUS_SUCCESS;
}

static void
use_program_arb (cairo_gl_shader_program_t *program)
{
    if (program)
	glUseProgramObjectARB (program->program);
    else
	glUseProgramObjectARB (0);
}

/* OpenGL Core 2.0 API. */
static cairo_status_t
compile_shader_core_2_0 (GLuint *shader, GLenum type, const char *text)
{
    const char* strings[1] = { text };
    GLint gl_status;

    *shader = glCreateShader (type);
    glShaderSource (*shader, 1, strings, 0);
    glCompileShader (*shader);
    glGetShaderiv (*shader, GL_COMPILE_STATUS, &gl_status);
    if (gl_status == GL_FALSE) {
        GLint log_size;
        glGetShaderiv (*shader, GL_INFO_LOG_LENGTH, &log_size);
        if (0 < log_size) {
            char *log = _cairo_malloc (log_size);
            GLint chars;

            log[log_size - 1] = '\0';
            glGetShaderInfoLog (*shader, log_size, &chars, log);
            printf ("OpenGL shader compilation failed.  Shader:\n"
                    "%s\n"
                    "OpenGL compilation log:\n"
                    "%s\n",
                    text, log);

            tb_free (log);
        } else {
            printf ("OpenGL shader compilation failed.\n");
        }

        return CAIRO_INT_STATUS_UNSUPPORTED;
    }

    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
link_shader_core_2_0 (GLuint *program, GLuint vert, GLuint frag)
{
    GLint gl_status;

    *program = glCreateProgram ();
    glAttachShader (*program, vert);
    glAttachShader (*program, frag);
    glLinkProgram (*program);
    glGetProgramiv (*program, GL_LINK_STATUS, &gl_status);
    if (gl_status == GL_FALSE) {
        GLint log_size;
        glGetProgramiv (*program, GL_INFO_LOG_LENGTH, &log_size);
        if (0 < log_size) {
            char *log = _cairo_malloc (log_size);
            GLint chars;

            log[log_size - 1] = '\0';
            glGetProgramInfoLog (*program, log_size, &chars, log);
            printf ("OpenGL shader link failed:\n%s\n", log);

            tb_free (log);
        } else {
            printf ("OpenGL shader link failed.\n");
        }

        return CAIRO_INT_STATUS_UNSUPPORTED;
    }

    return CAIRO_STATUS_SUCCESS;
}

static void
destroy_shader_program_core_2_0 (cairo_gl_shader_program_t *program)
{
    glDeleteShader (program->vertex_shader);
    glDeleteShader (program->fragment_shader);
    glDeleteProgram (program->program);
}

static cairo_status_t
bind_float_to_shader_core_2_0 (GLuint program, const char *name,
                               float value)
{
    GLint location = glGetUniformLocation (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform1f (location, value);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_vec2_to_shader_core_2_0 (GLuint program, const char *name,
                              float value0, float value1)
{
    GLint location = glGetUniformLocation (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform2f (location, value0, value1);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_vec3_to_shader_core_2_0 (GLuint program, const char *name,
                              float value0, float value1,
                              float value2)
{
    GLint location = glGetUniformLocation (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform3f (location, value0, value1, value2);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_vec4_to_shader_core_2_0 (GLuint program, const char *name,
                              float value0, float value1,
                              float value2, float value3)
{
    GLint location = glGetUniformLocation (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform4f (location, value0, value1, value2, value3);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_matrix_to_shader_core_2_0 (GLuint program, const char *name, cairo_matrix_t* m)
{
    GLint location = glGetUniformLocation (program, name);
    float gl_m[16] = {
        m->xx, m->xy, 0,     m->x0,
        m->yx, m->yy, 0,     m->y0,
        0,     0,     1,     0,
        0,     0,     0,     1
    };
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniformMatrix4fv (location, 1, GL_TRUE, gl_m);
    return CAIRO_STATUS_SUCCESS;
}

static cairo_status_t
bind_texture_to_shader_core_2_0 (GLuint program, const char *name, GLuint tex_unit)
{
    GLint location = glGetUniformLocation (program, name);
    if (location == -1)
        return CAIRO_INT_STATUS_UNSUPPORTED;
    glUniform1i (location, tex_unit);
    return CAIRO_STATUS_SUCCESS;
}

static void
use_program_core_2_0 (cairo_gl_shader_program_t *program)
{
    if (program)
	glUseProgram (program->program);
    else
	glUseProgram (0);
}

static const shader_impl_t shader_impl_core_2_0 = {
    compile_shader_core_2_0,
    link_shader_core_2_0,
    destroy_shader_program_core_2_0,
    bind_float_to_shader_core_2_0,
    bind_vec2_to_shader_core_2_0,
    bind_vec3_to_shader_core_2_0,
    bind_vec4_to_shader_core_2_0,
    bind_matrix_to_shader_core_2_0,
    bind_texture_to_shader_core_2_0,
    use_program_core_2_0,
};

static const shader_impl_t shader_impl_arb = {
    compile_shader_arb,
    link_shader_arb,
    destroy_shader_program_arb,
    bind_float_to_shader_arb,
    bind_vec2_to_shader_arb,
    bind_vec3_to_shader_arb,
    bind_vec4_to_shader_arb,
    bind_matrix_to_shader_arb,
    bind_texture_to_shader_arb,
    use_program_arb,
};

static const shader_impl_t*
get_impl (void)
{
    /* XXX multiple device support? */
    if (GLEW_VERSION_2_0) {
        return &shader_impl_core_2_0;
    } else if (GLEW_ARB_shader_objects &&
               GLEW_ARB_fragment_shader &&
               GLEW_ARB_vertex_program) {
        return &shader_impl_arb;
    }

    return NULL;
}

void
init_shader_program (cairo_gl_shader_program_t *program)
{
    program->vertex_shader = 0;
    program->fragment_shader = 0;
    program->program = 0;
    program->build_failure = FALSE;
}

void
destroy_shader_program (cairo_gl_shader_program_t *program)
{
    return get_impl()->destroy_shader_program(program);
}

cairo_status_t
create_shader_program (cairo_gl_shader_program_t *program,
                       const char *vertex_text,
                       const char *fragment_text)
{
    cairo_status_t status;
    const shader_impl_t *impl;

    if (program->program != 0)
        return CAIRO_STATUS_SUCCESS;

    if (program->build_failure)
        return CAIRO_INT_STATUS_UNSUPPORTED;

    impl = get_impl ();
    if (impl == NULL)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    status = impl->compile_shader (&program->vertex_shader,
				   GL_VERTEX_SHADER,
				   vertex_text);
    if (unlikely (status))
        goto FAILURE;

    status = impl->compile_shader (&program->fragment_shader,
				   GL_FRAGMENT_SHADER,
				   fragment_text);
    if (unlikely (status))
        goto FAILURE;

    status = impl->link_shader (&program->program,
				program->vertex_shader,
				program->fragment_shader);
    if (unlikely (status))
        goto FAILURE;

    return CAIRO_STATUS_SUCCESS;

 FAILURE:
    destroy_shader_program (program);
    program->vertex_shader = 0;
    program->fragment_shader = 0;
    program->program = 0;
    program->build_failure = TRUE;

    return CAIRO_INT_STATUS_UNSUPPORTED;
}

cairo_status_t
bind_float_to_shader (GLuint program, const char *name,
                      float value)
{
    return get_impl()->bind_float_to_shader(program, name, value);
}

cairo_status_t
bind_vec2_to_shader (GLuint program, const char *name,
                     float value0, float value1)
{
    return get_impl()->bind_vec2_to_shader(program, name, value0, value1);
}

cairo_status_t
bind_vec3_to_shader (GLuint program, const char *name,
                     float value0, float value1,
                     float value2)
{
    return get_impl()->bind_vec3_to_shader(program, name, value0, value1, value2);
}

cairo_status_t
bind_vec4_to_shader (GLuint program, const char *name,
                     float value0, float value1,
                     float value2, float value3)
{
    return get_impl()->bind_vec4_to_shader(program, name, value0, value1, value2, value3);
}

cairo_status_t
bind_matrix_to_shader (GLuint program, const char *name, cairo_matrix_t* m)
{
    return get_impl()->bind_matrix_to_shader(program, name, m);
}

cairo_status_t
bind_texture_to_shader (GLuint program, const char *name, GLuint tex_unit)
{
    return get_impl()->bind_texture_to_shader(program, name, tex_unit);
}

void
_cairo_gl_use_program (cairo_gl_shader_program_t *program)
{
    const shader_impl_t *impl;

    impl = get_impl ();
    if (impl == NULL)
	return;

    impl->use_program (program);
}

static const char *vs_no_coords =
    "void main()\n"
    "{\n"
    "	gl_Position = ftransform();\n"
    "}\n";
static const char *vs_source_coords =
    "varying vec2 source_texcoords;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = ftransform();\n"
    "	source_texcoords = gl_MultiTexCoord0.xy;\n"
    "}\n";
static const char *vs_mask_coords =
    "varying vec2 mask_texcoords;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = ftransform();\n"
    "	mask_texcoords = gl_MultiTexCoord1.xy;\n"
    "}\n";
static const char *vs_source_mask_coords =
    "varying vec2 source_texcoords;\n"
    "varying vec2 mask_texcoords;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = ftransform();\n"
    "	source_texcoords = gl_MultiTexCoord0.xy;\n"
    "	mask_texcoords = gl_MultiTexCoord1.xy;\n"
    "}\n";
static const char *vs_spans_no_coords =
    "varying float coverage;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = ftransform();\n"
    "   coverage = gl_Color.a;\n"
    "}\n";
static const char *vs_spans_source_coords =
    "varying vec2 source_texcoords;\n"
    "varying float coverage;\n"
    "void main()\n"
    "{\n"
    "	gl_Position = ftransform();\n"
    "	source_texcoords = gl_MultiTexCoord0.xy;\n"
    "   coverage = gl_Color.a;\n"
    "}\n";
static const char *fs_source_constant =
    "uniform vec4 constant_source;\n"
    "vec4 get_source()\n"
    "{\n"
    "	return constant_source;\n"
    "}\n";
static const char *fs_source_texture =
    "uniform sampler2D source_sampler;\n"
    "varying vec2 source_texcoords;\n"
    "vec4 get_source()\n"
    "{\n"
    "	return texture2D(source_sampler, source_texcoords);\n"
    "}\n";
static const char *fs_source_texture_rect =
    "uniform sampler2DRect source_sampler;\n"
    "varying vec2 source_texcoords;\n"
    "vec4 get_source()\n"
    "{\n"
    "	return texture2DRect(source_sampler, source_texcoords);\n"
    "}\n";
static const char *fs_source_texture_alpha =
    "uniform sampler2D source_sampler;\n"
    "varying vec2 source_texcoords;\n"
    "vec4 get_source()\n"
    "{\n"
    "	return vec4(0, 0, 0, texture2D(source_sampler, source_texcoords).a);\n"
    "}\n";
static const char *fs_source_texture_alpha_rect =
    "uniform sampler2DRect source_sampler;\n"
    "varying vec2 source_texcoords;\n"
    "vec4 get_source()\n"
    "{\n"
    "	return vec4(0, 0, 0, texture2DRect(source_sampler, source_texcoords).a);\n"
    "}\n";
static const char *fs_source_linear_gradient =
    "uniform sampler1D source_sampler;\n"
    "uniform mat4 source_matrix;\n"
    "uniform vec2 source_segment;\n"
    "\n"
    "vec4 get_source()\n"
    "{\n"
    "    vec2 pos = (source_matrix * vec4 (gl_FragCoord.xy, 0.0, 1.0)).xy;\n"
    "    float t = dot (pos, source_segment) / dot (source_segment, source_segment);\n"
    "    return texture1D (source_sampler, t);\n"
    "}\n";
static const char *fs_source_radial_gradient =
    "uniform sampler1D source_sampler;\n"
    "uniform mat4 source_matrix;\n"
    "uniform vec2 source_circle_1;\n"
    "uniform float source_radius_0;\n"
    "uniform float source_radius_1;\n"
    "\n"
    "vec4 get_source()\n"
    "{\n"
    "    vec2 pos = (source_matrix * vec4 (gl_FragCoord.xy, 0.0, 1.0)).xy;\n"
    "    \n"
    "    float dr = source_radius_1 - source_radius_0;\n"
    "    float dot_circle_1 = dot (source_circle_1, source_circle_1);\n"
    "    float dot_pos_circle_1 = dot (pos, source_circle_1);\n"
    "    \n"
    "    float A = dot_circle_1 - dr * dr;\n"
    "    float B = -2.0 * (dot_pos_circle_1 + source_radius_0 * dr);\n"
    "    float C = dot (pos, pos) - source_radius_0 * source_radius_0;\n"
    "    float det = B * B - 4.0 * A * C;\n"
    "    det = max (det, 0.0);\n"
    "    \n"
    "    float sqrt_det = sqrt (det);\n"
    "    sqrt_det *= sign(A);\n"
    "    \n"
    "    float t = (-B + sqrt_det) / (2.0 * A);\n"
    "    return texture1D (source_sampler, t);\n"
    "}\n";
static const char *fs_mask_constant =
    "uniform vec4 constant_mask;\n"
    "vec4 get_mask()\n"
    "{\n"
    "	return constant_mask;\n"
    "}\n";
static const char *fs_mask_texture =
    "uniform sampler2D mask_sampler;\n"
    "varying vec2 mask_texcoords;\n"
    "vec4 get_mask()\n"
    "{\n"
    "	return texture2D(mask_sampler, mask_texcoords);\n"
    "}\n";
static const char *fs_mask_texture_rect =
    "uniform sampler2DRect mask_sampler;\n"
    "varying vec2 mask_texcoords;\n"
    "vec4 get_mask()\n"
    "{\n"
    "	return texture2DRect(mask_sampler, mask_texcoords);\n"
    "}\n";
static const char *fs_mask_texture_alpha =
    "uniform sampler2D mask_sampler;\n"
    "varying vec2 mask_texcoords;\n"
    "vec4 get_mask()\n"
    "{\n"
    "	return vec4(0, 0, 0, texture2D(mask_sampler, mask_texcoords).a);\n"
    "}\n";
static const char *fs_mask_texture_alpha_rect =
    "uniform sampler2DRect mask_sampler;\n"
    "varying vec2 mask_texcoords;\n"
    "vec4 get_mask()\n"
    "{\n"
    "	return vec4(0, 0, 0, texture2DRect(mask_sampler, mask_texcoords).a);\n"
    "}\n";
static const char *fs_mask_linear_gradient =
    "uniform sampler1D mask_sampler;\n"
    "uniform mat4 mask_matrix;\n"
    "uniform vec2 mask_segment;\n"
    "\n"
    "vec4 get_mask()\n"
    "{\n"
    "    vec2 pos = (mask_matrix * vec4 (gl_FragCoord.xy, 0.0, 1.0)).xy;\n"
    "    float t = dot (pos, mask_segment) / dot (mask_segment, mask_segment);\n"
    "    return texture1D (mask_sampler, t);\n"
    "}\n";
static const char *fs_mask_radial_gradient =
    "uniform sampler1D mask_sampler;\n"
    "uniform mat4 mask_matrix;\n"
    "uniform vec2 mask_circle_1;\n"
    "uniform float mask_radius_0;\n"
    "uniform float mask_radius_1;\n"
    "\n"
    "vec4 get_mask()\n"
    "{\n"
    "    vec2 pos = (mask_matrix * vec4 (gl_FragCoord.xy, 0.0, 1.0)).xy;\n"
    "    \n"
    "    float dr = mask_radius_1 - mask_radius_0;\n"
    "    float dot_circle_1 = dot (mask_circle_1, mask_circle_1);\n"
    "    float dot_pos_circle_1 = dot (pos, mask_circle_1);\n"
    "    \n"
    "    float A = dot_circle_1 - dr * dr;\n"
    "    float B = -2.0 * (dot_pos_circle_1 + mask_radius_0 * dr);\n"
    "    float C = dot (pos, pos) - mask_radius_0 * mask_radius_0;\n"
    "    float det = B * B - 4.0 * A * C;\n"
    "    det = max (det, 0.0);\n"
    "    \n"
    "    float sqrt_det = sqrt (det);\n"
    "    sqrt_det *= sign(A);\n"
    "    \n"
    "    float t = (-B + sqrt_det) / (2.0 * A);\n"
    "    return texture1D (mask_sampler, t);\n"
    "}\n";
static const char *fs_mask_none =
    "vec4 get_mask()\n"
    "{\n"
    "	return vec4(0, 0, 0, 1);\n"
    "}\n";
static const char *fs_mask_spans =
    "varying float coverage;\n"
    "vec4 get_mask()\n"
    "{\n"
    "	return vec4(0, 0, 0, coverage);\n"
    "}\n";
static const char *fs_in_normal =
    "void main()\n"
    "{\n"
    "	gl_FragColor = get_source() * get_mask().a;\n"
    "}\n";
static const char *fs_in_component_alpha_source =
    "void main()\n"
    "{\n"
    "	gl_FragColor = get_source() * get_mask();\n"
    "}\n";
static const char *fs_in_component_alpha_alpha =
    "void main()\n"
    "{\n"
    "	gl_FragColor = get_source().a * get_mask();\n"
    "}\n";

/**
 * This function reduces the GLSL program combinations we compile when
 * there are non-functional differences.
 */
static cairo_gl_shader_program_t *
_cairo_gl_select_program (cairo_gl_context_t *ctx,
			  cairo_gl_shader_source_t source,
			  cairo_gl_shader_mask_t mask,
			  cairo_gl_shader_in_t in)
{
    if (in == CAIRO_GL_SHADER_IN_NORMAL &&
	mask == CAIRO_GL_SHADER_MASK_TEXTURE_ALPHA)
    {
	mask = CAIRO_GL_SHADER_MASK_TEXTURE;
    }
    if (in == CAIRO_GL_SHADER_IN_CA_SOURCE_ALPHA &&
	source == CAIRO_GL_SHADER_SOURCE_TEXTURE_ALPHA)
    {
	source = CAIRO_GL_SHADER_SOURCE_TEXTURE;
    }

    return &ctx->shaders[source][mask][in];
}

cairo_status_t
_cairo_gl_get_program (cairo_gl_context_t *ctx,
		       cairo_gl_shader_source_t source,
		       cairo_gl_shader_mask_t mask,
		       cairo_gl_shader_in_t in,
		       cairo_gl_shader_program_t **out_program)
{
    const char *source_sources[CAIRO_GL_SHADER_SOURCE_COUNT] = {
	fs_source_constant,
	fs_source_texture,
	fs_source_texture_alpha,
	fs_source_linear_gradient,
	fs_source_radial_gradient,
    };
    const char *mask_sources[CAIRO_GL_SHADER_MASK_COUNT] = {
	fs_mask_constant,
	fs_mask_texture,
	fs_mask_texture_alpha,
	fs_mask_linear_gradient,
	fs_mask_radial_gradient,
	fs_mask_none,
	fs_mask_spans,
    };
    const char *in_sources[CAIRO_GL_SHADER_IN_COUNT] = {
	fs_in_normal,
	fs_in_component_alpha_source,
	fs_in_component_alpha_alpha,
    };
    cairo_gl_shader_program_t *program;
    const char *source_source, *mask_source, *in_source;
    const char *vs_source;
    char *fs_source;
    cairo_status_t status;

    program = _cairo_gl_select_program(ctx, source, mask, in);
    if (program->program) {
	*out_program = program;
	return CAIRO_STATUS_SUCCESS;
    }

    if (program->build_failure)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    if (get_impl () == NULL)
	return CAIRO_INT_STATUS_UNSUPPORTED;

    source_source = source_sources[source];
    mask_source = mask_sources[mask];
    in_source = in_sources[in];

    /* For ARB_texture_rectangle, rewrite sampler2D and texture2D to
     * sampler2DRect and texture2DRect.
     */
    if (ctx->tex_target == GL_TEXTURE_RECTANGLE_EXT) {
	if (source_source == fs_source_texture)
	    source_source = fs_source_texture_rect;
	else if (source_source == fs_source_texture_alpha)
	    source_source = fs_source_texture_alpha_rect;

	if (mask_source == fs_mask_texture)
	    mask_source = fs_mask_texture_rect;
	else if (mask_source == fs_mask_texture_alpha)
	    mask_source = fs_mask_texture_alpha_rect;
    }

    fs_source = _cairo_malloc (strlen(source_source) +
			       strlen(mask_source) +
			       strlen(in_source) +
			       1);
    if (unlikely (fs_source == NULL))
	return _cairo_error (CAIRO_STATUS_NO_MEMORY);

    if (source == CAIRO_GL_SHADER_SOURCE_CONSTANT ||
	source == CAIRO_GL_SHADER_SOURCE_LINEAR_GRADIENT ||
	source == CAIRO_GL_SHADER_SOURCE_RADIAL_GRADIENT) {
	if (mask == CAIRO_GL_SHADER_MASK_SPANS)
	    vs_source = vs_spans_no_coords;
	else if (mask == CAIRO_GL_SHADER_MASK_CONSTANT ||
		 mask == CAIRO_GL_SHADER_MASK_LINEAR_GRADIENT ||
		 mask == CAIRO_GL_SHADER_MASK_RADIAL_GRADIENT)
	    vs_source = vs_no_coords;
	else
	    vs_source = vs_mask_coords;
    } else {
	if (mask == CAIRO_GL_SHADER_MASK_SPANS)
	    vs_source = vs_spans_source_coords;
	else if (mask == CAIRO_GL_SHADER_MASK_CONSTANT ||
		 mask == CAIRO_GL_SHADER_MASK_LINEAR_GRADIENT ||
		 mask == CAIRO_GL_SHADER_MASK_RADIAL_GRADIENT)
	    vs_source = vs_source_coords;
	else
	    vs_source = vs_source_mask_coords;
    }

    sprintf(fs_source, "%s%s%s", source_source, mask_source, in_source);

    init_shader_program (program);
    status = create_shader_program (program,
				    vs_source,
				    fs_source);
    tb_free (fs_source);

    if (_cairo_status_is_error (status))
	return status;

    _cairo_gl_use_program (program);
    if (source != CAIRO_GL_SHADER_SOURCE_CONSTANT) {
	status = bind_texture_to_shader (program->program, "source_sampler", 0);
	assert (!_cairo_status_is_error (status));
    }
    if (mask != CAIRO_GL_SHADER_MASK_CONSTANT &&
	mask != CAIRO_GL_SHADER_MASK_SPANS &&
	mask != CAIRO_GL_SHADER_MASK_NONE) {
	status = bind_texture_to_shader (program->program, "mask_sampler", 1);
	assert (!_cairo_status_is_error (status));
    }

    _cairo_gl_use_program (NULL);

    *out_program = program;
    return CAIRO_STATUS_SUCCESS;
}
