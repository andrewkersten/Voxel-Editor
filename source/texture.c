#include "texture.h"

#include "lodepng.h"
#include "utility.h"

struct textures Textures;

static GLuint texture_load(char* filepath)
{
	unsigned error;
	unsigned char* image;
	unsigned width;
	unsigned height;

	error = lodepng_decode32_file(&image, &width, &height, filepath);

	if (error)
	{
		log_warning("Texture Error: %u - %s\n", error, lodepng_error_text(error));
		return 0;
	}

	/*
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned int width_bytes = width * 4;
	unsigned char temp = 0;

	for (int i = 0; i < height / 2; i++)
	{
		top = image + i * width_bytes;
		bottom = image + (height - i - 1) * width_bytes;

		for (int j = 0; j < width_bytes; j++)
		{
			temp = *top;
			*top = *bottom;
			*bottom = temp;

			top++;
			bottom++;
		}
	}
	*/

	GLuint result = 0;


	glGenTextures(1, &result);
	glBindTexture(GL_TEXTURE_2D, result);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(image);

	return result;
}

static GLuint texture_load_tile(char* filepath)
{
	unsigned error;
	unsigned char* image;
	unsigned width;
	unsigned height;

	error = lodepng_decode32_file(&image, &width, &height, filepath);

	if (error)
	{
		log_warning("Texture Error: %u - %s\n", error, lodepng_error_text(error));
		return 0;
	}

	/*
	unsigned char* top = NULL;
	unsigned char* bottom = NULL;
	unsigned int width_bytes = width * 4;
	unsigned char temp = 0;

	for (int i = 0; i < height / 2; i++)
	{
	top = image + i * width_bytes;
	bottom = image + (height - i - 1) * width_bytes;

	for (int j = 0; j < width_bytes; j++)
	{
	temp = *top;
	*top = *bottom;
	*bottom = temp;

	top++;
	bottom++;
	}
	}
	*/

	GLuint result = 0;


	glGenTextures(1, &result);
	glBindTexture(GL_TEXTURE_2D, result);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	free(image);

	return result;
}

void textures_load(void)
{
	Textures.cursor = texture_load(TEXTURES_ASSET_DIRECTORY "cursor.png");
	Textures.ground = texture_load_tile(TEXTURES_ASSET_DIRECTORY "ground.png");
}
