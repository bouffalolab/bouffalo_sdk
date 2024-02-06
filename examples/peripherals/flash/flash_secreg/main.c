#include "bflb_mtimer.h"
#include "board.h"
#include "bflb_flash_secreg.h"
#include "bflb_sf_cfg.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define REGION_ERASE 0
#define REGION_WRITE 1
#define REGION_WREAD 2
#define REGION_EREAD 3

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

struct region_test_data {
    uint8_t test;
    const void *wbuf;
    void *wrbuf;
    void *erbuf;

    uint32_t write_byte;
    uint32_t erase_byte;
    uint32_t wread_byte;
    uint32_t eread_byte;
};

bool ATTR_TCM_SECTION region_rwe_test(const bflb_flash_secreg_region_info_t *info, void *data)
{
    uint32_t beg;
    uint32_t end;
    struct region_test_data *test = data;

    switch (test->test) {
        case REGION_ERASE:
            LOG_I("Try to erase region %u\r\n", info->index);
            if (info->locked) {
                LOG_W("Region is locked\r\n");
            }

            _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
            beg = bflb_mtimer_get_time_us();
            _ASSERT_ZERO_FUNC(bflb_flash_secreg_erase_by_idx(info->index));
            end = bflb_mtimer_get_time_us();
            _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
            test->erase_byte += info->secreg_size;
            LOG_I("Erase region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
            LOG_I("================================\r\n");
            break;

        case REGION_WRITE:
            LOG_I("Try to write region %u\r\n", info->index);
            if (info->locked) {
                LOG_W("Region is locked\r\n");
            }

            _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
            beg = bflb_mtimer_get_time_us();
            _ASSERT_ZERO_FUNC(bflb_flash_secreg_write_by_idx(info->index, 0, test->wbuf + info->secreg_size * info->index, info->secreg_size));
            end = bflb_mtimer_get_time_us();
            _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
            test->write_byte += info->secreg_size;
            LOG_I("Write region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
            LOG_I("================================\r\n");
            break;

        case REGION_WREAD:
            LOG_I("Try to read after write region %u\r\n", info->index);
            beg = bflb_mtimer_get_time_us();
            _ASSERT_ZERO_FUNC(bflb_flash_secreg_read_by_idx(info->index, 0, test->wrbuf + info->secreg_size * info->index, info->secreg_size));
            end = bflb_mtimer_get_time_us();
            test->wread_byte += info->secreg_size;
            LOG_I("Read region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
            LOG_I("================================\r\n");
            break;

        case REGION_EREAD:
            LOG_I("Try to read after erase region %u\r\n", info->index);
            beg = bflb_mtimer_get_time_us();
            _ASSERT_ZERO_FUNC(bflb_flash_secreg_read_by_idx(info->index, 0, test->erbuf + info->secreg_size * info->index, info->secreg_size));
            end = bflb_mtimer_get_time_us();
            test->eread_byte += info->secreg_size;
            LOG_I("Read region %u done, bytes %u cost %u us\r\n", info->index, info->secreg_size, end - beg);
            LOG_I("================================\r\n");
            break;

        default:
            LOG_W("Invalid test %u for region %u\r\n", test->test, info->index);
            LOG_I("================================\r\n");
            break;
    }

    return true;
}

static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(4) wbuf[16 * 1024];
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(4) wrbuf[16 * 1024];
static uint8_t ATTR_NOCACHE_NOINIT_RAM_SECTION __ALIGNED(4) erbuf[16 * 1024];

int ATTR_TCM_SECTION main(void)
{
    uint32_t beg;
    uint32_t end;
    const bflb_flash_secreg_param_t *param;
    struct region_test_data test;
    uint8_t lb;

    uint8_t count = 0;

    board_init();

    LOG_I("Flash security register test\r\n");
    LOG_I("================================\r\n");
    bflb_mtimer_delay_ms(1000);

    test.wbuf = wbuf;
    test.wrbuf = wrbuf;
    test.erbuf = erbuf;

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_get_param(&param));
    LOG_I("Test %d ok\r\n", ++count);
    print_param(param);

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_get_param(&param));
    LOG_I("Test %d ok\r\n", ++count);

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_get_lockbits(&lb));
    LOG_I("lock bit is 0x%02x\r\n", lb);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    _ASSERT_ZERO_FUNC(bflb_flash_secreg_region_foreach(print_region, NULL));
    LOG_I("Test %d ok\r\n", ++count);

    for (uint32_t i = 0; i < sizeof(wbuf); i++) {
        wbuf[i] = i;
        wrbuf[i] = 0x00;
        erbuf[i] = 0x55;
    }

    test.test = REGION_ERASE;
    test.erase_byte = 0;
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_region_foreach(region_rwe_test, (void *)&test));
    LOG_I("Total erase byte %u\r\n", test.erase_byte);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    test.test = REGION_EREAD;
    test.eread_byte = 0;
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_region_foreach(region_rwe_test, (void *)&test));
    LOG_I("Total read after erase byte %u\r\n", test.eread_byte);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    _ASSERT_EQUAL_PARAM(test.erase_byte, test.eread_byte);
    DBG_HEXDUMP(test.erbuf, test.eread_byte);
    for (uint32_t i = 0; i < test.eread_byte; i++) {
        if (((uint8_t *)(test.erbuf))[i] != 0xff) {
            LOG_E("Read after erase check faild, address 0x%08x\r\n", i);
            _ASSERT_TRUE_FUNC(false);
        }
    }
    LOG_I("Read after erase check ok\r\n");
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    test.test = REGION_WRITE;
    test.write_byte = 0;
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_region_foreach(region_rwe_test, (void *)&test));
    LOG_I("Total write byte %u\r\n", test.write_byte);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    test.test = REGION_WREAD;
    test.wread_byte = 0;
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_region_foreach(region_rwe_test, (void *)&test));
    LOG_I("Total read after write byte %u\r\n", test.wread_byte);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    _ASSERT_EQUAL_PARAM(test.write_byte, test.wread_byte);
    DBG_HEXDUMP(test.wrbuf, test.wread_byte);
    for (uint32_t i = 0; i < test.wread_byte; i++) {
        if (((uint8_t *)(test.wrbuf))[i] != ((uint8_t *)(test.wbuf))[i]) {
            LOG_E("Read after write check faild, address 0x%08x\r\n", i);
            _ASSERT_TRUE_FUNC(false);
        }
    }
    LOG_I("Read after write check ok\r\n");
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    for (uint32_t i = 0; i < sizeof(wbuf); i++) {
        wbuf[i] = i;
        wrbuf[i] = 0x00;
        erbuf[i] = 0x55;
    }

    test.erase_byte = param->region_count * param->secreg_size * 256;
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
    beg = bflb_mtimer_get_time_us();
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_erase(0, test.erase_byte));
    end = bflb_mtimer_get_time_us();
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
    LOG_I("Total erase byte %u cost %u us\r\n", test.erase_byte, end - beg);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    test.eread_byte = param->region_count * param->secreg_size * 256;
    beg = bflb_mtimer_get_time_us();
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_read(0, test.erbuf, test.eread_byte));
    end = bflb_mtimer_get_time_us();
    LOG_I("Total read after erase byte %u cost %u us\r\n", test.eread_byte, end - beg);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    _ASSERT_EQUAL_PARAM(test.erase_byte, test.eread_byte);
    DBG_HEXDUMP(test.erbuf, test.eread_byte);
    for (uint32_t i = 0; i < test.eread_byte; i++) {
        if (((uint8_t *)(test.erbuf))[i] != 0xff) {
            LOG_E("Read after erase check faild, address 0x%08x\r\n", i);
            _ASSERT_TRUE_FUNC(false);
        }
    }
    LOG_I("Read after erase check ok\r\n");
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    test.write_byte = param->region_count * param->secreg_size * 256;
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_opunlock());
    beg = bflb_mtimer_get_time_us();
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_write(0, test.wbuf, test.write_byte));
    end = bflb_mtimer_get_time_us();
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_oplock());
    LOG_I("Total write byte %u cost %u us\r\n", test.write_byte, end - beg);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    test.wread_byte = param->region_count * param->secreg_size * 256;
    beg = bflb_mtimer_get_time_us();
    _ASSERT_ZERO_FUNC(bflb_flash_secreg_read(0, test.wrbuf, test.wread_byte));
    end = bflb_mtimer_get_time_us();
    LOG_I("Total read after write byte %u cost %u us\r\n", test.wread_byte, end - beg);
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    _ASSERT_EQUAL_PARAM(test.write_byte, test.wread_byte);
    DBG_HEXDUMP(test.wrbuf, test.wread_byte);
    for (uint32_t i = 0; i < test.wread_byte; i++) {
        if (((uint8_t *)(test.wrbuf))[i] != ((uint8_t *)(test.wbuf))[i]) {
            LOG_E("Read after write check faild, address 0x%08x\r\n", i);
            _ASSERT_TRUE_FUNC(false);
        }
    }
    LOG_I("Read after write check ok\r\n");
    LOG_I("Test %d ok\r\n", ++count);
    LOG_I("================================\r\n");

    LOG_I("All test success\r\n");
    LOG_I("================================\r\n");

    while (1) {
        bflb_mtimer_delay_ms(2000);
    }
}
