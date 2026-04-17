#ifndef APP_USER_H
#define APP_USER_H

int app_pm_init(void);
int app_user_init(void);

int app_wifi_init(void);
void app_atmodule_init(void);
#ifdef CONFIG_MR_VIRTUALCHAN
void app_vchan_init(void);
#endif /* CONFIG_MR_VIRTUALCHAN */

#endif
