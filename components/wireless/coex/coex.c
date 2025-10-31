#include "coex.h"

#if defined(CONFIG_COEX_TDMA) || defined(CONFIG_COEX_PTI)
#if defined(CONFIG_COEX_TDMA)
int coex_init_tdma(void);
int coex_deinit_tdma(void);
int coex_event_tdma(struct coex_evt_arg* arg);
#endif
#if defined(CONFIG_COEX_PTI)
int coex_init_pti(void);
int coex_deinit_pti(void);
int coex_event_pti(struct coex_evt_arg* arg);
#endif

static int coex_mode[COEX_ROLE_MAX] = {
    [COEX_ROLE_BT]     = CONFIG_COEX_BT_MODE,
    [COEX_ROLE_WIFI]   = CONFIG_COEX_WIFI_MODE,
    [COEX_ROLE_THREAD] = CONFIG_COEX_THREAD_MODE,
};

int coex_event(struct coex_evt_arg* arg)
{
    if (!arg || arg->role >= COEX_ROLE_MAX || arg->type >= COEX_EVT_MAX)
    {
        return COEX_FAIL;
    }

    switch (coex_mode[arg->role])
    {
#if defined(CONFIG_COEX_TDMA)
    case COEX_MODE_TDMA:
        return coex_event_tdma(arg);
#endif
#if defined(CONFIG_COEX_PTI)
    case COEX_MODE_PTI:
        return coex_event_pti(arg);
#endif
    default:
        return COEX_OK;
    }
}

int coex_init(void)
{
#if defined(CONFIG_COEX_TDMA)
    return coex_init_tdma();
#elif defined(CONFIG_COEX_PTI)
    return coex_init_pti();
#endif
}

int coex_deinit(void)
{
#if defined(CONFIG_COEX_TDMA)
    return coex_deinit_tdma();
#elif defined(CONFIG_COEX_PTI)
    return coex_deinit_pti();
#endif
}
#else
int coex_event(struct coex_evt_arg* arg)
{}

int coex_init(void)
{}

int coex_deinit(void)
{}
#endif // defined(CONFIG_COEX_TDMA) || defined(CONFIG_COEX_PTI)
