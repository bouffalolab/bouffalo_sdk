#ifndef __OT_PDS_H__
#define __OT_PDS_H__

#include <ot_rom_pds.h>

void otPds_init(otPds_sleep_opCb_t callback);

uint32_t otPds_prepareFullStackSleep();
void otPds_setDelayBeforeRetry(uint32_t macRetryBackoffMinExp, uint32_t macRetryBackoffMaxExp);
void otPds_handleWakeup(bool isWakeupFromRtc);
void otPds_setDataPollCsma(bool isEnableCsma);

#endif