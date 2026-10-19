#ifndef TICKLESS_H
#define TICKLESS_H

#include <stdint.h>

int tickless_enter(void);
int tickless_exit(void);
int *tickless_handke_get(void);

#if defined(CFG_BLE_ENABLE) && defined(BL618DG)
/* Override LPFW RC32K triggering (0/1) until reboot, applied at the next sleep hand-off. */
int bl_lp_ble_rc32k_trigger_set(uint8_t enable);
#endif
#endif
