#include "coex.h"

#if defined(CONFIG_COEX_PTI)

int coex_init_pti(int role, void* arg)
{
    return COEX_OK;
}

int coex_deinit_pti(int role, void* arg)
{
    return COEX_OK;
}

int coex_event_pti(struct coex_evt_arg* arg)
{
    return COEX_OK;
}
#endif // defined(CONFIG_COEX_PTI)