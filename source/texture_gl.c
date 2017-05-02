#include "texture_gl.h"

#include "lodepng.h"

#include <assert.h>
#include <stdlib.h>

struct texture_gl* texture_gl_create(const char* path)
{
	struct texture_gl* self = calloc(1, sizeof(*self));
	assert(self);

	unsigned error;
	unsigned char* image;
	unsigned width;
	unsigned height;

	error = lodepng_decode32_file(&image, &width, &height, path);

	if (error)
	{
		//log_warning("Texture Error: %u - %s\n", error, lodepng_error_text(error));
		return self;
	}


	glGenTextures(1, &self->handle);
	glBindTexture(GL_TEXTURE_2D, self->handle);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(image);

	return self;
}

struct texture_gl* texture_gl_create2(int width, int height, void* data)
{
	struct texture_gl* self = calloc(1, sizeof(*self));

	assert(self);

	glGenTextures(1, &self->handle);
	glBindTexture(GL_TEXTURE_2D, self->handle);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);

	return self;
}

void texture_gl_destroy(struct texture_gl* self)
{
	assert(self);

	glDeleteTextures(1, &self->handle);

	free(self);
}
