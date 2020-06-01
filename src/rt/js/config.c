#include <rt/js/config.h>

void MATHLIB_WARNING() { UNDEF_FUN_ERR(); }

double chebyshev_eval(double x, double* y, int z) { UNDEF_FUN_ERR(); }

double log1p(double x) {
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

  if (xmin == 0.0)
    xmin = -1 + sqrt(DBL_EPSILON); /*was sqrt(d1mach(4)); */
  if (nlnrel == 0)                 /* initialize chebychev coefficients */
    nlnrel = chebyshev_init(alnrcs, 43, DBL_EPSILON / 20); /*was .1*d1mach(3)*/
#else
#define nlnrel 22
  const static double xmin = -0.999999985;
/* 22: for IEEE double precision where DBL_EPSILON =  2.22044604925031e-16 */
#endif

  if (x == 0.)
    return 0.; /* speed */
  if (x == -1)
    return (ML_NEGINF);
  if (x < -1)
    ML_ERR_return_NAN;

  if (fabs(x) <= .375) {
    /* Improve on speed (only);
       again give result accurate to IEEE double precision: */
    if (fabs(x) < .5 * DBL_EPSILON)
      return x;

    if ((0 < x && x < 1e-8) || (-1e-9 < x && x < 0))
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

/**
 * Unsure if this has the intended behavior for cbrt -Aaron
 */
void getHighWordD(int32_t* h, double x) {
  union z {
    double d;
    int64_t i64;
  };
  union z k;
  k.d = x;
  *h = (k.i64 >> 16) & ((1L << 17) - 1);
}

/**
 * Unsure if this has the intended behavior for cbrt -Aaron
 */
void getLowWordD(int32_t* l, double x) {
  union z {
    double d;
    int64_t i64;
  };
  union z k;
  k.d = x;
  *l = k.i64 & ((1L << 17) - 1);
}

/**
 * Unsure if this has the intended behavior for cbrt -Aaron
 */
void setHighWordD(double* x, int32_t h) {
  union z {
    double d;
    int64_t i64;
  };
  union z k;
  k.d = *x;
  k.i64 &= ((1L << 17) - 1);
  k.i64 |= ((int64_t)h) << 32;
  *x = k.d;
}

/**
 * Unsure if this has the intended behavior for cbrt -Aaron
 */
void setLowWordD(double* x, int32_t l) {
  union z {
    double d;
    int64_t i64;
  };
  union z k;
  k.d = *x;
  k.i64 &= (((1L << 17) - 1) << 32);
  k.i64 |= l;
  *x = k.d;
}

/**
 * @brief Cube root function
 */
double cbrt(double x) {
  int32_t hx, lx, ht;
  double r, s, t = 0.0, w;
  uint32_t sign;

  getHighWordD(&hx, x);

  // sign = sign(x)
  sign = hx & 0x80000000;
  hx ^= sign;

  // cbrt(NaN,INF) is itself
  if (hx >= 0x7FF00000) {
    return x + x;
  }

  getLowWordD(&lx, x);

  // cbrt(0) is itself
  if ((hx | lx) == 0) {
    return x;
  }

  // x <- |x|
  setHighWordD(&x, hx);

  // rough cbrt to 5 bits
  if (hx < 0x00100000) // subnormal number
  {
    setHighWordD(&t, 0x43500000); // set t= 2**54
    t *= x;
    getHighWordD(&ht, t);
    setHighWordD(&t, ht / 3 + B2);
  } else {
    setHighWordD(&t, hx / 3 + B1);
  }

  // new cbrt to 23 bits, may be implemented in single precision
  r = t * t / x;
  s = C + r * t;
  t *= G + F / (s + E + D / s);

  // chopped to 20 bits and make it larger than cbrt(x)
  setHighWordD(&t, 0);
  getHighWordD(&ht, t);
  setHighWordD(&t, ht + 0x00000001);

  // one step newton iteration to 53 bits with error less than 0.667 ulps
  s = t * t; // t*t is exact
  r = x / s;
  w = t + t;
  r = (r - t) / (w + r); // r-s is exact
  t = t + t * r;

  // retore the sign bit
  getHighWordD(&ht, t);
  setHighWordD(&t, ht | sign);

  return (t);
}

// void assert(int x) {
//   if (!x) {
//     PRINT("FAIL ASSERT");
//     exit(1);
//   }
// }

char* realpath(const char* name, char* resolved) {
  UNDEF_FUN_ERR();
  return NULL;
}

FILE* popen(const char* filename, const char* mode) {
  UNDEF_FUN_ERR();
  return NULL;
}
int pclose(FILE* stream) {
  UNDEF_FUN_ERR();
  return 0;
}
int open(const char* pathname, int flags) {
  UNDEF_FUN_ERR();
  return 0;
}
int close(int fd) {
  UNDEF_FUN_ERR();
  return 0;
}

#define STUBBY     \
  UNDEF_FUN_ERR(); \
  return 0;

int ftello(FILE* x) { STUBBY }

int fseeko(FILE* x, int y, int z) { STUBBY }

int fgetc(FILE* x) { STUBBY }

int lseek(int x, int y, int z) { STUBBY }

int getcwd(char* x, int y) { STUBBY }
int chdir(const char* x) { STUBBY }
int mkdir(const char* x, int y) { STUBBY }
int symlink(const char* x, const char* y) { STUBBY }
int readlink(const char* x, char* y, int z) { STUBBY }
int utimes(const char* x, int y) { STUBBY }
int putchar(char x) { STUBBY }
int dup(int x) { STUBBY }
int dup2(int x, int y) { STUBBY }
int openm(const char* x, int y, int z) { STUBBY }
int perror(const char* x) { STUBBY }

int nanosleep(struct timespec* x, void* y) { STUBBY }

int waitpid(int x, int* y, int z) { STUBBY }

int pipe(int* x) { STUBBY }

int kill(int x, int y) { STUBBY }

int dlopen(char* x, int y) { STUBBY }

int dlsym(void* x, char* y) { STUBBY }

int dlclose(int x) { STUBBY }

int lrint(int x) { STUBBY }

int isfinite(int x) { STUBBY }

int alloca(int x) { STUBBY }

int atomic_fetch_add(void* x, int y) { STUBBY }
int atomic_fetch_and(void* x, int y) { STUBBY }
int atomic_fetch_or(void* x, int y) { STUBBY }
int atomic_fetch_sub(void* x, int y) { STUBBY }
int atomic_fetch_xor(void* x, int y) { STUBBY }
int atomic_exchange(void* x, int y) { STUBBY }
int atomic_store(void* x, int y) { STUBBY }
int atomic_compare_exchange_strong(void* x, void* y, int z) { STUBBY }
int atomic_load(void* x) { STUBBY }

int pthread_cond_timedwait(void* x, void* y, void* z) { STUBBY }
int pthread_cond_destroy(void* x) { STUBBY }
int pthread_cond_signal(void* x) { STUBBY }
int pthread_cond_init(void* x, int y) { STUBBY }
int pthread_cond_wait(void* x, void* y) { STUBBY }

int fmin(int x, int y) { STUBBY }
int fmax(int x, int y) { STUBBY }

int signbit(int x) { STUBBY }

int round(int x) { STUBBY }
int hypot(int x, int y) { STUBBY }

int countof(int x) { STUBBY }

int fesetround(int x) { STUBBY }