#ifndef CHERRYUSB_ADB_USB_CONFIG_H
#define CHERRYUSB_ADB_USB_CONFIG_H

#include "cherryusb_config_template.h"

/* The demo only uses the ADB device class, so the defaults from the
 * CherryUSB configuration template are sufficient. High-speed support is
 * enabled on every chip except BL702 (full-speed only controller). */
#ifndef BL702
#define CONFIG_USB_HS
#endif

#endif /* CHERRYUSB_ADB_USB_CONFIG_H */
