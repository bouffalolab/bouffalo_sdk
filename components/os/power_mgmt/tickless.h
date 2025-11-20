#ifndef TICKLESS_H
#define TICKLESS_H

#include <stdint.h>

int tickless_enter(void);
int tickless_exit(void);
int *tickless_handke_get(void);
#endif
