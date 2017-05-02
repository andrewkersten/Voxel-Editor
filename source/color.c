#pragma once

#include "color.h"

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
