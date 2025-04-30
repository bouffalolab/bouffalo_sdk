#if defined BL616
#include <bl616.h>
#include <bl616_hbn.h>
#include <bl616_sys.h>
#elif defined (BL702)
#include <bl702.h>
#include <bl702_hbn.h>
#include <bl702_sys.h>
#elif defined (BL702L)
#include <bl702l.h>
#include <bl702l_hbn.h>
#include <bl702l_sys.h>
#endif

#include <FreeRTOS.h>
#include <task.h>
#include <shell.h>

#include <openthread/thread.h>

#if defined (BL702L)
int cmd_uart_boot(int argc, char **argv)
{
    HBN_Set_User_Boot_Config(1);
    // BL_WR_WORD(HBN_BASE + HBN_RSV0_OFFSET, 'T' << 24 | 'R' << 16 | 'A' << 8 | 'U');

    bl_sys_reset_por();

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_uart_boot, uart_boot, enter uart boot mode);

#elif defined (BL702)
int cmd_uart_boot(int argc, char **argv)
{
    BL_WR_WORD(HBN_BASE + HBN_RSV0_OFFSET, 'T' << 24 | 'R' << 16 | 'A' << 8 | 'U');
    vTaskDelay(100);

    bl_sys_reset_por();

    return 0;
}

int cmd_usb_boot(int argc, char **argv)
{
    BL_WR_WORD(HBN_BASE + HBN_RSV0_OFFSET, 'B' << 16 | 'S' << 8 | 'U');
    vTaskDelay(100);

    bl_sys_reset_por();

    return 0;
}

SHELL_CMD_EXPORT_ALIAS(cmd_uart_boot, uart_boot, enter uart boot mode);
SHELL_CMD_EXPORT_ALIAS(cmd_usb_boot, usb_boot, enter usb boot mode);
#endif

int cmd_abort(int argc, char **argv)
{
    abort();
    return 0;
}
SHELL_CMD_EXPORT_ALIAS(cmd_abort, abort, user calls abort);
