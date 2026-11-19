#include <stddef.h>
#include "sysinit.h"

__attribute__((weak)) void board_early_init(void)
{
}

static void board_sysinit_early(const void *arg)
{
    (void)arg;
    board_early_init();
}
SYSINIT(board_early, SI_SUB_LOWEST, SI_ORDER_FIRST, board_sysinit_early, NULL);

__attribute__((weak)) void board_late_init(void)
{
}

static void board_sysinit_late(const void *arg)
{
    (void)arg;
    board_late_init();
}
SYSINIT(board_late, SI_SUB_DRIVERS, SI_ORDER_LAST, board_sysinit_late, NULL);
