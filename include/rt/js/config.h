#pragma once

#include <nautilus/libccompat.h>
#include <nautilus/math.h>
#include <nautilus/naut_string.h>
#include <nautilus/naut_types.h>
#include <nautilus/nautilus.h>

#define PRINT(fmt, args...) nk_vc_printf(fmt, ##args)
#define PUTC(c) nk_vc_putchar(c)
#define printf PRINT
#define fprintf(f, fmt, args...) PRINT(fmt, ##args)
#define _printf PRINT
#define _fprintf fprintf
#define _putc(c, stdout) PUTC(c)

#define stdin ((void *)0UL)
#define stdout ((void *)1UL)
#define stderr ((void *)2UL)

#define putc _putc

// typedef char int8_t;
typedef sint16_t int16_t;
typedef sint32_t int32_t;
typedef sint64_t int64_t;

// bc missing pthread std lib
#define pthread_cond_t unsigned
#define pthread_mutex_t spinlock_t
#define PTHREAD_MUTEX_INITIALIZER SPINLOCK_INITIALIZER

// guessing a value
#define _SC_OPEN_MAX 4096

struct timeval {
  uint64_t tv_sec;
  uint64_t tv_usec;
};

#define NAN 0.0 / 0.0
#define INFINITY 1.0 / 0.0
#define NEG_INF -1.0 / 0.0

// Floating point environment compilation patch
// TODO: it is very likely we need to better implement this
#define FE_TONEAREST 0
#define FE_DOWNWARD 0
#define FE_UPWARD 0

// Threading compilation patch. This is a TODO
#define ETIMEDOUT 1

// typedef _Bool BOOL;

static inline uint32_t log2(const uint32_t x) {
  uint32_t y;
  asm("\tbsr %1, %0\n" : "=r"(y) : "r"(x));
  return y;
}

#define DBL_EPSILON 2.2204460492503131e-16

#define ML_POSINF (1.0 / 0.0)
#define ML_NEGINF ((-1.0) / 0.0)
#define ML_NAN (0.0 / 0.0)

#define ML_ERR_return_NAN                                                      \
  {                                                                            \
    ML_ERROR(ME_DOMAIN, "");                                                   \
    return ML_NAN;                                                             \
  }

#define ME_NONE 0
/*	no error */
#define ME_DOMAIN 1
/*	argument out of domain */
#define ME_RANGE 2
/*	value out of range */
#define ME_NOCONV 4
/*	process did not converge */
#define ME_PRECISION 8
/*	does not have "full" precision */
#define ME_UNDERFLOW 16
/*	and underflow occured (important for IEEE)*/

//# define MATHLIB_WARNING(fmt,x)		warning(fmt,x)

#define ML_ERROR(x, s)                                                         \
  {                                                                            \
    if (x > ME_DOMAIN) {                                                       \
      char *msg = "";                                                          \
      switch (x) {                                                             \
      case ME_DOMAIN:                                                          \
        msg = _("argument out of domain in '%s'\n");                           \
        break;                                                                 \
      case ME_RANGE:                                                           \
        msg = _("value out of range in '%s'\n");                               \
        break;                                                                 \
      case ME_NOCONV:                                                          \
        msg = _("convergence failed in '%s'\n");                               \
        break;                                                                 \
      case ME_PRECISION:                                                       \
        msg = _("full precision may not have been achieved in '%s'\n");        \
        break;                                                                 \
      case ME_UNDERFLOW:                                                       \
        msg = _("underflow occurred in '%s'\n");                               \
        break;                                                                 \
      }                                                                        \
      MATHLIB_WARNING(msg, s);                                                 \
    }                                                                          \
  }

double log1p(double);

static const unsigned B1 = 715094163, /* B1 = (682-0.03306235651)*2**20 */
    B2 = 696219795;                   /* B2 = (664-0.03306235651)*2**20 */

static const double
    C = 5.42857142857142815906e-01,  /* 19/35     = 0x3FE15F15, 0xF15F15F1 */
    D = -7.05306122448979611050e-01, /* -864/1225 = 0xBFE691DE, 0x2532C834 */
    E = 1.41428571428571436819e+00,  /* 99/70     = 0x3FF6A0EA, 0x0EA0EA0F */
    F = 1.60714285714285720630e+00,  /* 45/28     = 0x3FF9B6DB, 0x6DB6DB6E */
    G = 3.57142857142857150787e-01;  /* 5/14      = 0x3FD6DB6D, 0xB6DB6DB7 */

double cbrt(double);

void assert(int);

// end my defs
