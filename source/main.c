#include "engine.h"

/*
#include <stdlib.h>
#include "lodepng.h"

float hue_to_rgb(float v1, float v2, float vh)
{
	if (vh < 0.0f)
	{
		vh += 1.0f;
	}

	if (vh > 1.0f)
	{
		vh -= 1.0f;
	}

	if ((6.0f * vh) < 1.0f)
	{
		return (v1 + (v2 - v1) * 6.0f * vh);
	}

	if ((2.0f * vh) < 1.0f)
	{
		return v2;
	}

	if ((3.0f * vh) < 2.0f)
	{
		return (v1 + (v2 - v1) * ((2.0f / 3.0f) - vh) * 6.0f);
	}

	return v1;
}

void create_hue_slider(void)
{
	size_t width = 240;
	size_t height = 17;

	size_t stride = 3 * width;

	unsigned char* data = malloc(stride * height);

	float H = 0.0f;
	float S = 0.5f;
	float L = 0.5f;

	float v1 = 0.0f;
	float v2 = 0.0f;

	unsigned char R = 0;
	unsigned char G = 0;
	unsigned char B = 0;

	for (int x = 0; x < width; x++)
	{
		H = (float)x / 240.0f;

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

			R = 255 * hue_to_rgb(v1, v2, H + (1.0f / 3.0f));
			G = 255 * hue_to_rgb(v1, v2, H);
			B = 255 * hue_to_rgb(v1, v2, H - (1.0f / 3.0f));
		}

		size_t xx = x * 3;

		for (int y = 0; y < height; y++)
		{
			size_t base = y * stride + xx;

			data[base + 0] = R;
			data[base + 1] = G;
			data[base + 2] = B;
		}
	}

	lodepng_encode24_file("hue_slider.png", data, width, height);

	free(data);
}

void create_selection_area(void)
{
	size_t width = 240;
	size_t height = 240;

	size_t stride = 3 * width;

	unsigned char* data = malloc(stride * height);

	float H = 0.0f;
	float S = 0.0f;
	float L = 0.0f;

	float v1 = 0.0f;
	float v2 = 0.0f;

	unsigned char R = 0;
	unsigned char G = 0;
	unsigned char B = 0;

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

				R = 255 * hue_to_rgb(v1, v2, H + (1.0f / 3.0f));
				G = 255 * hue_to_rgb(v1, v2, H);
				B = 255 * hue_to_rgb(v1, v2, H - (1.0f / 3.0f));
			}

			data[y * stride + x * 3 + 0] = R;
			data[y * stride + x * 3 + 1] = G;
			data[y * stride + x * 3 + 2] = B;
		}
	}

	lodepng_encode24_file("selection_area.png", data, width, height);

	free(data);
}
*/

/**
 * @mainpage
 * @brief VEdit begins execution in main.c by calling functions from engine.h
 *
 * engine.h contains all global state and starts the game loop.
 */

int main(int argc, char** argv)
{
	//create_hue_slider();
	//create_selection_area();

	struct engine* engine = engine_create();

	engine_loop(engine);

	return 0;
}
