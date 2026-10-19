#include <stdlib.h>
#include <string.h>
#include "bl_lp_internal.h"

static struct lp_env *gp_lp_env = NULL;

void bl_lp_env_init(void)
{
    assert(!gp_lp_env);

    gp_lp_env = malloc(sizeof(struct lp_env));
    assert(gp_lp_env);

    memset(gp_lp_env, 0, sizeof(struct lp_env));
}

int bl_lp_sys_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    if (enter_callback == NULL && exit_callback == NULL) {
        return -1;
    }

    env->sys_pre_enter_callback = enter_callback;
    env->sys_after_exit_callback = exit_callback;
    env->sys_enter_arg = enter_arg;
    env->sys_exit_arg = exit_arg;

    return 0;
}

int bl_lp_user_callback_register(bl_lp_cb_t enter_callback, void *enter_arg, bl_lp_cb_t exit_callback, void *exit_arg)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    if (enter_callback == NULL && exit_callback == NULL) {
        return -1;
    }

    env->user_pre_enter_callback = enter_callback;
    env->user_after_exit_callback = exit_callback;
    env->user_enter_arg = enter_arg;
    env->user_exit_arg = exit_arg;

    return 0;
}

void bl_lp_call_user_pre_enter(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    if (env->user_pre_enter_callback) {
        env->user_pre_enter_callback(env->user_enter_arg);
    }
}

void bl_lp_call_user_after_exit(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    if (env->user_after_exit_callback) {
        env->user_after_exit_callback(env->user_exit_arg);
    }
}

void bl_lp_call_sys_pre_enter(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    if (env->sys_pre_enter_callback) {
        env->sys_pre_enter_callback(env->sys_enter_arg);
    }
}

void bl_lp_call_sys_after_exit(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    if (env->sys_after_exit_callback) {
        env->sys_after_exit_callback(env->sys_exit_arg);
    }
}

void bl_lp_set_resume_wifi(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    env->wifi_hw_resume = 1;
}

void bl_lp_clear_resume_wifi(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    env->wifi_hw_resume = 0;
}

int bl_lp_get_resume_wifi(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    return env->wifi_hw_resume;
}

void bl_set_fw_ready(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    env->wifi_fw_ready = 1;
}

void bl_clear_fw_ready(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    env->wifi_fw_ready = 0;
}

int bl_check_fw_ready(void)
{
    assert(gp_lp_env);

    struct lp_env *env = gp_lp_env;

    return env->wifi_fw_ready;
}
