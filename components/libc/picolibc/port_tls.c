#include <picotls.h>

extern char __boot_tls_start[];

void bflb_picolibc_init_boot_tls(void)
{
    _init_tls(__boot_tls_start);
    _set_tls(__boot_tls_start);
}
