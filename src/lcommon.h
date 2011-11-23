/***************************************************************************
 *   Copyright (C) 2011 by Robert G. Jakabosky <bobby@sharedrealm.com>     *
 *                                                                         *
 ***************************************************************************/
#if !defined(__L_COMMON_H__)
#define __L_COMMON_H__

#define L_LIB_API extern
#define L_INLINE static inline

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/user.h>

#ifdef NDEBUG
#define l_assert(expr) do { } while(0)
#define l_assert2(expr, str) do { } while(0)
#else
#define l_assert(expr) do { \
  if L_LIKELY(expr) { \
  } else { \
    fprintf(stderr, #expr); \
		abort(); \
  } \
} while(0)
#define l_assert2(expr, str) do { \
  if L_LIKELY(expr) { \
  } else { \
    fprintf(stderr, str ", " #expr); \
		abort(); \
  } \
} while(0)
#endif

#define l_strlen(str) \
	(__builtin_constant_p(str) ? (sizeof(str) - 1) : strlen(str))

#define L_GNUC_PACKED __attribute__((__packed__))
#define L_UNUSED(var) ((void)var)
#define L_FUNC_UNUSED __attribute__((unused))

/* L_LIKELY/L_UNLIKELY */
#if defined(__GNUC__) && (__GNUC__ > 2) && defined(__OPTIMIZE__)
#define L_LIKELY(exp) (__builtin_expect(((exp) ? 1 : 0), 1))
#define L_UNLIKELY(exp) (__builtin_expect(((exp) ? 1 : 0), 0))
#else
#define L_LIKELY(exp) (exp)
#define L_UNLIKELY(exp) (exp)
#endif

/* L_PAGE_SIZE */
#ifdef PAGE_SIZE
#define L_PAGE_SIZE PAGE_SIZE
#else
#define L_PAGE_SIZE (4 * 1024)
#endif

#endif /* __L_COMMON_H__ */
