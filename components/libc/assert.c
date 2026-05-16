#include <stdio.h>
#include <assert.h>

/* for exit() and abort() */
void __attribute__ ((noreturn))
#if defined(__PICOLIBC__)
__assert (const char *file, const char *cond, int line)
#else
__assert (const char *file, int line, const char *cond)
#endif
{
    printf("Assertion Failed: %s, in %s:%d\r\n", cond, file, line);
    asm("ebreak");
    while(1);
}

void __attribute__ ((noreturn))
__assert_func (const char *file, int line, const char *func, const char *cond)
{
    printf("Assertion Failed: %s, in %s %s:%d\r\n", cond, func, file, line);
    asm("ebreak");
    while(1);
}

void __attribute__ ((noreturn))
abort(void) {
    asm("ebreak");
    while(1);
}
