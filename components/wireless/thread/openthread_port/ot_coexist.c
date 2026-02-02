
#include <lmac154.h>
#include <openthread/thread.h>
#include <openthread/platform/alarm-micro.h>

#include <openthread_port.h>
#include <coex.h>

typedef void (*statChangedCallback_t)(otChangedFlags);

static void ot_coexist_notify_cb(void *env, struct coex_notify_args *args)
{
    (void)env;
    if (!args) {
        return;
    }
}

static int ot_coexist_event_pan_op(bool isStart)
{
    struct coex_evt_arg arg = {
        .role             = COEX_ROLE_THREAD,
        .type             = COEX_EVT_SET_ACTIVITY,
        .arg.set_act.type = isStart ? ACT_START_PAN : ACT_STOP_PAN,
    };

    return ((COEX_OK == coex_event(&arg)) ? 0 : -1);
}

static void ot_coexist_stateChangedCallback(otChangedFlags aFlags) 
{
    if (OT_CHANGED_THREAD_NETIF_STATE & aFlags) {
        ot_coexist_event_pan_op(otIp6IsEnabled(otrGetInstance()));
    }
}

static void ot_coexist_stateChangedCallback_context(otChangedFlags aFlags, void *aContext) 
{
    ot_coexist_stateChangedCallback(aFlags);
}

__attribute__((weak)) void otAddStateChangedCallback(otInstance *aInstance, statChangedCallback_t aCallback)
{
    /** No border router module linked, call otSetStateChangedCallback to setup state changed callback in thread stack */
    otSetStateChangedCallback(otrGetInstance(), ot_coexist_stateChangedCallback_context, NULL);
}

int ot_coexist_event_init(void)
{
    struct coex_evt_arg arg = {
        .role             = COEX_ROLE_THREAD,
        .type             = COEX_EVT_INIT,
        .arg.init.cb      = ot_coexist_notify_cb,
        .arg.init.env     = NULL,
    };

    if (COEX_OK != coex_event(&arg)) {
        return -1;
    }

    otAddStateChangedCallback(otrGetInstance(), ot_coexist_stateChangedCallback);

    lmac154_enableTxAbortInt();

    return 0;
}
