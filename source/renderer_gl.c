#include "renderer_gl.h"

#include <assert.h>
#include <stdlib.h>

#include "color.h"

void destroy(struct renderer* base);

void clear_screen(struct renderer* base);

unsigned char* screenshot(struct renderer* base, size_t* width, size_t* height);

void render_editor(struct renderer* base, struct editor* editor);

void render_cursor(struct renderer_gl* self);

void render_panel(struct renderer_gl* self);

void render_selector(struct renderer_gl* self);

void render_preview(struct renderer_gl* self);

void render_colorbar(struct renderer_gl* self);

void render_volume(struct renderer* base, struct volume* volume, struct transform* transform);

void render_text(struct renderer* base, const char* text, float x, float y);

void init_opengl(struct renderer_gl* self);

void init_freetype(struct renderer_gl* self);

void create_shaders(struct renderer_gl* self);

void create_textures(struct renderer_gl* self);

void create_matrices(struct renderer_gl* self, struct window* window);

void create_cursor(struct renderer_gl* self);

void create_panel(struct renderer_gl* self);

void create_selector(struct renderer_gl* self);

void update_selector(struct renderer_gl* self, struct editor* editor);

void create_preview(struct renderer_gl* self);

void update_preview(struct renderer_gl* self, struct editor* editor);

void create_colorbar(struct renderer_gl* self);

void update_colorbar(struct renderer_gl* self, struct editor* editor);

struct renderer_gl* renderer_gl_create(struct window* window)
{
	assert(window);

	struct renderer_gl* self = calloc(1, sizeof(*self));

	assert(self);

	self->base.destroy = destroy;
	self->base.clear = clear_screen;
	self->base.screenshot = screenshot;
	self->base.render_editor = render_editor;
	self->base.render_volume = render_volume;
	self->base.render_text = render_text;

	init_opengl(self);
	init_freetype(self);

	create_shaders(self);
	create_textures(self);
	create_matrices(self, window);

	create_cursor(self);
	create_panel(self);
	create_selector(self);
	create_preview(self);
	create_colorbar(self);

	return self;
}

void destroy(struct renderer* base)
{
	assert(base);

	struct renderer_gl* self = (struct renderer_gl*) base;

	free(self);
}

void clear_screen(struct renderer* base)
{
	assert(base);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

unsigned char* screenshot(struct renderer* base, size_t* width, size_t* height)
{
	*width = 1440;
	*height = 900;

	size_t data_size = 3 * 1440 * 900;
	GLubyte* data = malloc(data_size);

	assert(data);

	if (data == NULL)
	{
		return NULL;
	}

	glReadPixels(0, 0, 1440, 900, GL_RGB, GL_UNSIGNED_BYTE, data);

	size_t stride = 3 * 1440;
	GLubyte* buffer = malloc(stride);

	assert(buffer);

	if (buffer == NULL)
	{
		return NULL;
	}

	for (int i = 0; i < 900 / 2; i++)
	{
		memcpy(buffer, data + i * stride, stride);
		memcpy(data + i * stride, data + data_size - (i + 1) * stride, stride);
		memcpy(data + data_size - (i + 1) * stride, buffer, stride);
	}

	free(buffer);

	return data;
}

void render_editor(struct renderer* base, struct editor* editor)
{
	assert(base);
	assert(editor);

	struct renderer_gl* self = (struct renderer_gl*) base;

	base->matrix_view = camera_look_through(editor->camera);

	render_volume(self, &editor->volume, &editor->transform);

	update_colorbar(self, editor);
	render_colorbar(self);

	if (editor->has_focus)
	{
		render_cursor(self);
	}
	else
	{
		render_panel(self);

		update_selector(self, editor);
		render_selector(self);

		update_preview(self, editor);
		render_preview(self);
	}
}

void render_cursor(struct renderer_gl* self)
{
	assert(self);

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(self->shader_cursor->handle);

	glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&self->projection_2d);

	glBindVertexArray(self->cursor_vao);

	glUniform1i(glGetUniformLocation(self->shader_cursor->handle, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->texture_cursor->handle);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);

	glEnable(GL_DEPTH_TEST);
}

void render_panel(struct renderer_gl* self)
{
	assert(self);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(self->shader_cursor->handle);

	glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&self->projection_2d);

	glBindVertexArray(self->panel_vao);

	glUniform1i(glGetUniformLocation(self->shader_cursor->handle, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->texture_panel->handle);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}

void render_selector(struct renderer_gl* self)
{
	assert(self);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(self->shader_cursor->handle);

	glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&self->projection_2d);

	glBindVertexArray(self->selector_vao);

	glUniform1i(glGetUniformLocation(self->shader_cursor->handle, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->texture_selector->handle);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindTexture(GL_TEXTURE_2D, 0);

	glEnable(GL_DEPTH_TEST);
}

void render_preview(struct renderer_gl* self)
{
	assert(self);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(self->shader_ui_color->handle);

	glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&self->projection_2d);

	glBindVertexArray(self->preview_vao);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glEnable(GL_DEPTH_TEST);
}

void render_colorbar(struct renderer_gl* self)
{
	assert(self);

	glDisable(GL_DEPTH_TEST);

	glUseProgram(self->shader_ui_color->handle);

	glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&self->projection_2d);

	glBindVertexArray(self->colorbar_vao);

	glDrawArrays(GL_TRIANGLES, 0, 120);

	glEnable(GL_DEPTH_TEST);
}

void render_volume(struct renderer* base, struct volume* volume, struct transform* transform)
{
	assert(base);
	assert(volume);

	struct renderer_gl* self = (struct renderer_gl*) base;

	if (volume->meshed)
	{
		volume_upload(volume);
	}

	glUseProgram(self->shader_volume->handle);

	glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&self->projection_3d);

	glBindVertexArray(volume->vao);

	GLKMatrix4 modelview;

	modelview = GLKMatrix4Multiply(base->matrix_view, transform->model_matrix);

	glUniformMatrix4fv(4, 1, GL_FALSE, (const GLfloat*)&modelview);

	glDrawArrays(GL_TRIANGLES, 0, volume->element_count);
}

void render_text(struct renderer* base, const char* text, float x, float y)
{
	assert(base);
	assert(text);

	struct renderer_gl* self = (struct renderer_gl*) base;

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glUseProgram(self->shader_text->handle);

	glUniformMatrix4fv(0, 1, GL_FALSE, (const GLfloat*)&self->projection_2d);

	glUniform1i(glGetUniformLocation(self->shader_text->handle, "tex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, self->font_tex);

	const char* c = NULL;

	glBindVertexArray(self->font_vao);
	glBindBuffer(GL_ARRAY_BUFFER, self->font_vbo);

	for (c = text; *c; c++)
	{
		if (FT_Load_Char(self->font_face, *c, FT_LOAD_RENDER))
		{
			continue;
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, self->font_glyph->bitmap.width, self->font_glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, self->font_glyph->bitmap.buffer);

		float y3 = y - self->font_glyph->bitmap_top;
		float y4 = y3 + self->font_glyph->bitmap.rows;


		float x2 = x + self->font_glyph->bitmap_left;
		float y2 = y + self->font_glyph->bitmap_top;// +Text.glyph->bitmap_top;
		float w = self->font_glyph->bitmap.width;
		float h = self->font_glyph->bitmap.rows;

		GLfloat box[4][4] = {
			{ x2,     y3    , 0, 0 },
			{ x2 + w, y3    , 1, 0 },
			{ x2,     y4, 0, 1 },
			{ x2 + w, y4, 1, 1 },
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_DYNAMIC_DRAW);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		x += self->font_glyph->advance.x >> 6;
		y += self->font_glyph->advance.y >> 6;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glUseProgram(0);
}

void init_opengl(struct renderer_gl* self)
{
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		//log_error_exit("Failed to initialize glew.");
	}

	//log_info("GL_VERSION: %s", glGetString(GL_VERSION));
	//log_info("GL_VENDOR: %s", glGetString(GL_VENDOR));
	//log_info("GL_RENDERER: %s", glGetString(GL_RENDERER));
	// log_info("GL_EXTENSIONS: %s", glGetString(GL_EXTENSIONS));

	glEnable(GL_MULTISAMPLE);

	glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glDisable(GL_CULL_FACE);

	glClearColor(0.49, 0.75, 0.93, 1.0);
}

void init_freetype(struct renderer_gl* self)
{
	if (FT_Init_FreeType(&self->freetype))
	{
		assert(false);
	}

	if (FT_New_Face(self->freetype, "assets/fonts/Inconsolata.otf", 0, &self->font_face))
	{
		assert(false);
	}

	self->font_glyph = self->font_face->glyph;

	FT_Set_Pixel_Sizes(self->font_face, 0, 18);

	glGenTextures(1, &self->font_tex);

	glBindTexture(GL_TEXTURE_2D, self->font_tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glBindTexture(GL_TEXTURE_2D, 0);

	glGenVertexArrays(1, &self->font_vao);

	glBindVertexArray(self->font_vao);

	glGenBuffers(1, &self->font_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, self->font_vbo);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void create_shaders(struct renderer_gl* self)
{
	assert(self);

	self->shader_volume = shader_gl_create(SHADER_GL_ASSET_DIRECTORY "voxel_v.glsl", SHADER_GL_ASSET_DIRECTORY "voxel_f.glsl");
	self->shader_cursor = shader_gl_create(SHADER_GL_ASSET_DIRECTORY "cursor_v.glsl", SHADER_GL_ASSET_DIRECTORY "cursor_f.glsl");
	self->shader_text = shader_gl_create(SHADER_GL_ASSET_DIRECTORY "text_v.glsl", SHADER_GL_ASSET_DIRECTORY "text_f.glsl");
	self->shader_ui_color = shader_gl_create(SHADER_GL_ASSET_DIRECTORY "interface_v.glsl", SHADER_GL_ASSET_DIRECTORY "interface_f.glsl");
}

void create_textures(struct renderer_gl* self)
{
	self->texture_cursor = texture_gl_create(TEXTURE_GL_ASSET_DIRECTORY "cursor.png");
	self->texture_panel = texture_gl_create(TEXTURE_GL_ASSET_DIRECTORY "panel_v2.png");

	self->texture_selector_buffer = malloc(240 * 240 * 4);
	assert(self->texture_selector_buffer);
	self->texture_selector = texture_gl_create2(240, 240, self->texture_selector_buffer);
}

void create_matrices(struct renderer_gl* self, struct window* window)
{
	assert(self);
	assert(window);

	self->projection_2d = GLKMatrix4MakeOrtho(0.0f, (float)window->width, (float)window->height, 0.0f, -1.0f, 1.0f);
	self->projection_3d = GLKMatrix4MakePerspective(GLKMathDegreesToRadians(45.0f), (float)window->width / (float)window->height, 1.0f, 1000.0f);
}

void create_cursor(struct renderer_gl* self)
{
	struct vertex_ui_texture vertices[6];

	GLfloat small_x = 712.0f;
	GLfloat small_y = 442.0f;
	GLfloat width = 16.0f;
	GLfloat height = 16.0f;

	vertices[0].position.x = small_x;
	vertices[0].position.y = small_y + height;
	vertices[0].texcoord.u = 0.0f;
	vertices[0].texcoord.v = 0.0f;

	vertices[1].position.x = small_x;
	vertices[1].position.y = small_y;
	vertices[1].texcoord.u = 0.0f;
	vertices[1].texcoord.v = 1.0f;

	vertices[2].position.x = small_x + width;
	vertices[2].position.y = small_y;
	vertices[2].texcoord.u = 1.0f;
	vertices[2].texcoord.v = 1.0f;

	vertices[3].position.x = small_x;
	vertices[3].position.y = small_y + height;
	vertices[3].texcoord.u = 0.0f;
	vertices[3].texcoord.v = 0.0f;

	vertices[4].position.x = small_x + width;
	vertices[4].position.y = small_y;
	vertices[4].texcoord.u = 1.0f;
	vertices[4].texcoord.v = 1.0f;

	vertices[5].position.x = small_x + width;
	vertices[5].position.y = small_y + height;
	vertices[5].texcoord.u = 1.0f;
	vertices[5].texcoord.v = 0.0f;

	glGenVertexArrays(1, &self->cursor_vao);
	glBindVertexArray(self->cursor_vao);

	glGenBuffers(1, &self->cursor_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, self->cursor_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(struct vertex_ui_texture), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_texture), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_texture), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void create_panel(struct renderer_gl* self)
{
	struct vertex_ui_texture vertices[6];

	GLfloat small_x = 0.0f;
	GLfloat small_y = 0.0f;
	GLfloat width = 280.0f;
	GLfloat height = 900.0f;

	vertices[0].position.x = small_x;
	vertices[0].position.y = small_y + height;
	vertices[0].texcoord.u = 0.0f;
	vertices[0].texcoord.v = 1.0f;

	vertices[1].position.x = small_x;
	vertices[1].position.y = small_y;
	vertices[1].texcoord.u = 0.0f;
	vertices[1].texcoord.v = 0.0f;

	vertices[2].position.x = small_x + width;
	vertices[2].position.y = small_y;
	vertices[2].texcoord.u = 1.0f;
	vertices[2].texcoord.v = 0.0f;

	vertices[3].position.x = small_x;
	vertices[3].position.y = small_y + height;
	vertices[3].texcoord.u = 0.0f;
	vertices[3].texcoord.v = 1.0f;

	vertices[4].position.x = small_x + width;
	vertices[4].position.y = small_y;
	vertices[4].texcoord.u = 1.0f;
	vertices[4].texcoord.v = 0.0f;

	vertices[5].position.x = small_x + width;
	vertices[5].position.y = small_y + height;
	vertices[5].texcoord.u = 1.0f;
	vertices[5].texcoord.v = 1.0f;

	glGenVertexArrays(1, &self->panel_vao);
	glBindVertexArray(self->panel_vao);

	glGenBuffers(1, &self->panel_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, self->panel_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(struct vertex_ui_texture), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_texture), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_texture), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void create_selector(struct renderer_gl* self)
{
	struct vertex_ui_texture vertices[6];

	GLfloat small_x = 20.0f;
	GLfloat small_y = 20.0f;
	GLfloat width = 240.0f;
	GLfloat height = 240.0f;

	vertices[0].position.x = small_x;
	vertices[0].position.y = small_y + height;
	vertices[0].texcoord.u = 0.0f;
	vertices[0].texcoord.v = 1.0f;

	vertices[1].position.x = small_x;
	vertices[1].position.y = small_y;
	vertices[1].texcoord.u = 0.0f;
	vertices[1].texcoord.v = 0.0f;

	vertices[2].position.x = small_x + width;
	vertices[2].position.y = small_y;
	vertices[2].texcoord.u = 1.0f;
	vertices[2].texcoord.v = 0.0f;

	vertices[3].position.x = small_x;
	vertices[3].position.y = small_y + height;
	vertices[3].texcoord.u = 0.0f;
	vertices[3].texcoord.v = 1.0f;

	vertices[4].position.x = small_x + width;
	vertices[4].position.y = small_y;
	vertices[4].texcoord.u = 1.0f;
	vertices[4].texcoord.v = 0.0f;

	vertices[5].position.x = small_x + width;
	vertices[5].position.y = small_y + height;
	vertices[5].texcoord.u = 1.0f;
	vertices[5].texcoord.v = 1.0f;

	glGenVertexArrays(1, &self->selector_vao);
	glBindVertexArray(self->selector_vao);

	glGenBuffers(1, &self->selector_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, self->selector_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(struct vertex_ui_texture), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_texture), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_texture), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void update_selector(struct renderer_gl* self, struct editor* editor)
{
	size_t width = 240;
	size_t height = 240;
	size_t stride = width * 4;

	//float H = 0.0f;
	float S = 0.0f;
	float L = 0.0f;

	float selected_H = editor->panel->selector_H;

	float v1 = 0.0f;
	float v2 = 0.0f;

	unsigned char R = 0;
	unsigned char G = 0;
	unsigned char B = 0;
	unsigned char A = 255;

	for (int x = 0; x < width; x++)
	{
		for (int y = 0; y < height; y++)
		{
			S = (float)x / (float)width;
			L = 1.0f - (float)y / (float)height;

			if (S == 0.0f)
			{
				R = L * 255;
				G = L * 255;
				B = L * 255;
			}
			else
			{
				if (L < 0.5f)
				{
					v2 = L * (1.0f + S);
				}
				else
				{
					v2 = (L + S) - (S * L);
				}

				v1 = 2.0f * L - v2;

				R = 255 * hue_to_rgb(v1, v2, selected_H + (1.0f / 3.0f));
				G = 255 * hue_to_rgb(v1, v2, selected_H);
				B = 255 * hue_to_rgb(v1, v2, selected_H - (1.0f / 3.0f));
			}

			self->texture_selector_buffer[y * stride + x * 4 + 0] = R;
			self->texture_selector_buffer[y * stride + x * 4 + 1] = G;
			self->texture_selector_buffer[y * stride + x * 4 + 2] = B;
			self->texture_selector_buffer[y * stride + x * 4 + 3] = A;
		}
	}

	glBindTexture(GL_TEXTURE_2D, self->texture_selector->handle);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 240, 240, GL_RGBA, GL_UNSIGNED_BYTE, self->texture_selector_buffer);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void create_preview(struct renderer_gl* self)
{
	assert(self);

	struct vertex_ui_color vertices[6];

	GLfloat small_x = 20.0f;
	GLfloat small_y = 340.0f;
	GLfloat width = 240.0f;
	GLfloat height = 40.0f;

	vertices[0].position.x = small_x;
	vertices[0].position.y = small_y + height;
	vertices[0].color.r = 0.0f;
	vertices[0].color.g = 0.0f;
	vertices[0].color.b = 0.0f;
	vertices[0].color.a = 1.0f;

	vertices[1].position.x = small_x;
	vertices[1].position.y = small_y;
	vertices[1].color.r = 0.0f;
	vertices[1].color.g = 0.0f;
	vertices[1].color.b = 0.0f;
	vertices[1].color.a = 1.0f;

	vertices[2].position.x = small_x + width;
	vertices[2].position.y = small_y;
	vertices[2].color.r = 0.0f;
	vertices[2].color.g = 0.0f;
	vertices[2].color.b = 0.0f;
	vertices[2].color.a = 1.0f;

	vertices[3].position.x = small_x;
	vertices[3].position.y = small_y + height;
	vertices[3].color.r = 0.0f;
	vertices[3].color.g = 0.0f;
	vertices[3].color.b = 0.0f;
	vertices[3].color.a = 1.0f;

	vertices[4].position.x = small_x + width;
	vertices[4].position.y = small_y;
	vertices[4].color.r = 0.0f;
	vertices[4].color.g = 0.0f;
	vertices[4].color.b = 0.0f;
	vertices[4].color.a = 1.0f;

	vertices[5].position.x = small_x + width;
	vertices[5].position.y = small_y + height;
	vertices[5].color.r = 0.0f;
	vertices[5].color.g = 0.0f;
	vertices[5].color.b = 0.0f;
	vertices[5].color.a = 1.0f;

	glGenVertexArrays(1, &self->preview_vao);
	glBindVertexArray(self->preview_vao);

	glGenBuffers(1, &self->preview_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, self->preview_vbo);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(struct vertex_ui_color), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_color), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_color), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void update_preview(struct renderer_gl* self, struct editor* editor)
{
	assert(self);
	assert(editor);

	float H = editor->panel->selector_H;
	float S = editor->panel->selector_S;
	float L = editor->panel->selector_L;

	float v1 = 0.0f;
	float v2 = 0.0f;

	float R = 0.0f;
	float G = 0.0f;
	float B = 0.0f;

	if (S == 0.0f)
	{
		R = L;
		G = L;
		B = L;
	}
	else
	{
		if (L < 0.5f)
		{
			v2 = L * (1.0f + S);
		}
		else
		{
			v2 = (L + S) - (S * L);
		}

		v1 = 2.0f * L - v2;

		R = hue_to_rgb(v1, v2, H + (1.0f / 3.0f));
		G = hue_to_rgb(v1, v2, H);
		B = hue_to_rgb(v1, v2, H - (1.0f / 3.0f));
	}

	editor->selector_r = R;
	editor->selector_g = G;
	editor->selector_b = B;

	struct vertex_ui_color vertices[6];

	GLfloat small_x = 20.0f;
	GLfloat small_y = 340.0f;
	GLfloat width = 240.0f;
	GLfloat height = 40.0f;

	vertices[0].position.x = small_x;
	vertices[0].position.y = small_y + height;
	vertices[0].color.r = R;
	vertices[0].color.g = G;
	vertices[0].color.b = B;
	vertices[0].color.a = 1.0f;

	vertices[1].position.x = small_x;
	vertices[1].position.y = small_y;
	vertices[1].color.r = R;
	vertices[1].color.g = G;
	vertices[1].color.b = B;
	vertices[1].color.a = 1.0f;

	vertices[2].position.x = small_x + width;
	vertices[2].position.y = small_y;
	vertices[2].color.r = R;
	vertices[2].color.g = G;
	vertices[2].color.b = B;
	vertices[2].color.a = 1.0f;

	vertices[3].position.x = small_x;
	vertices[3].position.y = small_y + height;
	vertices[3].color.r = R;
	vertices[3].color.g = G;
	vertices[3].color.b = B;
	vertices[3].color.a = 1.0f;

	vertices[4].position.x = small_x + width;
	vertices[4].position.y = small_y;
	vertices[4].color.r = R;
	vertices[4].color.g = G;
	vertices[4].color.b = B;
	vertices[4].color.a = 1.0f;

	vertices[5].position.x = small_x + width;
	vertices[5].position.y = small_y + height;
	vertices[5].color.r = R;
	vertices[5].color.g = G;
	vertices[5].color.b = B;
	vertices[5].color.a = 1.0f;

	glBindBuffer(GL_ARRAY_BUFFER, self->preview_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 6 * sizeof(struct vertex_ui_color), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void create_colorbar(struct renderer_gl* self)
{
	assert(self);

	glGenVertexArrays(1, &self->colorbar_vao);
	glBindVertexArray(self->colorbar_vao);

	glGenBuffers(1, &self->colorbar_vbo);

	glBindBuffer(GL_ARRAY_BUFFER, self->colorbar_vbo);
	glBufferData(GL_ARRAY_BUFFER, 120 * sizeof(struct vertex_ui_color), NULL, GL_STREAM_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_color), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(struct vertex_ui_color), (GLvoid*)(2 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void update_colorbar(struct renderer_gl* self, struct editor* editor)
{
	assert(self);
	assert(editor);

	struct vertex_ui_color vertices[120];

	int width = 1440;
	int height = 900;

	float left = 523;
	float right = 523 + 34;
	float top = 843;
	float bottom = 843 + 34;

	for (int i = 0; i < 10; i++)
	{
		if (i == editor->colorbar->selected_index)
		{
			top -= 2;
			bottom += 2;
			left -= 2;
			right += 2;
		}

		vertices[i * 6 + 0].position.x = left;
		vertices[i * 6 + 0].position.y = bottom;
		vertices[i * 6 + 0].color.r = 0.0f;
		vertices[i * 6 + 0].color.g = 0.0f;
		vertices[i * 6 + 0].color.b = 0.0f;
		vertices[i * 6 + 0].color.a = 1.0f;

		vertices[i * 6 + 1].position.x = left;
		vertices[i * 6 + 1].position.y = top;
		vertices[i * 6 + 1].color.r = 0.0f;
		vertices[i * 6 + 1].color.g = 0.0f;
		vertices[i * 6 + 1].color.b = 0.0f;
		vertices[i * 6 + 1].color.a = 1.0f;

		vertices[i * 6 + 2].position.x = right;
		vertices[i * 6 + 2].position.y = top;
		vertices[i * 6 + 2].color.r = 0.0f;
		vertices[i * 6 + 2].color.g = 0.0f;
		vertices[i * 6 + 2].color.b = 0.0f;
		vertices[i * 6 + 2].color.a = 1.0f;

		vertices[i * 6 + 3].position.x = left;
		vertices[i * 6 + 3].position.y = bottom;
		vertices[i * 6 + 3].color.r = 0.0f;
		vertices[i * 6 + 3].color.g = 0.0f;
		vertices[i * 6 + 3].color.b = 0.0f;
		vertices[i * 6 + 3].color.a = 1.0f;

		vertices[i * 6 + 4].position.x = right;
		vertices[i * 6 + 4].position.y = top;
		vertices[i * 6 + 4].color.r = 0.0f;
		vertices[i * 6 + 4].color.g = 0.0f;
		vertices[i * 6 + 4].color.b = 0.0f;
		vertices[i * 6 + 4].color.a = 1.0f;

		vertices[i * 6 + 5].position.x = right;
		vertices[i * 6 + 5].position.y = bottom;
		vertices[i * 6 + 5].color.r = 0.0f;
		vertices[i * 6 + 5].color.g = 0.0f;
		vertices[i * 6 + 5].color.b = 0.0f;
		vertices[i * 6 + 5].color.a = 1.0f;

		if (i == editor->colorbar->selected_index)
		{
			top += 2;
			bottom -= 2;
			left += 2;
			right -= 2;
		}

		left += 40;
		right += 40;
	}

	left = 524;
	right = 524 + 32;
	top = 844;
	bottom = 844 + 32;

	struct color* colors = editor->colorbar->colors;

	for (int i = 10; i < 20; i++)
	{
		vertices[i * 6 + 0].position.x = left;
		vertices[i * 6 + 0].position.y = bottom;
		vertices[i * 6 + 0].color.r = colors[i - 10].r;
		vertices[i * 6 + 0].color.g = colors[i - 10].g;
		vertices[i * 6 + 0].color.b = colors[i - 10].b;
		vertices[i * 6 + 0].color.a = colors[i - 10].a;

		vertices[i * 6 + 1].position.x = left;
		vertices[i * 6 + 1].position.y = top;
		vertices[i * 6 + 1].color.r = colors[i - 10].r;
		vertices[i * 6 + 1].color.g = colors[i - 10].g;
		vertices[i * 6 + 1].color.b = colors[i - 10].b;
		vertices[i * 6 + 1].color.a = colors[i - 10].a;

		vertices[i * 6 + 2].position.x = right;
		vertices[i * 6 + 2].position.y = top;
		vertices[i * 6 + 2].color.r = colors[i - 10].r;
		vertices[i * 6 + 2].color.g = colors[i - 10].g;
		vertices[i * 6 + 2].color.b = colors[i - 10].b;
		vertices[i * 6 + 2].color.a = colors[i - 10].a;

		vertices[i * 6 + 3].position.x = left;
		vertices[i * 6 + 3].position.y = bottom;
		vertices[i * 6 + 3].color.r = colors[i - 10].r;
		vertices[i * 6 + 3].color.g = colors[i - 10].g;
		vertices[i * 6 + 3].color.b = colors[i - 10].b;
		vertices[i * 6 + 3].color.a = colors[i - 10].a;

		vertices[i * 6 + 4].position.x = right;
		vertices[i * 6 + 4].position.y = top;
		vertices[i * 6 + 4].color.r = colors[i - 10].r;
		vertices[i * 6 + 4].color.g = colors[i - 10].g;
		vertices[i * 6 + 4].color.b = colors[i - 10].b;
		vertices[i * 6 + 4].color.a = colors[i - 10].a;

		vertices[i * 6 + 5].position.x = right;
		vertices[i * 6 + 5].position.y = bottom;
		vertices[i * 6 + 5].color.r = colors[i - 10].r;
		vertices[i * 6 + 5].color.g = colors[i - 10].g;
		vertices[i * 6 + 5].color.b = colors[i - 10].b;
		vertices[i * 6 + 5].color.a = colors[i - 10].a;

		left += 40;
		right += 40;
	}

	glBindBuffer(GL_ARRAY_BUFFER, self->colorbar_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 120 * sizeof(struct vertex_ui_color), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
