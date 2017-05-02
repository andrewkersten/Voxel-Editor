#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include <GL/glew.h>

#include <ft2build.h>
#include FT_FREETYPE_H

struct text
{
	FT_Library ftlib;
	FT_Face face;
	FT_GlyphSlot glyph;

	GLuint texture;
	GLuint vao;
	GLuint vbo;
};

extern struct text Text;

void text_init(void);

void text_render2(const char* text, float x, float y);

void text_render(const char* text, float x, float y, float sx, float sy);

#endif
