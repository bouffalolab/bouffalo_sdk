#ifndef __WIFI_MGMR_EVENT_H__
#define __WIFI_MGMR_EVENT_H__
int wifi_mgmr_event_init(void);
extern void wifi_mgmr_set_connect_stat_info(struct wifi_event_sm_connect_ind *ind, uint8_t ind_type);
#endif
