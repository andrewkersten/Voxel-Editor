#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <GL/glew.h>

//#include "camera.h"
#include "editor.h"
#include "keyboard.h"
#include "mouse.h"
#include "renderer.h"
#include "window.h"

struct engine
{
	struct window* window;
	struct keyboard* keyboard;
	struct mouse* mouse;

	struct editor* editor;

	//struct camera* camera;

	struct renderer* renderer;
};

struct engine* engine_create(void);

void engine_loop(struct engine* engine);

#endif
