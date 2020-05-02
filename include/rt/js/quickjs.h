/*
 * QuickJS Javascript Engine
 *
 * Copyright (c) 2017-2020 Fabrice Bellard
 * Copyright (c) 2017-2020 Charlie Gordon
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef QUICKJS_H
#define QUICKJS_H

#include <nautilus/libccompat.h>


//TODO: consider replacing references to these types instead
#define int8_t  sint8_t
#define int16_t sint16_t
#define int32_t sint32_t
#define int64_t sint64_t

// my defs:

typedef _Bool BOOL;

static inline uint32_t log2(const uint32_t x) {
  uint32_t y;
  asm ( "\tbsr %1, %0\n"
      : "=r"(y)
      : "r" (x)
  );
  return y;
}

#define DBL_EPSILON 2.2204460492503131e-16


#define ML_POSINF	(1.0 / 0.0)
#define ML_NEGINF	((-1.0) / 0.0)
#define ML_NAN		(0.0 / 0.0)

#define ML_ERR_return_NAN { ML_ERROR(ME_DOMAIN, ""); return ML_NAN; }

#define ME_NONE		0
/*	no error */
#define ME_DOMAIN	1
/*	argument out of domain */
#define ME_RANGE	2
/*	value out of range */
#define ME_NOCONV	4
/*	process did not converge */
#define ME_PRECISION	8
/*	does not have "full" precision */
#define ME_UNDERFLOW	16
/*	and underflow occured (important for IEEE)*/

//# define MATHLIB_WARNING(fmt,x)		warning(fmt,x)


#define ML_ERROR(x, s) { \
   if(x > ME_DOMAIN) { \
       char *msg = ""; \
       switch(x) { \
       case ME_DOMAIN: \
	   msg = _("argument out of domain in '%s'\n");	\
	   break; \
       case ME_RANGE: \
	   msg = _("value out of range in '%s'\n");	\
	   break; \
       case ME_NOCONV: \
	   msg = _("convergence failed in '%s'\n");	\
	   break; \
       case ME_PRECISION: \
	   msg = _("full precision may not have been achieved in '%s'\n"); \
	   break; \
       case ME_UNDERFLOW: \
	   msg = _("underflow occurred in '%s'\n");	\
	   break; \
       } \
       MATHLIB_WARNING(msg, s); \
   } \
}

#ifndef HAVE_LOG1P
double log1p(double x)
{
    /* series for log1p on the interval -.375 to .375
     *				     with weighted error   6.35e-32
     *				      log weighted error  31.20
     *			    significant figures required  30.93
     *				 decimal places required  32.01
     */
    const static double alnrcs[43] = {
	+.10378693562743769800686267719098e+1,
	-.13364301504908918098766041553133e+0,
	+.19408249135520563357926199374750e-1,
	-.30107551127535777690376537776592e-2,
	+.48694614797154850090456366509137e-3,
	-.81054881893175356066809943008622e-4,
	+.13778847799559524782938251496059e-4,
	-.23802210894358970251369992914935e-5,
	+.41640416213865183476391859901989e-6,
	-.73595828378075994984266837031998e-7,
	+.13117611876241674949152294345011e-7,
	-.23546709317742425136696092330175e-8,
	+.42522773276034997775638052962567e-9,
	-.77190894134840796826108107493300e-10,
	+.14075746481359069909215356472191e-10,
	-.25769072058024680627537078627584e-11,
	+.47342406666294421849154395005938e-12,
	-.87249012674742641745301263292675e-13,
	+.16124614902740551465739833119115e-13,
	-.29875652015665773006710792416815e-14,
	+.55480701209082887983041321697279e-15,
	-.10324619158271569595141333961932e-15,
	+.19250239203049851177878503244868e-16,
	-.35955073465265150011189707844266e-17,
	+.67264542537876857892194574226773e-18,
	-.12602624168735219252082425637546e-18,
	+.23644884408606210044916158955519e-19,
	-.44419377050807936898878389179733e-20,
	+.83546594464034259016241293994666e-21,
	-.15731559416479562574899253521066e-21,
	+.29653128740247422686154369706666e-22,
	-.55949583481815947292156013226666e-23,
	+.10566354268835681048187284138666e-23,
	-.19972483680670204548314999466666e-24,
	+.37782977818839361421049855999999e-25,
	-.71531586889081740345038165333333e-26,
	+.13552488463674213646502024533333e-26,
	-.25694673048487567430079829333333e-27,
	+.48747756066216949076459519999999e-28,
	-.92542112530849715321132373333333e-29,
	+.17578597841760239233269760000000e-29,
	-.33410026677731010351377066666666e-30,
	+.63533936180236187354180266666666e-31,
    };

#ifdef NOMORE_FOR_THREADS
    static int nlnrel = 0;
    static double xmin = 0.0;

    if (xmin == 0.0) xmin = -1 + sqrt(DBL_EPSILON);/*was sqrt(d1mach(4)); */
    if (nlnrel == 0) /* initialize chebychev coefficients */
	nlnrel = chebyshev_init(alnrcs, 43, DBL_EPSILON/20);/*was .1*d1mach(3)*/
#else
# define nlnrel 22
    const static double xmin = -0.999999985;
/* 22: for IEEE double precision where DBL_EPSILON =  2.22044604925031e-16 */
#endif

    if (x == 0.) return 0.;/* speed */
    if (x == -1) return(ML_NEGINF);
    if (x  < -1) ML_ERR_return_NAN;

    if (fabs(x) <= .375) {
        /* Improve on speed (only);
	   again give result accurate to IEEE double precision: */
	if(fabs(x) < .5 * DBL_EPSILON)
	    return x;

	if( (0 < x && x < 1e-8) || (-1e-9 < x && x < 0))
	    return x * (1 - .5 * x);
	/* else */
	return x * (1 - x * chebyshev_eval(x / .375, alnrcs, nlnrel));
    }
    /* else */
    if (x < xmin) {
	/* answer less than half precision because x too near -1 */
	ML_ERROR(ME_PRECISION, "log1p");
    }
    return log(1 + x);
}
#endif
    
    static const unsigned
     B1 = 715094163, /* B1 = (682-0.03306235651)*2**20 */
     B2 = 696219795; /* B2 = (664-0.03306235651)*2**20 */
 
 static const double
     C = 5.42857142857142815906e-01,  /* 19/35     = 0x3FE15F15, 0xF15F15F1 */
     D = -7.05306122448979611050e-01, /* -864/1225 = 0xBFE691DE, 0x2532C834 */
     E = 1.41428571428571436819e+00,  /* 99/70     = 0x3FF6A0EA, 0x0EA0EA0F */
     F = 1.60714285714285720630e+00,  /* 45/28     = 0x3FF9B6DB, 0x6DB6DB6E */
     G = 3.57142857142857150787e-01;  /* 5/14      = 0x3FD6DB6D, 0xB6DB6DB7 */
 
 /**
  * @brief Cube root function
  */
 double cbrt(double x)
 {
     int32_t hx, lx, ht;
     double r, s, t = 0.0, w;
     uint32_t sign;
 
     GET_HIGH_WORD(hx, x);
 
     // sign = sign(x)
     sign = hx & 0x80000000;
     hx ^= sign;
 
     // cbrt(NaN,INF) is itself
     if (hx >= 0x7FF00000)
     {
         return x + x;
     }
 
     GET_LOW_WORD(lx, x);
 
     // cbrt(0) is itself
     if ((hx | lx) == 0)
     {
         return x;
     }
 
     // x <- |x|
     SET_HIGH_WORD(x, hx);
 
     // rough cbrt to 5 bits
     if (hx < 0x00100000) // subnormal number
     {
         SET_HIGH_WORD(t, 0x43500000); // set t= 2**54
         t *= x;
         GET_HIGH_WORD(ht, t);
         SET_HIGH_WORD(t, ht / 3 + B2);
     }
     else
     {
         SET_HIGH_WORD(t, hx / 3 + B1);
     }
 
     // new cbrt to 23 bits, may be implemented in single precision
     r = t * t / x;
     s = C + r * t;
     t *= G + F / (s + E + D / s);
 
     // chopped to 20 bits and make it larger than cbrt(x)
     SET_LOW_WORD(t, 0);
     GET_HIGH_WORD(ht, t);
     SET_HIGH_WORD(t, ht + 0x00000001);
 
     // one step newton iteration to 53 bits with error less than 0.667 ulps
     s = t * t; // t*t is exact
     r = x / s;
     w = t + t;
     r = (r - t) / (w + r); // r-s is exact
     t = t + t * r;
 
     // retore the sign bit
     GET_HIGH_WORD(ht, t);
     SET_HIGH_WORD(t, ht | sign);
 
     return (t);
 }


//end my defs





#ifdef __cplusplus
extern "C" {
#endif

#if defined(__GNUC__) || defined(__clang__)
#define js_likely(x)          __builtin_expect(!!(x), 1)
#define js_unlikely(x)        __builtin_expect(!!(x), 0)
#define js_force_inline       inline __attribute__((always_inline))
#define __js_printf_like(f, a)   __attribute__((format(printf, f, a)))
#else
#define js_likely(x)     (x)
#define js_unlikely(x)   (x)
#define js_force_inline  inline
#define __js_printf_like(a, b)
#endif

#define JS_BOOL int

typedef struct JSRuntime JSRuntime;
typedef struct JSContext JSContext;
typedef struct JSObject JSObject;
typedef struct JSClass JSClass;
typedef uint32_t JSClassID;
typedef uint32_t JSAtom;

#if INTPTR_MAX >= INT64_MAX
#define JS_PTR64
#define JS_PTR64_DEF(a) a
#else
#define JS_PTR64_DEF(a)
#endif

#ifndef JS_PTR64
#define JS_NAN_BOXING
#endif

enum {
    /* all tags with a reference count are negative */
    JS_TAG_FIRST       = -11, /* first negative tag */
    JS_TAG_BIG_DECIMAL = -11,
    JS_TAG_BIG_INT     = -10,
    JS_TAG_BIG_FLOAT   = -9,
    JS_TAG_SYMBOL      = -8,
    JS_TAG_STRING      = -7,
    JS_TAG_MODULE      = -3, /* used internally */
    JS_TAG_FUNCTION_BYTECODE = -2, /* used internally */
    JS_TAG_OBJECT      = -1,

    JS_TAG_INT         = 0,
    JS_TAG_BOOL        = 1,
    JS_TAG_NULL        = 2,
    JS_TAG_UNDEFINED   = 3,
    JS_TAG_UNINITIALIZED = 4,
    JS_TAG_CATCH_OFFSET = 5,
    JS_TAG_EXCEPTION   = 6,
    JS_TAG_FLOAT64     = 7,
    /* any larger tag is FLOAT64 if JS_NAN_BOXING */
};

typedef struct JSRefCountHeader {
    int ref_count;
} JSRefCountHeader;

#define JS_FLOAT64_NAN NAN

#ifdef CONFIG_CHECK_JSVALUE
/* JSValue consistency : it is not possible to run the code in this
   mode, but it is useful to detect simple reference counting
   errors. It would be interesting to modify a static C analyzer to
   handle specific annotations (clang has such annotations but only
   for objective C) */
typedef struct __JSValue *JSValue;
typedef const struct __JSValue *JSValueConst;

#define JS_VALUE_GET_TAG(v) (int)((uintptr_t)(v) & 0xf)
/* same as JS_VALUE_GET_TAG, but return JS_TAG_FLOAT64 with NaN boxing */
#define JS_VALUE_GET_NORM_TAG(v) JS_VALUE_GET_TAG(v)
#define JS_VALUE_GET_INT(v) (int)((intptr_t)(v) >> 4)
#define JS_VALUE_GET_BOOL(v) JS_VALUE_GET_INT(v)
#define JS_VALUE_GET_FLOAT64(v) (double)JS_VALUE_GET_INT(v)
#define JS_VALUE_GET_PTR(v) (void *)((intptr_t)(v) & ~0xf)

#define JS_MKVAL(tag, val) (JSValue)(intptr_t)(((val) << 4) | (tag))
#define JS_MKPTR(tag, p) (JSValue)((intptr_t)(p) | (tag))

#define JS_TAG_IS_FLOAT64(tag) ((unsigned)(tag) == JS_TAG_FLOAT64)

#define JS_NAN JS_MKVAL(JS_TAG_FLOAT64, 1)

static inline JSValue __JS_NewFloat64(JSContext *ctx, double d)
{
    return JS_MKVAL(JS_TAG_FLOAT64, (int)d);
}

static inline JS_BOOL JS_VALUE_IS_NAN(JSValue v)
{
    return 0;
}
    
#elif defined(JS_NAN_BOXING)

typedef uint64_t JSValue;

#define JSValueConst JSValue

#define JS_VALUE_GET_TAG(v) (int)((v) >> 32)
#define JS_VALUE_GET_INT(v) (int)(v)
#define JS_VALUE_GET_BOOL(v) (int)(v)
#define JS_VALUE_GET_PTR(v) (void *)(intptr_t)(v)

#define JS_MKVAL(tag, val) (((uint64_t)(tag) << 32) | (uint32_t)(val))
#define JS_MKPTR(tag, ptr) (((uint64_t)(tag) << 32) | (uintptr_t)(ptr))

#define JS_FLOAT64_TAG_ADDEND (0x7ff80000 - JS_TAG_FIRST + 1) /* quiet NaN encoding */

static inline double JS_VALUE_GET_FLOAT64(JSValue v)
{
    union {
        JSValue v;
        double d;
    } u;
    u.v = v;
    u.v += (uint64_t)JS_FLOAT64_TAG_ADDEND << 32;
    return u.d;
}

#define JS_NAN (0x7ff8000000000000 - ((uint64_t)JS_FLOAT64_TAG_ADDEND << 32))

static inline JSValue __JS_NewFloat64(JSContext *ctx, double d)
{
    union {
        double d;
        uint64_t u64;
    } u;
    JSValue v;
    u.d = d;
    /* normalize NaN */
    if (js_unlikely((u.u64 & 0x7fffffffffffffff) > 0x7ff0000000000000))
        v = JS_NAN;
    else
        v = u.u64 - ((uint64_t)JS_FLOAT64_TAG_ADDEND << 32);
    return v;
}

#define JS_TAG_IS_FLOAT64(tag) ((unsigned)((tag) - JS_TAG_FIRST) >= (JS_TAG_FLOAT64 - JS_TAG_FIRST))

/* same as JS_VALUE_GET_TAG, but return JS_TAG_FLOAT64 with NaN boxing */
static inline int JS_VALUE_GET_NORM_TAG(JSValue v)
{
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(v);
    if (JS_TAG_IS_FLOAT64(tag))
        return JS_TAG_FLOAT64;
    else
        return tag;
}

static inline JS_BOOL JS_VALUE_IS_NAN(JSValue v)
{
    uint32_t tag;
    tag = JS_VALUE_GET_TAG(v);
    return tag == (JS_NAN >> 32);
}
    
#else /* !JS_NAN_BOXING */

typedef union JSValueUnion {
    int32_t int32;
    double float64;
    void *ptr;
} JSValueUnion;

typedef struct JSValue {
    JSValueUnion u;
    int64_t tag;
} JSValue;

#define JSValueConst JSValue

#define JS_VALUE_GET_TAG(v) ((int32_t)(v).tag)
/* same as JS_VALUE_GET_TAG, but return JS_TAG_FLOAT64 with NaN boxing */
#define JS_VALUE_GET_NORM_TAG(v) JS_VALUE_GET_TAG(v)
#define JS_VALUE_GET_INT(v) ((v).u.int32)
#define JS_VALUE_GET_BOOL(v) ((v).u.int32)
#define JS_VALUE_GET_FLOAT64(v) ((v).u.float64)
#define JS_VALUE_GET_PTR(v) ((v).u.ptr)

#define JS_MKVAL(tag, val) (JSValue){ (JSValueUnion){ .int32 = val }, tag }
#define JS_MKPTR(tag, p) (JSValue){ (JSValueUnion){ .ptr = p }, tag }

#define JS_TAG_IS_FLOAT64(tag) ((unsigned)(tag) == JS_TAG_FLOAT64)

#define JS_NAN (JSValue){ .u.float64 = JS_FLOAT64_NAN, JS_TAG_FLOAT64 }

static inline JSValue __JS_NewFloat64(JSContext *ctx, double d)
{
    JSValue v;
    v.tag = JS_TAG_FLOAT64;
    v.u.float64 = d;
    return v;
}

static inline JS_BOOL JS_VALUE_IS_NAN(JSValue v)
{
    union {
        double d;
        uint64_t u64;
    } u;
    if (v.tag != JS_TAG_FLOAT64)
        return 0;
    u.d = v.u.float64;
    return (u.u64 & 0x7fffffffffffffff) > 0x7ff0000000000000;
}

#endif /* !JS_NAN_BOXING */

#define JS_VALUE_IS_BOTH_INT(v1, v2) ((JS_VALUE_GET_TAG(v1) | JS_VALUE_GET_TAG(v2)) == 0)
#define JS_VALUE_IS_BOTH_FLOAT(v1, v2) (JS_TAG_IS_FLOAT64(JS_VALUE_GET_TAG(v1)) && JS_TAG_IS_FLOAT64(JS_VALUE_GET_TAG(v2)))

#define JS_VALUE_GET_OBJ(v) ((JSObject *)JS_VALUE_GET_PTR(v))
#define JS_VALUE_GET_STRING(v) ((JSString *)JS_VALUE_GET_PTR(v))
#define JS_VALUE_HAS_REF_COUNT(v) ((unsigned)JS_VALUE_GET_TAG(v) >= (unsigned)JS_TAG_FIRST)

/* special values */
#define JS_NULL      JS_MKVAL(JS_TAG_NULL, 0)
#define JS_UNDEFINED JS_MKVAL(JS_TAG_UNDEFINED, 0)
#define JS_FALSE     JS_MKVAL(JS_TAG_BOOL, 0)
#define JS_TRUE      JS_MKVAL(JS_TAG_BOOL, 1)
#define JS_EXCEPTION JS_MKVAL(JS_TAG_EXCEPTION, 0)
#define JS_UNINITIALIZED JS_MKVAL(JS_TAG_UNINITIALIZED, 0)

/* flags for object properties */
#define JS_PROP_CONFIGURABLE  (1 << 0)
#define JS_PROP_WRITABLE      (1 << 1)
#define JS_PROP_ENUMERABLE    (1 << 2)
#define JS_PROP_C_W_E         (JS_PROP_CONFIGURABLE | JS_PROP_WRITABLE | JS_PROP_ENUMERABLE)
#define JS_PROP_LENGTH        (1 << 3) /* used internally in Arrays */
#define JS_PROP_TMASK         (3 << 4) /* mask for NORMAL, GETSET, VARREF, AUTOINIT */
#define JS_PROP_NORMAL         (0 << 4)
#define JS_PROP_GETSET         (1 << 4)
#define JS_PROP_VARREF         (2 << 4) /* used internally */
#define JS_PROP_AUTOINIT       (3 << 4) /* used internally */

/* flags for JS_DefineProperty */
#define JS_PROP_HAS_SHIFT        8
#define JS_PROP_HAS_CONFIGURABLE (1 << 8)
#define JS_PROP_HAS_WRITABLE     (1 << 9)
#define JS_PROP_HAS_ENUMERABLE   (1 << 10)
#define JS_PROP_HAS_GET          (1 << 11)
#define JS_PROP_HAS_SET          (1 << 12)
#define JS_PROP_HAS_VALUE        (1 << 13)

/* throw an exception if false would be returned
   (JS_DefineProperty/JS_SetProperty) */
#define JS_PROP_THROW            (1 << 14)
/* throw an exception if false would be returned in strict mode
   (JS_SetProperty) */
#define JS_PROP_THROW_STRICT     (1 << 15)

#define JS_PROP_NO_ADD           (1 << 16) /* internal use */
#define JS_PROP_NO_EXOTIC        (1 << 17) /* internal use */

#define JS_DEFAULT_STACK_SIZE (256 * 1024)

/* JS_Eval() flags */
#define JS_EVAL_TYPE_GLOBAL   (0 << 0) /* global code (default) */
#define JS_EVAL_TYPE_MODULE   (1 << 0) /* module code */
#define JS_EVAL_TYPE_DIRECT   (2 << 0) /* direct call (internal use) */
#define JS_EVAL_TYPE_INDIRECT (3 << 0) /* indirect call (internal use) */
#define JS_EVAL_TYPE_MASK     (3 << 0)

#define JS_EVAL_FLAG_STRICT   (1 << 3) /* force 'strict' mode */
#define JS_EVAL_FLAG_STRIP    (1 << 4) /* force 'strip' mode */
/* compile but do not run. The result is an object with a
   JS_TAG_FUNCTION_BYTECODE or JS_TAG_MODULE tag. It can be executed
   with JS_EvalFunction(). */
#define JS_EVAL_FLAG_COMPILE_ONLY (1 << 5)
/* don't include the stack frames before this eval in the Error() backtraces */
#define JS_EVAL_FLAG_BACKTRACE_BARRIER (1 << 6)

typedef JSValue JSCFunction(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv);
typedef JSValue JSCFunctionMagic(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic);
typedef JSValue JSCFunctionData(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic, JSValue *func_data);

typedef struct JSMallocState {
    size_t malloc_count;
    size_t malloc_size;
    size_t malloc_limit;
    void *opaque; /* user opaque */
} JSMallocState;

typedef struct JSMallocFunctions {
    void *(*js_malloc)(JSMallocState *s, size_t size);
    void (*js_free)(JSMallocState *s, void *ptr);
    void *(*js_realloc)(JSMallocState *s, void *ptr, size_t size);
    size_t (*js_malloc_usable_size)(const void *ptr);
} JSMallocFunctions;

typedef struct JSGCObjectHeader JSGCObjectHeader;

JSRuntime *JS_NewRuntime(void);
/* info lifetime must exceed that of rt */
void JS_SetRuntimeInfo(JSRuntime *rt, const char *info);
void JS_SetMemoryLimit(JSRuntime *rt, size_t limit);
void JS_SetGCThreshold(JSRuntime *rt, size_t gc_threshold);
void JS_SetMaxStackSize(JSRuntime *rt, size_t stack_size);
JSRuntime *JS_NewRuntime2(const JSMallocFunctions *mf, void *opaque);
void JS_FreeRuntime(JSRuntime *rt);
void *JS_GetRuntimeOpaque(JSRuntime *rt);
void JS_SetRuntimeOpaque(JSRuntime *rt, void *opaque);
typedef void JS_MarkFunc(JSRuntime *rt, JSGCObjectHeader *gp);
void JS_MarkValue(JSRuntime *rt, JSValueConst val, JS_MarkFunc *mark_func);
void JS_RunGC(JSRuntime *rt);
JS_BOOL JS_IsLiveObject(JSRuntime *rt, JSValueConst obj);

JSContext *JS_NewContext(JSRuntime *rt);
void JS_FreeContext(JSContext *s);
JSContext *JS_DupContext(JSContext *ctx);
void *JS_GetContextOpaque(JSContext *ctx);
void JS_SetContextOpaque(JSContext *ctx, void *opaque);
JSRuntime *JS_GetRuntime(JSContext *ctx);
void JS_SetClassProto(JSContext *ctx, JSClassID class_id, JSValue obj);
JSValue JS_GetClassProto(JSContext *ctx, JSClassID class_id);

/* the following functions are used to select the intrinsic object to
   save memory */
JSContext *JS_NewContextRaw(JSRuntime *rt);
void JS_AddIntrinsicBaseObjects(JSContext *ctx);
void JS_AddIntrinsicDate(JSContext *ctx);
void JS_AddIntrinsicEval(JSContext *ctx);
void JS_AddIntrinsicStringNormalize(JSContext *ctx);
void JS_AddIntrinsicRegExpCompiler(JSContext *ctx);
void JS_AddIntrinsicRegExp(JSContext *ctx);
void JS_AddIntrinsicJSON(JSContext *ctx);
void JS_AddIntrinsicProxy(JSContext *ctx);
void JS_AddIntrinsicMapSet(JSContext *ctx);
void JS_AddIntrinsicTypedArrays(JSContext *ctx);
void JS_AddIntrinsicPromise(JSContext *ctx);
void JS_AddIntrinsicBigInt(JSContext *ctx);
void JS_AddIntrinsicBigFloat(JSContext *ctx);
void JS_AddIntrinsicBigDecimal(JSContext *ctx);
/* enable operator overloading */
void JS_AddIntrinsicOperators(JSContext *ctx);
/* enable "use math" */
void JS_EnableBignumExt(JSContext *ctx, JS_BOOL enable);

JSValue js_string_codePointRange(JSContext *ctx, JSValueConst this_val,
                                 int argc, JSValueConst *argv);

void *js_malloc_rt(JSRuntime *rt, size_t size);
void js_free_rt(JSRuntime *rt, void *ptr);
void *js_realloc_rt(JSRuntime *rt, void *ptr, size_t size);
size_t js_malloc_usable_size_rt(JSRuntime *rt, const void *ptr);
void *js_mallocz_rt(JSRuntime *rt, size_t size);

void *js_malloc(JSContext *ctx, size_t size);
void js_free(JSContext *ctx, void *ptr);
void *js_realloc(JSContext *ctx, void *ptr, size_t size);
size_t js_malloc_usable_size(JSContext *ctx, const void *ptr);
void *js_realloc2(JSContext *ctx, void *ptr, size_t size, size_t *pslack);
void *js_mallocz(JSContext *ctx, size_t size);
char *js_strdup(JSContext *ctx, const char *str);
char *js_strndup(JSContext *ctx, const char *s, size_t n);

typedef struct JSMemoryUsage {
    int64_t malloc_size, malloc_limit, memory_used_size;
    int64_t malloc_count;
    int64_t memory_used_count;
    int64_t atom_count, atom_size;
    int64_t str_count, str_size;
    int64_t obj_count, obj_size;
    int64_t prop_count, prop_size;
    int64_t shape_count, shape_size;
    int64_t js_func_count, js_func_size, js_func_code_size;
    int64_t js_func_pc2line_count, js_func_pc2line_size;
    int64_t c_func_count, array_count;
    int64_t fast_array_count, fast_array_elements;
    int64_t binary_object_count, binary_object_size;
} JSMemoryUsage;

void JS_ComputeMemoryUsage(JSRuntime *rt, JSMemoryUsage *s);
void JS_DumpMemoryUsage(FILE *fp, const JSMemoryUsage *s, JSRuntime *rt);

/* atom support */
JSAtom JS_NewAtomLen(JSContext *ctx, const char *str, size_t len);
JSAtom JS_NewAtom(JSContext *ctx, const char *str);
JSAtom JS_NewAtomUInt32(JSContext *ctx, uint32_t n);
JSAtom JS_DupAtom(JSContext *ctx, JSAtom v);
void JS_FreeAtom(JSContext *ctx, JSAtom v);
void JS_FreeAtomRT(JSRuntime *rt, JSAtom v);
JSValue JS_AtomToValue(JSContext *ctx, JSAtom atom);
JSValue JS_AtomToString(JSContext *ctx, JSAtom atom);
const char *JS_AtomToCString(JSContext *ctx, JSAtom atom);
JSAtom JS_ValueToAtom(JSContext *ctx, JSValueConst val);

/* object class support */

typedef struct JSPropertyEnum {
    JS_BOOL is_enumerable;
    JSAtom atom;
} JSPropertyEnum;

typedef struct JSPropertyDescriptor {
    int flags;
    JSValue value;
    JSValue getter;
    JSValue setter;
} JSPropertyDescriptor;

typedef struct JSClassExoticMethods {
    /* Return -1 if exception (can only happen in case of Proxy object),
       FALSE if the property does not exists, TRUE if it exists. If 1 is
       returned, the property descriptor 'desc' is filled if != NULL. */
    int (*get_own_property)(JSContext *ctx, JSPropertyDescriptor *desc,
                             JSValueConst obj, JSAtom prop);
    /* '*ptab' should hold the '*plen' property keys. Return 0 if OK,
       -1 if exception. The 'is_enumerable' field is ignored.
    */
    int (*get_own_property_names)(JSContext *ctx, JSPropertyEnum **ptab,
                                  uint32_t *plen,
                                  JSValueConst obj);
    /* return < 0 if exception, or TRUE/FALSE */
    int (*delete_property)(JSContext *ctx, JSValueConst obj, JSAtom prop);
    /* return < 0 if exception or TRUE/FALSE */
    int (*define_own_property)(JSContext *ctx, JSValueConst this_obj,
                               JSAtom prop, JSValueConst val,
                               JSValueConst getter, JSValueConst setter,
                               int flags);
    /* The following methods can be emulated with the previous ones,
       so they are usually not needed */
    /* return < 0 if exception or TRUE/FALSE */
    int (*has_property)(JSContext *ctx, JSValueConst obj, JSAtom atom);
    JSValue (*get_property)(JSContext *ctx, JSValueConst obj, JSAtom atom,
                            JSValueConst receiver);
    /* return < 0 if exception or TRUE/FALSE */
    int (*set_property)(JSContext *ctx, JSValueConst obj, JSAtom atom,
                        JSValueConst value, JSValueConst receiver, int flags);
} JSClassExoticMethods;

typedef void JSClassFinalizer(JSRuntime *rt, JSValue val);
typedef void JSClassGCMark(JSRuntime *rt, JSValueConst val,
                           JS_MarkFunc *mark_func);
#define JS_CALL_FLAG_CONSTRUCTOR (1 << 0)
typedef JSValue JSClassCall(JSContext *ctx, JSValueConst func_obj,
                            JSValueConst this_val, int argc, JSValueConst *argv,
                            int flags);

typedef struct JSClassDef {
    const char *class_name;
    JSClassFinalizer *finalizer;
    JSClassGCMark *gc_mark;
    /* if call != NULL, the object is a function. If (flags &
       JS_CALL_FLAG_CONSTRUCTOR) != 0, the function is called as a
       constructor. In this case, 'this_val' is new.target. A
       constructor call only happens if the object constructor bit is
       set (see JS_SetConstructorBit()). */
    JSClassCall *call;
    /* XXX: suppress this indirection ? It is here only to save memory
       because only a few classes need these methods */
    JSClassExoticMethods *exotic;
} JSClassDef;

JSClassID JS_NewClassID(JSClassID *pclass_id);
int JS_NewClass(JSRuntime *rt, JSClassID class_id, const JSClassDef *class_def);
int JS_IsRegisteredClass(JSRuntime *rt, JSClassID class_id);

/* value handling */

static js_force_inline JSValue JS_NewBool(JSContext *ctx, JS_BOOL val)
{
    return JS_MKVAL(JS_TAG_BOOL, val);
}

static js_force_inline JSValue JS_NewInt32(JSContext *ctx, int32_t val)
{
    return JS_MKVAL(JS_TAG_INT, val);
}

static js_force_inline JSValue JS_NewCatchOffset(JSContext *ctx, int32_t val)
{
    return JS_MKVAL(JS_TAG_CATCH_OFFSET, val);
}

static js_force_inline JSValue JS_NewInt64(JSContext *ctx, int64_t val)
{
    JSValue v;
    if (val == (int32_t)val) {
        v = JS_NewInt32(ctx, val);
    } else {
        v = __JS_NewFloat64(ctx, val);
    }
    return v;
}

static js_force_inline JSValue JS_NewUint32(JSContext *ctx, uint32_t val)
{
    JSValue v;
    if (val <= 0x7fffffff) {
        v = JS_NewInt32(ctx, val);
    } else {
        v = __JS_NewFloat64(ctx, val);
    }
    return v;
}

JSValue JS_NewBigInt64(JSContext *ctx, int64_t v);
JSValue JS_NewBigUint64(JSContext *ctx, uint64_t v);

static js_force_inline JSValue JS_NewFloat64(JSContext *ctx, double d)
{
    JSValue v;
    int32_t val;
    union {
        double d;
        uint64_t u;
    } u, t;
    u.d = d;
    val = (int32_t)d;
    t.d = val;
    /* -0 cannot be represented as integer, so we compare the bit
        representation */
    if (u.u == t.u) {
        v = JS_MKVAL(JS_TAG_INT, val);
    } else {
        v = __JS_NewFloat64(ctx, d);
    }
    return v;
}

static inline JS_BOOL JS_IsNumber(JSValueConst v)
{
    int tag = JS_VALUE_GET_TAG(v);
    return tag == JS_TAG_INT || JS_TAG_IS_FLOAT64(tag);
}

static inline JS_BOOL JS_IsBigInt(JSContext *ctx, JSValueConst v)
{
    int tag = JS_VALUE_GET_TAG(v);
    return tag == JS_TAG_BIG_INT;
}

static inline JS_BOOL JS_IsBigFloat(JSValueConst v)
{
    int tag = JS_VALUE_GET_TAG(v);
    return tag == JS_TAG_BIG_FLOAT;
}

static inline JS_BOOL JS_IsBigDecimal(JSValueConst v)
{
    int tag = JS_VALUE_GET_TAG(v);
    return tag == JS_TAG_BIG_DECIMAL;
}

static inline JS_BOOL JS_IsBool(JSValueConst v)
{
    return JS_VALUE_GET_TAG(v) == JS_TAG_BOOL;
}

static inline JS_BOOL JS_IsNull(JSValueConst v)
{
    return JS_VALUE_GET_TAG(v) == JS_TAG_NULL;
}

static inline JS_BOOL JS_IsUndefined(JSValueConst v)
{
    return JS_VALUE_GET_TAG(v) == JS_TAG_UNDEFINED;
}

static inline JS_BOOL JS_IsException(JSValueConst v)
{
    return js_unlikely(JS_VALUE_GET_TAG(v) == JS_TAG_EXCEPTION);
}

static inline JS_BOOL JS_IsUninitialized(JSValueConst v)
{
    return js_unlikely(JS_VALUE_GET_TAG(v) == JS_TAG_UNINITIALIZED);
}

static inline JS_BOOL JS_IsString(JSValueConst v)
{
    return JS_VALUE_GET_TAG(v) == JS_TAG_STRING;
}

static inline JS_BOOL JS_IsSymbol(JSValueConst v)
{
    return JS_VALUE_GET_TAG(v) == JS_TAG_SYMBOL;
}

static inline JS_BOOL JS_IsObject(JSValueConst v)
{
    return JS_VALUE_GET_TAG(v) == JS_TAG_OBJECT;
}

JSValue JS_Throw(JSContext *ctx, JSValue obj);
JSValue JS_GetException(JSContext *ctx);
JS_BOOL JS_IsError(JSContext *ctx, JSValueConst val);
void JS_ResetUncatchableError(JSContext *ctx);
JSValue JS_NewError(JSContext *ctx);
JSValue __js_printf_like(2, 3) JS_ThrowSyntaxError(JSContext *ctx, const char *fmt, ...);
JSValue __js_printf_like(2, 3) JS_ThrowTypeError(JSContext *ctx, const char *fmt, ...);
JSValue __js_printf_like(2, 3) JS_ThrowReferenceError(JSContext *ctx, const char *fmt, ...);
JSValue __js_printf_like(2, 3) JS_ThrowRangeError(JSContext *ctx, const char *fmt, ...);
JSValue __js_printf_like(2, 3) JS_ThrowInternalError(JSContext *ctx, const char *fmt, ...);
JSValue JS_ThrowOutOfMemory(JSContext *ctx);

void __JS_FreeValue(JSContext *ctx, JSValue v);
static inline void JS_FreeValue(JSContext *ctx, JSValue v)
{
    if (JS_VALUE_HAS_REF_COUNT(v)) {
        JSRefCountHeader *p = (JSRefCountHeader *)JS_VALUE_GET_PTR(v);
        if (--p->ref_count <= 0) {
            __JS_FreeValue(ctx, v);
        }
    }
}
void __JS_FreeValueRT(JSRuntime *rt, JSValue v);
static inline void JS_FreeValueRT(JSRuntime *rt, JSValue v)
{
    if (JS_VALUE_HAS_REF_COUNT(v)) {
        JSRefCountHeader *p = (JSRefCountHeader *)JS_VALUE_GET_PTR(v);
        if (--p->ref_count <= 0) {
            __JS_FreeValueRT(rt, v);
        }
    }
}

static inline JSValue JS_DupValue(JSContext *ctx, JSValueConst v)
{
    if (JS_VALUE_HAS_REF_COUNT(v)) {
        JSRefCountHeader *p = (JSRefCountHeader *)JS_VALUE_GET_PTR(v);
        p->ref_count++;
    }
    return (JSValue)v;
}

static inline JSValue JS_DupValueRT(JSRuntime *rt, JSValueConst v)
{
    if (JS_VALUE_HAS_REF_COUNT(v)) {
        JSRefCountHeader *p = (JSRefCountHeader *)JS_VALUE_GET_PTR(v);
        p->ref_count++;
    }
    return (JSValue)v;
}

int JS_ToBool(JSContext *ctx, JSValueConst val); /* return -1 for JS_EXCEPTION */
int JS_ToInt32(JSContext *ctx, int32_t *pres, JSValueConst val);
static inline int JS_ToUint32(JSContext *ctx, uint32_t *pres, JSValueConst val)
{
    return JS_ToInt32(ctx, (int32_t*)pres, val);
}
int JS_ToInt64(JSContext *ctx, int64_t *pres, JSValueConst val);
int JS_ToIndex(JSContext *ctx, uint64_t *plen, JSValueConst val);
int JS_ToFloat64(JSContext *ctx, double *pres, JSValueConst val);
/* return an exception if 'val' is a Number */
int JS_ToBigInt64(JSContext *ctx, int64_t *pres, JSValueConst val);
/* same as JS_ToInt64() but allow BigInt */
int JS_ToInt64Ext(JSContext *ctx, int64_t *pres, JSValueConst val);

JSValue JS_NewStringLen(JSContext *ctx, const char *str1, size_t len1);
JSValue JS_NewString(JSContext *ctx, const char *str);
JSValue JS_NewAtomString(JSContext *ctx, const char *str);
JSValue JS_ToString(JSContext *ctx, JSValueConst val);
JSValue JS_ToPropertyKey(JSContext *ctx, JSValueConst val);
const char *JS_ToCStringLen2(JSContext *ctx, size_t *plen, JSValueConst val1, JS_BOOL cesu8);
static inline const char *JS_ToCStringLen(JSContext *ctx, size_t *plen, JSValueConst val1)
{
    return JS_ToCStringLen2(ctx, plen, val1, 0);
}
static inline const char *JS_ToCString(JSContext *ctx, JSValueConst val1)
{
    return JS_ToCStringLen2(ctx, NULL, val1, 0);
}
void JS_FreeCString(JSContext *ctx, const char *ptr);

JSValue JS_NewObjectProtoClass(JSContext *ctx, JSValueConst proto, JSClassID class_id);
JSValue JS_NewObjectClass(JSContext *ctx, int class_id);
JSValue JS_NewObjectProto(JSContext *ctx, JSValueConst proto);
JSValue JS_NewObject(JSContext *ctx);

JS_BOOL JS_IsFunction(JSContext* ctx, JSValueConst val);
JS_BOOL JS_IsConstructor(JSContext* ctx, JSValueConst val);
JS_BOOL JS_SetConstructorBit(JSContext *ctx, JSValueConst func_obj, JS_BOOL val);

JSValue JS_NewArray(JSContext *ctx);
int JS_IsArray(JSContext *ctx, JSValueConst val);

JSValue JS_GetPropertyInternal(JSContext *ctx, JSValueConst obj,
                               JSAtom prop, JSValueConst receiver,
                               JS_BOOL throw_ref_error);
static js_force_inline JSValue JS_GetProperty(JSContext *ctx, JSValueConst this_obj,
                                              JSAtom prop)
{
    return JS_GetPropertyInternal(ctx, this_obj, prop, this_obj, 0);
}
JSValue JS_GetPropertyStr(JSContext *ctx, JSValueConst this_obj,
                          const char *prop);
JSValue JS_GetPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                             uint32_t idx);

int JS_SetPropertyInternal(JSContext *ctx, JSValueConst this_obj,
                           JSAtom prop, JSValue val,
                           int flags);
static inline int JS_SetProperty(JSContext *ctx, JSValueConst this_obj,
                                 JSAtom prop, JSValue val)
{
    return JS_SetPropertyInternal(ctx, this_obj, prop, val, JS_PROP_THROW);
}
int JS_SetPropertyUint32(JSContext *ctx, JSValueConst this_obj,
                         uint32_t idx, JSValue val);
int JS_SetPropertyInt64(JSContext *ctx, JSValueConst this_obj,
                        int64_t idx, JSValue val);
int JS_SetPropertyStr(JSContext *ctx, JSValueConst this_obj,
                      const char *prop, JSValue val);
int JS_HasProperty(JSContext *ctx, JSValueConst this_obj, JSAtom prop);
int JS_IsExtensible(JSContext *ctx, JSValueConst obj);
int JS_PreventExtensions(JSContext *ctx, JSValueConst obj);
int JS_DeleteProperty(JSContext *ctx, JSValueConst obj, JSAtom prop, int flags);
int JS_SetPrototype(JSContext *ctx, JSValueConst obj, JSValueConst proto_val);
JSValueConst JS_GetPrototype(JSContext *ctx, JSValueConst val);

#define JS_GPN_STRING_MASK  (1 << 0)
#define JS_GPN_SYMBOL_MASK  (1 << 1)
#define JS_GPN_PRIVATE_MASK (1 << 2)
/* only include the enumerable properties */
#define JS_GPN_ENUM_ONLY    (1 << 4)
/* set theJSPropertyEnum.is_enumerable field */
#define JS_GPN_SET_ENUM     (1 << 5)

int JS_GetOwnPropertyNames(JSContext *ctx, JSPropertyEnum **ptab,
                           uint32_t *plen, JSValueConst obj, int flags);
int JS_GetOwnProperty(JSContext *ctx, JSPropertyDescriptor *desc,
                      JSValueConst obj, JSAtom prop);

JSValue JS_Call(JSContext *ctx, JSValueConst func_obj, JSValueConst this_obj,
                int argc, JSValueConst *argv);
JSValue JS_Invoke(JSContext *ctx, JSValueConst this_val, JSAtom atom,
                  int argc, JSValueConst *argv);
JSValue JS_CallConstructor(JSContext *ctx, JSValueConst func_obj,
                           int argc, JSValueConst *argv);
JSValue JS_CallConstructor2(JSContext *ctx, JSValueConst func_obj,
                            JSValueConst new_target,
                            int argc, JSValueConst *argv);
JS_BOOL JS_DetectModule(const char *input, size_t input_len);
/* 'input' must be zero terminated i.e. input[input_len] = '\0'. */
JSValue JS_Eval(JSContext *ctx, const char *input, size_t input_len,
                const char *filename, int eval_flags);
JSValue JS_EvalFunction(JSContext *ctx, JSValue fun_obj);
JSValue JS_GetGlobalObject(JSContext *ctx);
int JS_IsInstanceOf(JSContext *ctx, JSValueConst val, JSValueConst obj);
int JS_DefineProperty(JSContext *ctx, JSValueConst this_obj,
                      JSAtom prop, JSValueConst val,
                      JSValueConst getter, JSValueConst setter, int flags);
int JS_DefinePropertyValue(JSContext *ctx, JSValueConst this_obj,
                           JSAtom prop, JSValue val, int flags);
int JS_DefinePropertyValueUint32(JSContext *ctx, JSValueConst this_obj,
                                 uint32_t idx, JSValue val, int flags);
int JS_DefinePropertyValueStr(JSContext *ctx, JSValueConst this_obj,
                              const char *prop, JSValue val, int flags);
int JS_DefinePropertyGetSet(JSContext *ctx, JSValueConst this_obj,
                            JSAtom prop, JSValue getter, JSValue setter,
                            int flags);
void JS_SetOpaque(JSValue obj, void *opaque);
void *JS_GetOpaque(JSValueConst obj, JSClassID class_id);
void *JS_GetOpaque2(JSContext *ctx, JSValueConst obj, JSClassID class_id);

/* 'buf' must be zero terminated i.e. buf[buf_len] = '\0'. */
JSValue JS_ParseJSON(JSContext *ctx, const char *buf, size_t buf_len,
                     const char *filename);
JSValue JS_JSONStringify(JSContext *ctx, JSValueConst obj,
                         JSValueConst replacer, JSValueConst space0);

typedef void JSFreeArrayBufferDataFunc(JSRuntime *rt, void *opaque, void *ptr);
JSValue JS_NewArrayBuffer(JSContext *ctx, uint8_t *buf, size_t len,
                          JSFreeArrayBufferDataFunc *free_func, void *opaque,
                          JS_BOOL is_shared);
JSValue JS_NewArrayBufferCopy(JSContext *ctx, const uint8_t *buf, size_t len);
void JS_DetachArrayBuffer(JSContext *ctx, JSValueConst obj);
uint8_t *JS_GetArrayBuffer(JSContext *ctx, size_t *psize, JSValueConst obj);
JSValue JS_GetTypedArrayBuffer(JSContext *ctx, JSValueConst obj,
                               size_t *pbyte_offset,
                               size_t *pbyte_length,
                               size_t *pbytes_per_element);

JSValue JS_NewPromiseCapability(JSContext *ctx, JSValue *resolving_funcs);

/* is_handled = TRUE means that the rejection is handled */
typedef void JSHostPromiseRejectionTracker(JSContext *ctx, JSValueConst promise,
                                           JSValueConst reason,
                                           JS_BOOL is_handled, void *opaque);
void JS_SetHostPromiseRejectionTracker(JSRuntime *rt, JSHostPromiseRejectionTracker *cb, void *opaque);

/* return != 0 if the JS code needs to be interrupted */
typedef int JSInterruptHandler(JSRuntime *rt, void *opaque);
void JS_SetInterruptHandler(JSRuntime *rt, JSInterruptHandler *cb, void *opaque);
/* if can_block is TRUE, Atomics.wait() can be used */
void JS_SetCanBlock(JSRuntime *rt, JS_BOOL can_block);

typedef struct JSModuleDef JSModuleDef;

/* return the module specifier (allocated with js_malloc()) or NULL if
   exception */
typedef char *JSModuleNormalizeFunc(JSContext *ctx,
                                    const char *module_base_name,
                                    const char *module_name, void *opaque);
typedef JSModuleDef *JSModuleLoaderFunc(JSContext *ctx,
                                        const char *module_name, void *opaque);

/* module_normalize = NULL is allowed and invokes the default module
   filename normalizer */
void JS_SetModuleLoaderFunc(JSRuntime *rt,
                            JSModuleNormalizeFunc *module_normalize,
                            JSModuleLoaderFunc *module_loader, void *opaque);
/* return the import.meta object of a module */
JSValue JS_GetImportMeta(JSContext *ctx, JSModuleDef *m);
JSAtom JS_GetModuleName(JSContext *ctx, JSModuleDef *m);

/* JS Job support */

typedef JSValue JSJobFunc(JSContext *ctx, int argc, JSValueConst *argv);
int JS_EnqueueJob(JSContext *ctx, JSJobFunc *job_func, int argc, JSValueConst *argv);

JS_BOOL JS_IsJobPending(JSRuntime *rt);
int JS_ExecutePendingJob(JSRuntime *rt, JSContext **pctx);

/* Object Writer/Reader (currently only used to handle precompiled code) */
#define JS_WRITE_OBJ_BYTECODE (1 << 0) /* allow function/module */
#define JS_WRITE_OBJ_BSWAP    (1 << 1) /* byte swapped output */
uint8_t *JS_WriteObject(JSContext *ctx, size_t *psize, JSValueConst obj,
                        int flags);
#define JS_READ_OBJ_BYTECODE  (1 << 0) /* allow function/module */
#define JS_READ_OBJ_ROM_DATA  (1 << 1) /* avoid duplicating 'buf' data */
JSValue JS_ReadObject(JSContext *ctx, const uint8_t *buf, size_t buf_len,
                      int flags);
/* load the dependencies of the module 'obj'. Useful when JS_ReadObject()
   returns a module. */
int JS_ResolveModule(JSContext *ctx, JSValueConst obj);

/* C function definition */
typedef enum JSCFunctionEnum {  /* XXX: should rename for namespace isolation */
    JS_CFUNC_generic,
    JS_CFUNC_generic_magic,
    JS_CFUNC_constructor,
    JS_CFUNC_constructor_magic,
    JS_CFUNC_constructor_or_func,
    JS_CFUNC_constructor_or_func_magic,
    JS_CFUNC_f_f,
    JS_CFUNC_f_f_f,
    JS_CFUNC_getter,
    JS_CFUNC_setter,
    JS_CFUNC_getter_magic,
    JS_CFUNC_setter_magic,
    JS_CFUNC_iterator_next,
} JSCFunctionEnum;

typedef union JSCFunctionType {
    JSCFunction *generic;
    JSValue (*generic_magic)(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, int magic);
    JSCFunction *constructor;
    JSValue (*constructor_magic)(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv, int magic);
    JSCFunction *constructor_or_func;
    double (*f_f)(double);
    double (*f_f_f)(double, double);
    JSValue (*getter)(JSContext *ctx, JSValueConst this_val);
    JSValue (*setter)(JSContext *ctx, JSValueConst this_val, JSValueConst val);
    JSValue (*getter_magic)(JSContext *ctx, JSValueConst this_val, int magic);
    JSValue (*setter_magic)(JSContext *ctx, JSValueConst this_val, JSValueConst val, int magic);
    JSValue (*iterator_next)(JSContext *ctx, JSValueConst this_val,
                             int argc, JSValueConst *argv, int *pdone, int magic);
} JSCFunctionType;

JSValue JS_NewCFunction2(JSContext *ctx, JSCFunction *func,
                         const char *name,
                         int length, JSCFunctionEnum cproto, int magic);
JSValue JS_NewCFunctionData(JSContext *ctx, JSCFunctionData *func,
                            int length, int magic, int data_len,
                            JSValueConst *data);

static inline JSValue JS_NewCFunction(JSContext *ctx, JSCFunction *func, const char *name,
                                      int length)
{
    return JS_NewCFunction2(ctx, func, name, length, JS_CFUNC_generic, 0);
}

static inline JSValue JS_NewCFunctionMagic(JSContext *ctx, JSCFunctionMagic *func,
                                           const char *name,
                                           int length, JSCFunctionEnum cproto, int magic)
{
    return JS_NewCFunction2(ctx, (JSCFunction *)func, name, length, cproto, magic);
}
void JS_SetConstructor(JSContext *ctx, JSValueConst func_obj, 
                       JSValueConst proto);

/* C property definition */

typedef struct JSCFunctionListEntry {
    const char *name;
    uint8_t prop_flags;
    uint8_t def_type;
    int16_t magic;
    union {
        struct {
            uint8_t length; /* XXX: should move outside union */
            uint8_t cproto; /* XXX: should move outside union */
            JSCFunctionType cfunc;
        } func;
        struct {
            JSCFunctionType get;
            JSCFunctionType set;
        } getset;
        struct {
            const char *name;
            int base;
        } alias;
        struct {
            const struct JSCFunctionListEntry *tab;
            int len;
        } prop_list;
        const char *str;
        int32_t i32;
        int64_t i64;
        double f64;
    } u;
} JSCFunctionListEntry;

#define JS_DEF_CFUNC          0
#define JS_DEF_CGETSET        1
#define JS_DEF_CGETSET_MAGIC  2
#define JS_DEF_PROP_STRING    3
#define JS_DEF_PROP_INT32     4
#define JS_DEF_PROP_INT64     5
#define JS_DEF_PROP_DOUBLE    6
#define JS_DEF_PROP_UNDEFINED 7
#define JS_DEF_OBJECT         8
#define JS_DEF_ALIAS          9

/* Note: c++ does not like nested designators */
#define JS_CFUNC_DEF(name, length, func1) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, 0, .u = { .func = { length, JS_CFUNC_generic, { .generic = func1 } } } }
#define JS_CFUNC_MAGIC_DEF(name, length, func1, magic) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, magic, .u = { .func = { length, JS_CFUNC_generic_magic, { .generic_magic = func1 } } } }
#define JS_CFUNC_SPECIAL_DEF(name, length, cproto, func1) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, 0, .u = { .func = { length, JS_CFUNC_ ## cproto, { .cproto = func1 } } } }
#define JS_ITERATOR_NEXT_DEF(name, length, func1, magic) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_CFUNC, magic, .u = { .func = { length, JS_CFUNC_iterator_next, { .iterator_next = func1 } } } }
#define JS_CGETSET_DEF(name, fgetter, fsetter) { name, JS_PROP_CONFIGURABLE, JS_DEF_CGETSET, 0, .u = { .getset = { .get = { .getter = fgetter }, .set = { .setter = fsetter } } } }
#define JS_CGETSET_MAGIC_DEF(name, fgetter, fsetter, magic) { name, JS_PROP_CONFIGURABLE, JS_DEF_CGETSET_MAGIC, magic, .u = { .getset = { .get = { .getter_magic = fgetter }, .set = { .setter_magic = fsetter } } } }
#define JS_PROP_STRING_DEF(name, cstr, prop_flags) { name, prop_flags, JS_DEF_PROP_STRING, 0, .u = { .str = cstr } }
#define JS_PROP_INT32_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_INT32, 0, .u = { .i32 = val } }
#define JS_PROP_INT64_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_INT64, 0, .u = { .i64 = val } }
#define JS_PROP_DOUBLE_DEF(name, val, prop_flags) { name, prop_flags, JS_DEF_PROP_DOUBLE, 0, .u = { .f64 = val } }
#define JS_PROP_UNDEFINED_DEF(name, prop_flags) { name, prop_flags, JS_DEF_PROP_UNDEFINED, 0, .u = { .i32 = 0 } }
#define JS_OBJECT_DEF(name, tab, len, prop_flags) { name, prop_flags, JS_DEF_OBJECT, 0, .u = { .prop_list = { tab, len } } }
#define JS_ALIAS_DEF(name, from) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_ALIAS, 0, .u = { .alias = { from, -1 } } }
#define JS_ALIAS_BASE_DEF(name, from, base) { name, JS_PROP_WRITABLE | JS_PROP_CONFIGURABLE, JS_DEF_ALIAS, 0, .u = { .alias = { from, base } } }

void JS_SetPropertyFunctionList(JSContext *ctx, JSValueConst obj,
                                const JSCFunctionListEntry *tab,
                                int len);

/* C module definition */

typedef int JSModuleInitFunc(JSContext *ctx, JSModuleDef *m);

JSModuleDef *JS_NewCModule(JSContext *ctx, const char *name_str,
                           JSModuleInitFunc *func);
/* can only be called before the module is instantiated */
int JS_AddModuleExport(JSContext *ctx, JSModuleDef *m, const char *name_str);
int JS_AddModuleExportList(JSContext *ctx, JSModuleDef *m,
                           const JSCFunctionListEntry *tab, int len);
/* can only be called after the module is instantiated */
int JS_SetModuleExport(JSContext *ctx, JSModuleDef *m, const char *export_name,
                       JSValue val);
int JS_SetModuleExportList(JSContext *ctx, JSModuleDef *m,
                           const JSCFunctionListEntry *tab, int len);

#undef js_unlikely
#undef js_force_inline

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* QUICKJS_H */
