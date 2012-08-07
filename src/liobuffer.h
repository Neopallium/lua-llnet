/***************************************************************************
 * Copyright (C) 2007-2010 by Robert G. Jakabosky <bobby@neoawareness.com> *
 *                                                                         *
 ***************************************************************************/
#if !defined(__L_IOBUFFER_H__)
#define __L_IOBUFFER_H__

#include "lcommon.h"

typedef struct LIOBuffer LIOBuffer;

struct LIOBuffer {
	uint8_t *buf;     /**< buffer. */
	size_t  size;     /**< size of data in buffer. */
	size_t  capacity; /**< memory size of buffer. */
};

L_LIB_API void l_iobuffer_init(LIOBuffer *buf, const uint8_t *data, size_t size);

L_LIB_API void l_iobuffer_init_cap(LIOBuffer *buf, size_t capacity);

L_LIB_API void l_iobuffer_cleanup(LIOBuffer *buf);

L_LIB_API void l_iobuffer_reset(LIOBuffer *buf);

L_LIB_API size_t l_iobuffer_size(LIOBuffer *buf);

L_LIB_API bool l_iobuffer_set_size(LIOBuffer *buf, size_t size);

L_LIB_API uint8_t *l_iobuffer_data(LIOBuffer *buf);

L_LIB_API size_t l_iobuffer_capacity(LIOBuffer *buf);

L_LIB_API bool l_iobuffer_set_capacity(LIOBuffer *buf, size_t capacity);

#endif /* __L_IOBUFFER_H__ */
