# Bouffalolab LHAL Release v1.6.0

- [feat][usb][bl808] emove usb driver for bl808
- [fix][BL616][BL616L][AudioDAC] Fix the audioDAC data format configuration error.
- [fix][BLXXX][Build] modify (void*) calc to (uint8_t*) calc
- [fix]fix CMakeLists.txt of bflb_sec_gmac
- [improve][BL616][ADC][SPI][USB] sync code from zephyr sdk
- [improve][BL616][Security] Add support for app rollback prevention
- [improve][BL808][Build] Fix BL808 compile warning


# Bouffalolab LHAL Release v1.5.0

- [fix][dbi] support dbi for bl808
- [fix][clk] add CLOKOUT function for BL702L
- [fix][rtc] fix rtc error
- [fix][adc] fix adc diff mode
- [fix][adc] fix tsen config error
- [fix][pwm] fix bflb_pwm_v2_get_frequency function
- [fix][emac] Fix the RX clock timing issue and the reentrancy issue in the tx/rx push.
- [fix][efuse] add bflb_power_on_efuse and bflb_power_off_efuse for program operation
- [fix][irq] add r2b/dtsrc/osd irq number
- [fix][dma/uart] update uart2 irq number and dma request number
- [fix][gpio] fix gpio not clear unregistered interrupt issue and make bflb_lhal_assert_func always inline
- [fix][dma] update spi1 dma request number
- [fix][timer/ir] fix warning in bflb_timer.c and update bflb_ir.c
- [fix][romapi] fix romapi return and unused warning
- [fix][ir] update ir driver
- [fix][mjpeg] fix mjpeg kick address update
- [fix][memorymap] fix ram memory map
- [fix][flash] fix api for romdriver
- [fix][romapi] fix romapi jump out of range issue
- [fix][adc] fix adc fifo threshold, wo threshold
- [fix][dbi] update fifo_cfg reg
- [fix] fix result parse and add 2 cmds
- [fix] fix channels conflict between internal and external
- [fix][uart] modify UART3 to UART2, remove UART2
- [fix][spi] add SPI1 function
- [fix][GPIO] add GPIO_FUNC_GPIO define
- [fix][flash] fix flash secreg port error
- [fix][mjpeg] fix INVERSE_YUYV2UYVY function
- [fix][ir] fix ir compiler error and efuse warning, add pka lib for bl70x,bl808
- [fix][gpio] fix all chips GPIO number error
- [fix][gpio] fix bl808 GPIO_PIN_MAX error
- [fix][romapi] fix romapi in flash secreg_port
- [fix][devicetable] fix bootrom use device table as ram instead of const issue
- [fix][sf_ctrl] fix sf_trl not support software key issue
- [fix][flash_secreg] fix flash_secreg romapi error
- [fix][sdh] Fixed ADMA2 address alignment definition for SDH
- [fix][flash] fix flash driver compile error
- [fix][flash] remove global data in flash file
- [fix][assert] bflb_lhal_assert_func use ebreak instead of printf
- [fix][common] fix bflb_check_cache_addr error and make antirollback api into romdriver
- [fix][romdriver] fix for compatible with romdriver
- [fix][uart] add uart2 in device_table
- [fix][adc] fix ADC bflb_adc_get_int_fifo_status error
- [fix][pad] fix PAD function
- [fix][adc] fix adc fifo2 enable
- [fix][sec_eng]fix sec eng set hardware key source error
- [fix][efuse]Fix efuse write with only program and read with only reload
- [fix][usb_v2] Fix the interrupt issue for FIFO4-FIFO7
- [fix][adc] fix offset between two ADC, force unused scan channels to null
- [fix][emac] Fixed EMAC compilation errors and added support for 10M mode.
- [fix][flash] Fix compilation errors related to flash.
- [fix][mjpeg] fix yuv420 format bug
- [fix][i2c/uart/dma/trng] update i2c/uart/dma/trng driver
- [fix][uart] fix uart DEGLITCH_CNT name

- [feat][kys] feat kys reg
- [feat][clock] feat adc clk gate
- [feat][clk] feat issue of usb clock gate
- [feat][trng] feat issue of group0 request trng_access
- [feat][romdriver] update lhal rom driver
- [feat][flash] add bflb_sf_ctrl_set_cs_clk_delay and bflb_sf_ctrl_set_dqs_delay API
- [feat][memory] remove busremap memory
- [feat][romdriver] update gpio api
- [feat][spi_psram]remove spi psram include std register
- [feat][romdriver] add sha crc32 api
- [feat][romdriver] remove useless api
- [feat][romapi] add romapi macro for romapi function
- [feat][romdriver] remove useless moudle
- [feat][flash] make L1C_Set_Wrap only for BL602,702,702l
- [feat][dma/spi/timer/uart] update dma/spi/timer/uart driver
- [feat][pka] add bl602 pka lib
- [feat][efuse]add bflb_efuse_read_mac_address_opt declaration
- [feat][romdriver]update romdriver API
- [feat][flash_secreg]change flash_cfg type into pointer in bflb_flash_otp_config_t
- [feat][romapi]update ROMAPI and ROMDriver
- [feat][adc] add ANALOG and GPIO function
- [feat][adc] add adc and mjdec romapi
- [feat][pka] add more pka lib according to chip type
- [feat][efuse]update bl616 efuse driver
- [feat][mjdec] add mjdec driver
- [feat] add swap mode
- [feat][ipc] add ipc driver
- [feat][dma] add dma address
- [feat][sdio] update sdio2/sdio3 driver
- [feat][emac] add emac set speed mode support
- [feat][sdio3] Added support for DMA1(SDMA)
- [feat] extend subaddress and data max length, add timeout feature when write data
- [feat][pwm] add pwm center-aligned counter mode, pwm update control, pwm/mjpeg hw_version/sw_usage
- [feat][mjpeg] add the function of modifing input address in kick mode
- [update][cache] update for flash_xip_read of bl602
- [update][gpio] update gpio and emac api
- [update][dma] update for dma and cache aligned
- [update][clk] modify rtc clock
- [update][timer] check parameters of timer comp value
- [update][flash] add ATTR_TCM_SECTION attribute
- [update][flash] update get flash io and cs delay api
- [update][compiler] add ATTR_NOCACHE_NOINIT_LPFW_SHARE attribute
- [update][boot2] add api of getting boot2 version
- [update][clock] update PERIPHERAL_CLOCK_xxx_ENABLE
- [update][sec_eng] update sec_eng crc32 link
- [update][romdriver] update lhal rom driver
- [update][dma] update dma request name for timer
- [update][romdriver] update romdriver for adc
- [update][romdriver] add arch mem cmp api
- [update] update anti-rollback enable address
- [update][flash] update flash otp with romapi
- [update][romdriver] romdriver remove bflb_flash.c
- [update][romdriver] update romdriver and ef_ctrl.h
- [update][efuse] update efuse busy api
- [update][dma] update interrupt number and dma request
- [update][adc] update gpadc_conv_delay field, position from bit4 to bit 24, bits count from 4 to 8
- [update][romdriver] update lhal romdriver
- [update][irq]update irq num
- [update][romdriver]  update lhal romdriver address and add sdh device
- [update][flash] update the flash jedec id order in bflb_flash_get_jedec_id
- [update][flash] update flash otp multi-configs support
- [update] add abr disable function
- [update][memorymap] update memorymap
- [update][flash] update bflb_sf_ctrl_aes_set_decrypt_region API


Change Log Since v1.4.0 Release

# Bouffalolab LHAL Release v1.4.0

Change Log Since v1.3.0 Release

- [update] remove romdriver define for bflb_get_boot2_info_from_flash
- [update][boot2] add api of getting boot2 version
- [update][rtc]add api of rtc utc timestamp
- [fix][dbi] Fixed DBI clock calculation errors, and add ATTR_PSRAM_CODE_SECTION
- [fix][adc] remove gaincal when GPADC use tsen
- [update][acomp]add api of acomp_get_postive_input
- [update][boot2] update anti-rollback api in common
- [feat][sdh] Support smih sdh driver
- [feat][emac] rewrite the emac driver
- [feat][spi] add spi 3-pin mode, add get_fifo_cnt CMD for i2s
- [fix][sdio] update sdio2/sdio3 driver
- [update][flash_otp] update write and read register command in otp cfg
- [fix][bl602][flash] fix bl602 flash controller address error issue
- [fix][i2c] fix I2C bug when use 0 as data length
- [update] update flash otp multi-config support
- [feat][sdio3] Add SDIO3.0(SMID) LHAL driver
- [fix][adc] adc only calibrate one time after power on
- [fix][dma] Fixed an issue where dma irq_attach only takes effect for the first time
- [feat][spi] add spi de-glitch feature control
- [fix][wo] add WO to device_table
- [feat] add get app version api for anti-rollback function
- [feat][sdh] add bflb_sdh

# Bouffalolab LHAL Release v1.3.0

Change Log Since v1.2.0 Release

- [fix][efuse]fix bflb_ef_ctrl_write_common_trim putreg32 bug
- [fix]fix efuse trim read/write en address range error
- [fix][ef_ctrl] fix bflb_ef_ctrl_write_direct when pword=NULL, need to check program
- [fix][efuse] fix return para for bflb_efuse_get_chipid
- [fix] fix romapi return and input
- [update][cam] move some configs into feature control
- [update][common] remove crc32 table
- [fix] remove some flash cfg for bl808 boot2
- [update] remove platform dma reg
- [fix][auadc] fix romapi name
- [update][adc] add adc clear fifo cmd
- [fix][adc] remove adc channel 11 in bl616
- [fix][trng] add lock for random
- [refactor] remove platform dma
- [feat] add process corner interface
- [fix][config] remove bl616 dma ch4~7
- [fix][auadc] Remove unsupported analog channels of AUADC.
- [fix][gpio] delete api of 32bits write to GPIO, because GPIO mode is set/clr
- [fix][dac] fix dac offset in bl602
- [update][gpio] add check for pin
- [fix][lhal] fix dma tc int mask

# Bouffalolab LHAL Release v1.2.0

Change Log Since v1.1.0 Release

- [update] change device string with macros
- [fix] modify flash cfg for reduce bl808 boot2 code size
- [update] add assert for init api
- [update] fix  BFLG to BFLB, add callapi content
- [feat] add bflb name macros
- [update] add romapi macros for later chips
- [update] assert when get name fail
- [update][uart] add uart tx done api
- [fix][I2C] fix subaddr var as uint32_t
- [update][uart] add uart tx flush api
- [update][i2c] add I2C_M_WRITE macro
- [fix][flash]fix flash chip erase timeout value overflow issu
- [update][flash] update puya 1M-32M,XTX 8M-16M flash for bl602/bl702/bl702l/bl616
- [update][irq] patch for iot sdk
- [update][flash] make bflb_flash_set_cmds private
- [feat] add flash security register api

# Bouffalolab LHAL Release v1.1.0

Change Log Since v1.0.0 Release

- [fix][I2C] add set/get timing command, and fix default timing
- [fix][usb] rename BLFB to BFLB
- [fix][mjpeg] fix warning
- [update][sf_cfg] adjust api position
- [update][mjpeg] move mjpeg const array into global
- [update][sec] use const regbase for trng
- [update][adc] update adc convert by yongjin
- [update] update adc trim by yongjin
- [update][ef_ctrl] replace strlen api with bflb_ef_ctrl_strlen
- [fix][acomp] fix acomp return type
- [fix][ef_ctrl] fix memcmp to arch_memcmp
- [fix][efuse] rename bflb_ef_ctrl_get_device_info to bflb_efuse_get_device_info
- [refactor][aes] refactor aes hwkey config api
- [fix][efuse] make bflb_efuse_device_info_type common for all chips
- [fix][sf_ctrl]fix flash config magic macro not in tcm section issue
- [update][flash] config flash 2line by user
- [fix] add wdt and reduce flash cfg to reduce code size for boot2
- [fix] fix sf_ctrl_cmds_cfg and sf_ctrl_cfg_type
- [update][flash] add gt25q64a/gt25q16b flash cfg
- [update][flash] because winb_16jv cfg changed, update winb_16dv flash cfg
- [fix][config] remove get id api
- [feat][flash] add config for flash 2line
- [update][efuse] remove aes read write api
- [update][flash] add PY25Q16HB/PY25Q32HB/SK25E032 flash cfg
- [doc][rtc] add comment for rtc
- [feat][rtc] add utc time suppport
- [update]update hbn section
- [fix][dma] fix dma2 arr index
- [update][flash] update flash GT25Q32A config

# Bouffalolab LHAL Release v1.0.0

first release