#include <stdio.h>
#include <sdio_port.h>
#include <hostrouter_init.h>
#include <sdiowifi_mgmr.h>

extern struct sdiowifi_mgmr g_sdiowifi_mgmr;

int hostrouter_init(void)
{
    HR_LOGI("sdiowifi version:%d.%d.%d:%d\r\n",
        HOSTROUTER_VERSION_MAJOR,
        HOSTROUTER_VERSION_MINOR,
        HOSTROUTER_VERSION_REVISION,
        HOSTROUTER_VERSION_RC);

    sdiowifi_mgmr_start(&g_sdiowifi_mgmr);

    return 0;
}

int hostrouter_stats_display(void)
{
    extern void trcver_sdio_stats_display(void);
    extern void sdiowifi_txbuf_stats_display(void);

    // display config
    HR_LOGI("sdiowifi version:%d.%d.%d:%d\r\n",
        HOSTROUTER_VERSION_MAJOR,
        HOSTROUTER_VERSION_MINOR,
        HOSTROUTER_VERSION_REVISION,
        HOSTROUTER_VERSION_RC);

#ifdef SDIOWIFI_TTY
    HR_LOGI("SDIOWIFI_TTY enable\r\n");
#endif
#ifdef TX_RETRY_ENABLE
    HR_LOGI("TX_RETRY_ENABLE:%d\r\n", TX_RETRY_ENABLE);
#endif
#ifdef SDIOWIFI_HEARTBEAT
    HR_LOGI("SDIOWIFI_HEARTBEAT enable\r\n");
#endif
#ifdef BL_SDIO_RX_MAX_PORT_NUM
    HR_LOGI("BL_SDIO_RX_MAX_PORT_NUM:%d\r\n", BL_SDIO_RX_MAX_PORT_NUM);
#endif
#ifdef PM_PDS_LDO_LEVEL_DEFAULT
    HR_LOGI("PM_PDS_LDO_LEVEL_DEFAULT:%d\r\n", PM_PDS_LDO_LEVEL_DEFAULT);
#endif
#ifdef LWIP_HEAP_SIZE
    HR_LOGI("LWIP_HEAP_SIZE:%d\r\n", LWIP_HEAP_SIZE);
#endif
#ifdef CFG_TXDESC1
    HR_LOGI("CFG_TXDESC1:%d\r\n", CFG_TXDESC1);
#endif
#ifdef CFG_STA_MAX
    HR_LOGI("CFG_STA_MAX:%d\r\n", CFG_STA_MAX);
#endif

    // display stats
    trcver_sdio_stats_display();
    sdiowifi_txbuf_stats_display();

    return 0;
}

