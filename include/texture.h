#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <GL/glew.h>

#define TEXTURES_ASSET_DIRECTORY "assets/textures/"

struct textures
{
	GLuint cursor;
	GLuint ground;
};

extern struct textures Textures;

void textures_load(void);

#endif
