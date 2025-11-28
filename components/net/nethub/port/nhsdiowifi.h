/**
 * @file nhsdiowifi.h
 * @brief SDIO WiFi port adaptation layer header file
 */

#ifndef NHSDIOWIFI_H
#define NHSDIOWIFI_H

#include <stdint.h>
#include "nethub.h"
#include "nethub_types.h"

/* SDIO interface operation structure */
extern const struct nhif_ops nhsdio_ops;

/* STA interface operation structure */
extern const struct nhif_ops nhsta_ops;

/* TCPIPSTA interface operation structure */
extern const struct nhif_ops nhtcpipsta_ops;

/* AP interface operation structure */
extern const struct nhif_ops nhap_ops;

/* TCPIPAP interface operation structure */
extern const struct nhif_ops nhtcpipap_ops;

/* Nethub sdiowifi api */
int nethub_sdiowifi_init(void);
void nethub_sdiowifi_deinit(void);

#endif /* NHSDIOWIFI_H */
