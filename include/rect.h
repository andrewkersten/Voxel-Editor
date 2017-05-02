#pragma once

#include <stdbool.h>

struct rect
{
	int x;
	int y;
	int w;
	int h;
};

void rect_set(struct rect* self, int x, int y, int w, int h);

bool rect_contains(struct rect* self, int px, int py);
