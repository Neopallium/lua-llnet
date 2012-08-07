/***************************************************************************
 * Copyright (C) 2007-2010 by Robert G. Jakabosky <bobby@neoawareness.com> *
 *                                                                         *
 ***************************************************************************/

#include "liobuffer.h"
#include <stdlib.h>
#include <string.h>

static bool l_iobuffer_resize_internal(LIOBuffer *buf, size_t capacity) {
	uint8_t *ptr;

	/* try to realloc buffer. */
	ptr = (uint8_t *)realloc(buf->buf, sizeof(uint8_t) * capacity);
	if(ptr == NULL && capacity > 0) {
		return false;
	}
	buf->buf = ptr;

	if(buf->_size > capacity) {
		buf->_size = capacity;
	}
	buf->_capacity = capacity;
	return true;
}

void l_iobuffer_init(LIOBuffer *buf, const uint8_t *data, size_t size) {
	assert(data != NULL);
	/* initialize buffer object. */
	buf->_size = size;
	buf->_capacity = 0;
	buf->buf = NULL;
	l_iobuffer_resize_internal(buf, size);
	/* copy data into buffer. */
	memcpy(buf->buf, data, size);
}

void l_iobuffer_init_cap(LIOBuffer *buf, size_t capacity) {
	/* initialize buffer object. */
	buf->_size = 0;
	buf->_capacity = 0;
	buf->buf = NULL;
	l_iobuffer_resize_internal(buf, capacity);
}

void l_iobuffer_cleanup(LIOBuffer *buf) {
	if(buf->buf) free(buf->buf);
	buf->buf = NULL;
	buf->_size = 0;
	buf->_capacity = 0;
}

void l_iobuffer_reset(LIOBuffer *buf) {
	buf->_size = 0;
}

size_t l_iobuffer_size(LIOBuffer *buf) {
	return buf->_size;
}

bool l_iobuffer_set_size(LIOBuffer *buf, size_t size) {
	size_t cap = buf->_capacity;
	if L_UNLIKELY(size > cap) {
		return l_iobuffer_resize_internal(buf, size);
	}
	buf->_size = size;
	return true;
}

size_t l_iobuffer_capacity(LIOBuffer *buf) {
	return buf->_capacity;
}

bool l_iobuffer_set_capacity(LIOBuffer *buf, size_t capacity) {
	return l_iobuffer_resize_internal(buf, capacity);
}

uint8_t *l_iobuffer_data(LIOBuffer *buf) {
	return buf->buf;
}

