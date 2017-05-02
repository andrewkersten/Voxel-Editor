#include "text.h"

struct text Text;

void text_init(void)
{
	if (FT_Init_FreeType(&Text.ftlib))
	{
		printf("FreeType Init Failed.\n");
		exit(0);
	}

	if (FT_New_Face(Text.ftlib, "assets/fonts/Inconsolata.otf", 0, &Text.face))
	{
		printf("FreeType could not open font.\n");
		exit(0);
	}

	Text.glyph = Text.face->glyph;

	FT_Set_Pixel_Sizes(Text.face, 0, 18);

	// ----------------------------------------

	glGenTextures(1, &Text.texture);

	glBindTexture(GL_TEXTURE_2D, Text.texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenVertexArrays(1, &Text.vao);

	glBindVertexArray(Text.vao);

	glGenBuffers(1, &Text.vbo);

	glBindBuffer(GL_ARRAY_BUFFER, Text.vbo);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void text_render2(const char* text, float x, float y)
{
	const char* c = NULL;

	glBindVertexArray(Text.vao);
	glBindBuffer(GL_ARRAY_BUFFER, Text.vbo);

	for (c = text; *c; c++)
	{
		if (FT_Load_Char(Text.face, *c, FT_LOAD_RENDER))
		{
			continue;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Text.glyph->bitmap.width, Text.glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, Text.glyph->bitmap.buffer);

		float y3 = y - Text.glyph->bitmap_top;
		float y4 = y3 + Text.glyph->bitmap.rows;

		
		float x2 = x + Text.glyph->bitmap_left;
		float y2 = y + Text.glyph->bitmap_top;// +Text.glyph->bitmap_top;
		float w = Text.glyph->bitmap.width;
		float h = Text.glyph->bitmap.rows;

		GLfloat box[4][4] = {
			{ x2,     y3    , 0, 0 },
			{ x2 + w, y3    , 1, 0 },
			{ x2,     y4, 0, 1 },
			{ x2 + w, y4, 1, 1 },
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += Text.glyph->advance.x >> 6;
		y += Text.glyph->advance.y >> 6;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void text_render(const char* text, float x, float y, float sx, float sy)
{
	const char* c = NULL;

	glBindVertexArray(Text.vao);
	glBindBuffer(GL_ARRAY_BUFFER, Text.vbo);

	for (c = text; *c; c++)
	{
		if (FT_Load_Char(Text.face, *c, FT_LOAD_RENDER))
		{
			continue;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, Text.glyph->bitmap.width, Text.glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, Text.glyph->bitmap.buffer);

		/*
		float x2 = x + Text.glyph->bitmap_left * sx;
		float y2 = -y - Text.glyph->bitmap_top * sy;
		float w = Text.glyph->bitmap.width * sx;
		float h = Text.glyph->bitmap.rows * sy;

		GLfloat box[4][4] = {
			{ x2,     -y2    , 0, 0 },
			{ x2 + w, -y2    , 1, 0 },
			{ x2,     -y2 - h, 0, 1 },
			{ x2 + w, -y2 - h, 1, 1 },
		};
		*/

		GLfloat box[4][4] = {
			{ 0, 0, 0, 0 },
			{ 100, 0, 1, 0 },
			{ 0, 100, 0, 1 },
			{ 100, 100, 1, 1 },
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += (Text.glyph->advance.x >> 6) * sx;
		y += (Text.glyph->advance.y >> 6) * sy;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}
