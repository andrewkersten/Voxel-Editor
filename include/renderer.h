#pragma once

#include "GLKMath.h"

#include "editor.h"
//#include "hud.h"
#include "volume.h"

struct renderer;

typedef void(*renderer_destroy_func)(struct renderer* self);

typedef void(*renderer_clear_func)(struct renderer* self);

typedef unsigned char*(*renderer_screenshot_func)(struct renderer* self, size_t* width, size_t* height);

typedef void(*renderer_render_editor)(struct renderer* self, struct editor* editor);

//typedef void(*renderer_render_hud_func)(struct renderer* self, struct hud* hud);

typedef void(*renderer_render_volume_func)(struct renderer* self, struct volume* volume, struct transform* transform);

typedef void(*renderer_render_text_func)(struct renderer* self, const char* text, float x, float y);

//typedef void(*render_colorbar)(struct renderer* renderer, struct colorbar* colorbar);

struct renderer
{
	GLKMatrix4 matrix_view;

	renderer_destroy_func destroy;
	renderer_clear_func clear;
	renderer_screenshot_func screenshot;
	renderer_render_editor render_editor;
	//renderer_render_hud_func render_hud;
	renderer_render_volume_func render_volume;
	renderer_render_text_func render_text;
};
