#include <nautilus/nautilus.h>
#include <nautilus/naut_types.h>
#include <nautilus/naut_string.h>
#include <nautilus/libccompat.h>
#include <nautilus/math.h>


#define PRINT(fmt, args...)    nk_vc_printf(fmt, ##args)
#define PUTC(c)    nk_vc_putchar(c)
#define printf PRINT
#define fprintf(f,fmt, args...) PRINT(fmt, ##args)
#define _printf PRINT
#define _fprintf fprintf
#define _putc(c, stdout) PUTC(c)

#define stdin				((void*)0UL)
#define stdout				((void*)1UL)
#define stderr				((void*)2UL)

#define putc _putc

//TODO: consider replacing references to these types instead
#define int8_t  sint8_t
#define int16_t sint16_t
#define int32_t sint32_t
#define int64_t sint64_t


#define NAN {0.0/0.0};
#define INIFNITY {1.0 /0.0};
#define NEG_INF {-1.0/0.0};


//typedef _Bool BOOL;

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

