#ifndef __COMPILER_LD_H
#define __COMPILER_LD_H

#ifndef ATTR_UNI_SYMBOL
#define ATTR_STRINGIFY(x) #x
#define ATTR_TOSTRING(x)  ATTR_STRINGIFY(x)
#define ATTR_UNI_SYMBOL   __FILE__ ATTR_TOSTRING(__LINE__)
#endif

#ifndef ATTR_CLOCK_SECTION
#define ATTR_CLOCK_SECTION __attribute__((section(".sclock_rlt_code." ATTR_UNI_SYMBOL)))
#endif

#ifndef ATTR_CLOCK_CONST_SECTION
#define ATTR_CLOCK_CONST_SECTION __attribute__((section(".sclock_rlt_const." ATTR_UNI_SYMBOL)))
#endif

#ifndef ATTR_TCM_SECTION
#define ATTR_TCM_SECTION __attribute__((section(".tcm_code." ATTR_UNI_SYMBOL)))
#endif

#ifndef ATTR_TCM_CONST_SECTION
#define ATTR_TCM_CONST_SECTION __attribute__((section(".tcm_const." ATTR_UNI_SYMBOL)))
#endif

#ifndef ATTR_DTCM_SECTION
#define ATTR_DTCM_SECTION __attribute__((section(".tcm_data")))
#endif

#ifndef ATTR_HSRAM_SECTION
#define ATTR_HSRAM_SECTION __attribute__((section(".hsram_code")))
#endif

#ifndef ATTR_NOCACHE_RAM_SECTION
#define ATTR_NOCACHE_RAM_SECTION __attribute__((section(".nocache_ram")))
#endif

#ifndef ATTR_NOCACHE_NOINIT_RAM_SECTION
#define ATTR_NOCACHE_NOINIT_RAM_SECTION __attribute__((section(".nocache_noinit_ram")))
#endif

#ifndef ATTR_WIFI_RAM_SECTION
#define ATTR_WIFI_RAM_SECTION           __attribute__((section(".wifi_ram")))
#endif

#ifndef ATTR_NOINIT_PSRAM_SECTION
#define ATTR_NOINIT_PSRAM_SECTION       __attribute__((section(".psram_noinit")))
#endif

#ifndef ATTR_PSRAM_SECTION
#define ATTR_PSRAM_SECTION              __attribute__((section(".psram_data")))
#endif

#ifndef ATTR_HBN_CODE_SECTION
#define ATTR_HBN_CODE_SECTION            __attribute__((section(".hbn_code." ATTR_UNI_SYMBOL)))
#endif

#ifndef ATTR_HBN_DATA_SECTION
#define ATTR_HBN_DATA_SECTION           __attribute__((section(".hbn_data." ATTR_UNI_SYMBOL)))
#endif

#ifndef ATTR_HBN_CONST_SECTION
#define ATTR_HBN_CONST_SECTION          __attribute__((section(".hbn_data." ATTR_UNI_SYMBOL)))
#endif

#define ATTR_FALLTHROUGH()              __attribute__((fallthrough))

#endif
