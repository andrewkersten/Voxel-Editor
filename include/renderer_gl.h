#pragma once

#include "renderer.h"

#include "shader_gl.h"
#include "texture_gl.h"
#include "vertex_gl.h"

#include "window.h"

#include <ft2build.h>
#include FT_FREETYPE_H

struct renderer_gl
{
	struct renderer base;

	GLKMatrix4 projection_2d;
	GLKMatrix4 projection_3d;

	struct shader_gl* shader_volume;
	struct shader_gl* shader_text;
	struct shader_gl* shader_cursor;
	struct shader_gl* shader_ui_color;

	struct texture_gl* texture_cursor;
	struct texture_gl* texture_panel;

	GLubyte* texture_selector_buffer;
	struct texture_gl* texture_selector;

	GLuint cursor_vao;
	GLuint cursor_vbo;

	GLuint panel_vao;
	GLuint panel_vbo;

	GLuint selector_vao;
	GLuint selector_vbo;

	GLuint preview_vao;
	GLuint preview_vbo;

	GLuint colorbar_vao;
	GLuint colorbar_vbo;

	FT_Library freetype;
	FT_Face font_face;
	FT_GlyphSlot font_glyph;

	GLuint font_vao;
	GLuint font_vbo;
	GLuint font_tex;
};

struct renderer_gl* renderer_gl_create(struct window* window);
