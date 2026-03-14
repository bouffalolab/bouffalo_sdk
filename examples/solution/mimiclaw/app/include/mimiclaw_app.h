#ifndef MIMICLAW_APP_H
#define MIMICLAW_APP_H

void mimiclaw_app_init(void);
void mimiclaw_app_start(void);
void mimiclaw_app_stop(void);

void mimiclaw_app_on_network_ready(void);
void mimiclaw_app_on_network_lost(void);

#endif
