#ifndef __DARRAY_H__
#define __DARRAY_H__

#include <stdint.h>

#define DARRAY_GROW 512

extern int DARRAY_COUNT;

struct darray
{
	char* buffer;
	size_t length;
	size_t capcity;
	size_t element_size;
	size_t cursor;
};

struct darray* darray_create(size_t element_size, size_t capacity);

void darray_destroy(struct darray* self);

// deprecated
void darray_init(struct darray* darray, size_t element_size, size_t capacity);

// deprecated
void darray_free(struct darray* darray);

void darray_append(struct darray* darray, void* value);

size_t darray_reserve(struct darray* darray);

void darray_set(struct darray* darray, size_t index, void* value);

void* darray_get(struct darray* darray, size_t index);

void darray_grow(struct darray* darray, size_t count);

void* darray_next(struct darray* darray);

void* darray_prev(struct darray* darray);

void darray_cursor_set(struct darray* darray, size_t index);

size_t darray_cursor_get(struct darray* darray);

void darray_empty(struct darray* darray);

#endif
