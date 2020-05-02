#include <nautilus/nautilus.h>
#include <nautilus/naut_types.h>
#include <nautilus/naut_string.h>
//#include <nautilus/libccompat.h>


#define PRINT(fmt, args...)    nk_vc_printf(fmt, ##args)
#define PUTC(c)    nk_vc_putchar(c)
#define printf PRINT
#define fprintf(f,fmt, args...) PRINT(fmt, ##args)
#define _printf PRINT
#define _fprintf fprintf
#define _putc PUTC
