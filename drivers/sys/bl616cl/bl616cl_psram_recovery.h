#ifndef _BL616CL_PSRAM_RECOVERY_H
#define _BL616CL_PSRAM_RECOVERY_H

#include <stdint.h>

typedef struct {
    uint16_t psram_id;
    uint8_t size_enum;
    uint8_t drive_strength;
    uint16_t dqs_delay;
    uint8_t c_val;
} bl_lp_psram_cfg_t;


/**
 * @brief Save PSRAM recovery parameters for LP wakeup
 *
 * This function probes the current PSRAM configuration and stores the
 * result into the low-power shared parameter area used by the wakeup
 * recovery flow.
 */
void bl_lp_psram_para_save(void);

/**
 * @brief Recover PSRAM after wakeup
 *
 * This function restores PSRAM after LP wakeup by using the provided
 * PSRAM recovery parameters. The caller must pass a valid configuration
 * pointer.
 *
 * @param config PSRAM recovery configuration
 * @return 0 on success, non-zero on failure
 */
uint32_t bl_lp_psram_recovery_with_config(const bl_lp_psram_cfg_t *config);

int bl_lp_psram_resume(uint32_t pds_level);

/**
 * @brief Recover PSRAM after LP wakeup with the saved shared parameters
 */
uint32_t bl_lp_psram_recovery(void);

#endif /* _BL616CL_PSRAM_RECOVERY_H */
