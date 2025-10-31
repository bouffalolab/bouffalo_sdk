#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <linux/proc_fs.h>

#include "version.h"
#include "main.h"
#include "tty.h"

struct bl_device gl_dev;

#ifdef BL_PROCFS_ENABLE
struct proc_dir_entry *bl_proc_dir = NULL;
#endif

int __init bl_mod_init(void)
{
    int ret;

    pr_info("blwnet version %d.%d\n",
            BL_WNET_MAJOR_VERSION, BL_WNET_MINOR_VERSION);
#ifdef BL_PROCFS_ENABLE
    bl_proc_dir = proc_mkdir("bleth_driver", NULL);
    if (!bl_proc_dir)
        pr_warn("failed to mkdir /proc/bleth_driver\n");
#endif
    mutex_init(&gl_dev.mutex);

#ifdef BL_INTF_SDIO
    if ((ret = bl_tty_init())) {
        return ret;
    }
#endif
    if ((ret = bl_register_ctl_port()) != 0) {
        return ret;
    }

    return bl_register_eth_drv();
}

void __exit bl_mod_exit(void)
{
    pr_info("removing bleth_driver\n");
    mutex_lock(&gl_dev.mutex);
    gl_dev.status |= BL_DEVICE_STATUS_DRV_REMOVING;
    mutex_unlock(&gl_dev.mutex);
    bl_release_ctl_port();
    bl_unregister_eth_drv();
#ifdef BL_INTF_SDIO
    bl_tty_exit();
#endif
#ifdef BL_PROCFS_ENABLE
    proc_remove(bl_proc_dir);
#endif
}

bool bl_card_ok(void)
{
    int nok = 0;
    if (!(gl_dev.status & BL_DEVICE_STATUS_CARD_PRESENT)) {
        ++nok;
    }
#if CARD_DEAD_CHECK
    if (gl_dev.status & BL_DEVICE_STATUS_CARD_DEAD) {
        ++nok;
    }
#endif
    if (gl_dev.status & BL_DEVICE_STATUS_CARD_REMOVING) {
        ++nok;
    }
    if (gl_dev.status & BL_DEVICE_STATUS_DRV_REMOVING) {
        ++nok;
    }
    return !nok;
}

module_init(bl_mod_init);
module_exit(bl_mod_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Bouffalolab Device net driver");
