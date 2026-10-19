#ifndef USB_CONSOLE_CONFIG_H
#define USB_CONSOLE_CONFIG_H

#include "cherryusb_config_template.h"

/* CherryUSB diagnostics cannot use the console implemented by CherryUSB. */
#undef CONFIG_USB_PRINTF
#define CONFIG_USB_PRINTF(...) ((void)0)
#undef CONFIG_USB_DBG_LEVEL
#define CONFIG_USB_DBG_LEVEL (-1)

#if defined(BL616) || defined(BL616CL) || defined(BL618DG)
#define CONFIG_USB_HS
#endif

#endif /* USB_CONSOLE_CONFIG_H */
