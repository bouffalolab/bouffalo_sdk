#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_flash_secreg.h"
#include "bflb_sf_cfg.h"

#define DBG_TAG "MAIN"
#include "log.h"

void ATTR_TCM_SECTION print_param(const bflb_flash_secreg_param_t *param)
{
    LOG_I("Security Register Param\r\n");
    LOG_I("region_offset = %u\r\n", param->region_offset);
    LOG_I("region_count  = %u\r\n", param->region_count);
    LOG_I("region_size   = %u\r\n", param->region_size * 256);
    LOG_I("secreg_size   = %u\r\n", param->secreg_size * 256);
    LOG_I("api_type      = %u\r\n", param->api_type);
    LOG_I("lb_share      = %u\r\n", param->lb_share);
    LOG_I("lb_offset     = %u\r\n", param->lb_offset);
    LOG_I("lb_write_len  = %u\r\n", param->lb_write_len);
    LOG_I("lb_read_len   = %u\r\n", param->lb_read_len);
    LOG_I("================================\r\n");
}

bool ATTR_TCM_SECTION print_region(const bflb_flash_secreg_region_info_t *info, void *data)
{
    (void)data;

    LOG_I("Security Register [%d] Information\r\n", info->index);
    LOG_I("Address      [0x%08x]\r\n", info->address);
    LOG_I("PhysicalAddr [0x%08x]\r\n", info->physical_address);
    LOG_I("Region size  [%u] bytes\r\n", info->region_size);
    LOG_I("SecREG size  [%u] bytes\r\n", info->secreg_size);
    LOG_I("IsLocked     [%s]\r\n", info->locked ? "true" : "false");
    LOG_I("LockbitShare [%s]\r\n", info->lockbit_share ? "true" : "false");
    LOG_I("================================\r\n");
    return true;
}

static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(4) wbuf[8 * 1024];
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(4) w2buf[8 * 1024];
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(4) wrbuf[8 * 1024];
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(4) erbuf[8 * 1024];

bool ATTR_TCM_SECTION region_lock_test(const bflb_flash_secreg_region_info_t *info, void *data)
{
    uint32_t beg;
    uint32_t end;

    do {
        LOG_I("Try to erase region %u\r\n", info->index);
        if (info->locked) {
            LOG_W("Region is locked\r\n");
            LOG_W("Test region %u skiped\r\n", info->index);
            return true;
        }
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_erase_by_idx(info->index));
        end = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
        LOG_I("Erase region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");

        memset(erbuf, 0x55, info->secreg_size);

        LOG_I("Try to read after erase region %u\r\n", info->index);
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_read_by_idx(info->index, 0, erbuf, info->secreg_size));
        end = bflb_mtimer_get_time_us();
        LOG_I("Read region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");
        DBG_HEXDUMP(erbuf, info->secreg_size);
        for (uint32_t i = 0; i < info->secreg_size; i++) {
            if (erbuf[i] != 0xff) {
                LOG_E("Read after erase check faild, index %u offset %u\r\n", info->index, i);
                _ASSERT_TRUE_FUNC(false);
            }
        }

        LOG_I("Read after erase check ok\r\n");
        LOG_I("================================\r\n");

        for (uint32_t i = 0; i < info->secreg_size; i++) {
            wbuf[i] = i;
        }

        LOG_I("Try to write region %u\r\n", info->index);
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_write_by_idx(info->index, 0, wbuf, info->secreg_size));
        end = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
        LOG_I("Write region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");

        memset(wrbuf, 0xAA, info->secreg_size);

        LOG_I("Try to read after write region %u\r\n", info->index);
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_read_by_idx(info->index, 0, wrbuf, info->secreg_size));
        end = bflb_mtimer_get_time_us();
        LOG_I("Read region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");
        DBG_HEXDUMP(wrbuf, info->secreg_size);
        for (uint32_t i = 0; i < info->secreg_size; i++) {
            if (wrbuf[i] != wbuf[i]) {
                LOG_E("Read after write check faild, index %u offset %u\r\n", info->index, i);
                _ASSERT_TRUE_FUNC(false);
            }
        }

        LOG_I("Read after write check ok\r\n");
        LOG_I("================================\r\n");

        LOG_I("Try to lock region %u\r\n", info->index);
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_set_locked(info->index));
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
        LOG_I("Locked region %u success\r\n", info->index);
        LOG_I("================================\r\n");

        LOG_I("Try to get region lock %u\r\n", info->index);
        _ASSERT_EQUAL_FUNC(1, bflb_flash_secreg_get_locked(info->index));
        LOG_I("Region %u get locked success\r\n", info->index);
        LOG_I("================================\r\n");

        LOG_I("Try to erase locked region %u\r\n", info->index);
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_erase_by_idx(info->index));
        end = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
        LOG_I("Erase region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");

        memset(erbuf, 0x55, info->secreg_size);

        LOG_I("Try to read after erase locked region %u\r\n", info->index);
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_read_by_idx(info->index, 0, erbuf, info->secreg_size));
        end = bflb_mtimer_get_time_us();
        LOG_I("Read region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");
        DBG_HEXDUMP(erbuf, info->secreg_size);
        for (uint32_t i = 0; i < info->secreg_size; i++) {
            if (erbuf[i] != wbuf[i]) {
                LOG_E("Read after erase locked reigon check faild, index %u offset %u\r\n", info->index, i);
                _ASSERT_TRUE_FUNC(false);
            }
        }

        LOG_I("Read after erase locked region check ok\r\n");
        LOG_I("================================\r\n");

        memset(w2buf, 0, info->secreg_size);

        LOG_I("Try to write locked region %u\r\n", info->index);
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_write_by_idx(info->index, 0, w2buf, info->secreg_size));
        end = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
        LOG_I("Write locked region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");

        memset(wrbuf, 0xAA, info->secreg_size);

        LOG_I("Try to read after write locked region %u\r\n", info->index);
        beg = bflb_mtimer_get_time_us();
        _ASSERT_ZERO_FUNC(bflb_flash_secreg_read_by_idx(info->index, 0, wrbuf, info->secreg_size));
        end = bflb_mtimer_get_time_us();
        LOG_I("Read region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
        LOG_I("================================\r\n");
        DBG_HEXDUMP(wrbuf, info->secreg_size);
        for (uint32_t i = 0; i < info->secreg_size; i++) {
            if (wrbuf[i] != wbuf[i]) {
                LOG_E("Read after write locked region check faild, index %u offset %u\r\n", info->index, i);
                _ASSERT_TRUE_FUNC(false);
            }
        }

        LOG_I("Read after write locked region check ok\r\n");
        LOG_I("================================\r\n");

        LOG_I("All test in region %u success\r\n", info->index);
        LOG_I("================================\r\n");
    } while (0);

    return true;
}

int ATTR_TCM_SECTION main(void)
{
    const bflb_flash_secreg_param_t *param;
    uint8_t lb;

    board_init();

    LOG_I("Flash security register test\r\n");
    LOG_I("================================\r\n");
    bflb_mtimer_delay_ms(1000);

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_get_param(&param));
    print_param(param);

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_get_param(&param));

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_get_lockbits(&lb));
    LOG_I("lock bit is 0x%02x\r\n", lb);
    LOG_I("================================\r\n");

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_region_foreach(print_region, NULL));

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_region_foreach(region_lock_test, NULL));

    LOG_I("All test success\r\n");
    LOG_I("================================\r\n");

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
