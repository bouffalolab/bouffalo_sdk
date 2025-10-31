#include <stdio.h>
#include <sdio_port.h>
#include <hostrouter_init.h>
#include <sdiowifi_mgmr.h>
#include <sdiowifi_platform_adapt.h>

extern struct sdiowifi_mgmr g_sdiowifi_mgmr;

int hostrouter_init(void)
{
    printf("[%ld] hostrouter sdiowifi version:%d.%d.%d:%d\r\n",
        sdiowifi_tick_now(0),
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
    printf("[%ld] hostrouter sdiowifi version:%d.%d.%d:%d\r\n",
        sdiowifi_tick_now(0),
        HOSTROUTER_VERSION_MAJOR,
        HOSTROUTER_VERSION_MINOR,
        HOSTROUTER_VERSION_REVISION,
        HOSTROUTER_VERSION_RC);

#ifdef SDIOWIFI_TTY
    printf("[%ld] hostrouter SDIOWIFI_TTY enable\r\n",
            sdiowifi_tick_now(0));
#endif
#ifdef TX_RETRY_ENABLE
    printf("[%ld] hostrouter TX_RETRY_ENABLE:%d\r\n",
            sdiowifi_tick_now(0),
            TX_RETRY_ENABLE);
#endif
#ifdef SDIOWIFI_HEARTBEAT
    printf("[%ld] hostrouter SDIOWIFI_HEARTBEAT enable\r\n");
#endif
#ifdef BL_SDIO_RX_MAX_PORT_NUM
    printf("[%ld] hostrouter BL_SDIO_RX_MAX_PORT_NUM:%d\r\n",
            sdiowifi_tick_now(0),
            BL_SDIO_RX_MAX_PORT_NUM);
#endif

    extern int g_tx_buffer_cnt;
    printf("[%ld] hostrouter sdio for wifi tx pbuf cnt:%d\r\n",
            sdiowifi_tick_now(0),
            g_tx_buffer_cnt);

#ifdef PM_PDS_LDO_LEVEL_DEFAULT
    printf("[%ld] hostrouter PM_PDS_LDO_LEVEL_DEFAULT:%d\r\n",
            sdiowifi_tick_now(0),
            PM_PDS_LDO_LEVEL_DEFAULT);
#endif
#ifdef LWIP_HEAP_SIZE
    printf("[%ld] hostrouter LWIP_HEAP_SIZE:%d\r\n",
            sdiowifi_tick_now(0),
            LWIP_HEAP_SIZE);
#endif
#ifdef CFG_TXDESC0 && CFG_TXDESC1 && CFG_TXDESC2 && CFG_TXDESC3 && CFG_TXDESC4
    printf("[%ld] hostrouter CFG_TXDESC0-4: %d-%d-%d-%d-%d, \r\n",
            sdiowifi_tick_now(0),
            CFG_TXDESC0 ,CFG_TXDESC1 ,CFG_TXDESC2 ,CFG_TXDESC3 ,CFG_TXDESC4);
#endif
#ifdef CFG_STA_MAX
    printf("[%ld] hostrouter CFG_STA_MAX:%d\r\n",
            sdiowifi_tick_now(0),
            CFG_STA_MAX);
#endif
    printf("[%ld] hostrouter HW rx rb start:0x%08lX, end:0x%08lX, r:0x%08lX, w:0x%08lX\r\n",
            sdiowifi_tick_now(0),
            *(volatile uint32_t *)0x24b081c8, *(volatile uint32_t *)0x24b081cc,
            *(volatile uint32_t *)0x24b081d0, *(volatile uint32_t *)0x24b081d4);

    // display stats
    trcver_sdio_stats_display();
    sdiowifi_txbuf_stats_display();

    return 0;
}
