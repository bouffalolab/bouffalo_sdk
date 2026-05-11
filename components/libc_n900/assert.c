#include <stdio.h>
#include <assert.h>

/* for exit() and abort() */
/* Picolibc signature: __assert(const char *file, const char *func, int line) */
void __attribute__ ((noreturn))
__assert (const char *file, const char *func, int line)
{
    printf("Assertion Failed, in %s %s:%d\r\n", func, file, line);
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
__assert_no_args(void)
{
    printf("Assertion Failed\r\n");
    asm("ebreak");
    while(1);
}

void __attribute__ ((noreturn))
abort(void) {
    asm("ebreak");
    while(1);
}
