#include "darray.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct darray* darray_create(size_t element_size, size_t capacity)
{
	struct darray* self = calloc(1, sizeof(*self));

	// TODO: validate allocation

	self->element_size = element_size;
	self->capcity = capacity;
	self->length = 0;
	self->cursor = 0;

	self->buffer = malloc(element_size * capacity);

	// TODO: validate allocation

	return self;
}

void darray_destroy(struct darray* self)
{
	assert(self);

	if (self->buffer)
	{
		free(self->buffer);
	}

	free(self);
}

void darray_init(struct darray* darray, size_t element_size, size_t capacity)
{
	darray->element_size = element_size;
	darray->capcity = capacity;
	darray->length = 0;
	darray->cursor = 0;

	darray->buffer = malloc(element_size * capacity);

	if (darray->buffer == NULL)
	{
		// ERROR: Allocation error.
		abort();
	}
}

void darray_free(struct darray* darray)
{
	free(darray->buffer);
}

void darray_append(struct darray* darray, void* value)
{
	if (darray->length >= darray->capcity)
	{
		darray_grow(darray, DARRAY_GROW);
	}

	if (value != NULL)
	{
		darray_set(darray, darray->length, value);
	}

	darray->length++;
}

size_t darray_reserve(struct darray* darray)
{
	if (darray->length >= darray->capcity)
	{
		darray_grow(darray, DARRAY_GROW);
	}

	size_t result = darray->length;

	darray->length++;

	return result;
}

void darray_set(struct darray* darray, size_t index, void* value)
{
	memcpy(darray->buffer + darray->element_size * index, value, darray->element_size);
}

void* darray_get(struct darray* darray, size_t index)
{
	if (index < darray->length)
	{
		return darray->buffer + darray->element_size * index;
	}

	return NULL;
}

void darray_grow(struct darray* darray, size_t count)
{
	void* buffer = darray->buffer;

	darray->buffer = realloc(darray->buffer, (darray->capcity + count) * darray->element_size);

	if (darray->buffer == NULL)
	{
		// ERROR: Allocation failure.
		abort();
	}

	darray->capcity += count;
}

void* darray_next(struct darray* darray)
{
	return darray_get(darray, darray->cursor++);
}

void* darray_prev(struct darray* darray)
{
	return darray_get(darray, darray->cursor--);
}

void darray_cursor_set(struct darray* darray, size_t index)
{
	darray->cursor = index;
}

size_t darray_cursor_get(struct darray* darray)
{
	return darray->cursor;
}

void darray_empty(struct darray* darray)
{
	darray->length = 0;
	darray->cursor = 0;
}
