#pragma once

#include "rect.h"

#include <assert.h>

void rect_set(struct rect* self, int x, int y, int w, int h)
{
	assert(self);

	self->x = x;
	self->y = y;
	self->w = w;
	self->h = h;
}

bool rect_contains(struct rect* self, int px, int py)
{
	assert(self);

	if (self->x <= px && self->x + self->w >= px && self->y <= py && self->y + self->h >= py)
	{
		return true;
	}

	return false;
}
