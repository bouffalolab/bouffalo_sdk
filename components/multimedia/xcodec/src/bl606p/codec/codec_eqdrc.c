/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#include <stdio.h>
#include <stdint.h>
#include <xutils/debug.h>

int auo_eq_set(uint32_t *eq_filt_coef, int bytes)
{
    int m;
    int filt_cnt;

    filt_cnt = (bytes/sizeof(uint32_t))/5;
    if ((bytes%5) || (filt_cnt<1) || (filt_cnt > 10)) {
        return -1;
    }

    MSP_LOGI("filt_cnt = %d\r\n", filt_cnt);

    m = 0;
    while (1) {
        if (m >= 10) {
            break;
        }

        // disable left
        (*((volatile uint32_t *)(0x20055200 + 20*m))) = 0x100000;
        // disable right
        (*((volatile uint32_t *)(0x200552C8 + 20*m))) = 0x100000;

        if (m < filt_cnt) {
            // update left
            (*((volatile uint32_t *)(0x20055200 + 20*m + 4*4))) = eq_filt_coef[m*5 + 4];
            (*((volatile uint32_t *)(0x20055200 + 20*m + 3*4))) = eq_filt_coef[m*5 + 3];
            (*((volatile uint32_t *)(0x20055200 + 20*m + 2*4))) = eq_filt_coef[m*5 + 2];
            (*((volatile uint32_t *)(0x20055200 + 20*m + 1*4))) = eq_filt_coef[m*5 + 1];
            (*((volatile uint32_t *)(0x20055200 + 20*m + 0*4))) = eq_filt_coef[m*5 + 0];

            // update right
            (*((volatile uint32_t *)(0x200552C8 + 20*m + 4*4))) = eq_filt_coef[m*5 + 4];
            (*((volatile uint32_t *)(0x200552C8 + 20*m + 3*4))) = eq_filt_coef[m*5 + 3];
            (*((volatile uint32_t *)(0x200552C8 + 20*m + 2*4))) = eq_filt_coef[m*5 + 2];
            (*((volatile uint32_t *)(0x200552C8 + 20*m + 1*4))) = eq_filt_coef[m*5 + 1];
            (*((volatile uint32_t *)(0x200552C8 + 20*m + 0*4))) = eq_filt_coef[m*5 + 0];
        }

        m++;
    }

    return 0;
}

int auo_eq_show(void)
{
    int i;

    for (i = 0; i < 10; i++) {
        MSP_LOGI("left[%d]:0x%X,0x%X,0x%X,0x%X,0x%X\r\n",
                i,
                (uint32_t)(*((volatile uint32_t *)(0x20055200 + 20*i + 4*0))),
                (uint32_t)(*((volatile uint32_t *)(0x20055200 + 20*i + 4*1))),
                (uint32_t)(*((volatile uint32_t *)(0x20055200 + 20*i + 4*2))),
                (uint32_t)(*((volatile uint32_t *)(0x20055200 + 20*i + 4*3))),
                (uint32_t)(*((volatile uint32_t *)(0x20055200 + 20*i + 4*4)))
                );
    }

    for (i = 0; i < 10; i++) {
        MSP_LOGI("right[%d]:0x%X,0x%X,0x%X,0x%X,0x%X\r\n",
                i,
                (uint32_t)(*((volatile uint32_t *)(0x200552C8 + 20*i + 4*0))),
                (uint32_t)(*((volatile uint32_t *)(0x200552C8 + 20*i + 4*1))),
                (uint32_t)(*((volatile uint32_t *)(0x200552C8 + 20*i + 4*2))),
                (uint32_t)(*((volatile uint32_t *)(0x200552C8 + 20*i + 4*3))),
                (uint32_t)(*((volatile uint32_t *)(0x200552C8 + 20*i + 4*4)))
                );
    }

    return 0;
}

int auo_drc_set(uint32_t *drc_filt_coef, int bytes)
{
    if (8 != bytes) {
        return -1;
    }
    *(volatile uint32_t *)0x20055444 = 0;
    *(volatile uint32_t *)0x20055448 = drc_filt_coef[1];
    *(volatile uint32_t *)0x20055444 = drc_filt_coef[0];

    return 0;
}

void auo_eq_set_example(void)
{
    static const uint32_t eq_filt_coef[] = {
        0x1ff86, 0xc00f3, 0x1ff86, 0x3ff0d, 0xe007a,
        0x200ff, 0xc00ac, 0x1fe54, 0x3ff54, 0xe01ac,
        0x20299, 0xc020c, 0x1fb5c, 0x3fdf4, 0xe04a4,
        0x1fdba, 0xc0865, 0x1f9e8, 0x3f79b, 0xe0618,
        0x2058f, 0xc05ef, 0x1f493, 0x3fa11, 0xe0b6d,
    };

    auo_eq_set((uint32_t *)eq_filt_coef, sizeof(eq_filt_coef));
}
