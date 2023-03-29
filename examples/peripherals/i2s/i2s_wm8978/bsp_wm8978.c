/**
 * @file bsp_wm8978.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */

#include "bflb_i2c.h"
#include "bsp_wm8978.h"

#define WM8978_ADDR 0X1A

static struct bflb_device_s *wm8978_i2c = NULL;

static uint16_t bsp_wm8978_reg_tbl[58] = {
    0x0000, 0x0000, 0x0000, 0x0000, 0x0050, 0x0000, 0x0140, 0x0000,
    0x0000, 0x0000, 0x0000, 0x00FF, 0x00FF, 0x0000, 0x0100, 0x00FF,
    0x00FF, 0x0000, 0x012C, 0x002C, 0x002C, 0x002C, 0x002C, 0x0000,
    0x0032, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0038, 0x000B, 0x0032, 0x0000, 0x0008, 0x000C, 0x0093, 0x00E9,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0003, 0x0010, 0x0010, 0x0100,
    0x0100, 0x0002, 0x0001, 0x0001, 0x0039, 0x0039, 0x0039, 0x0039,
    0x0001, 0x0001
};

/*****************************************************************************
* @brief        init wm8978
* 
* @param[in]    i2c         i2c device handler
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int bsp_wm8978_init(struct bflb_device_s *i2c)
{
    if (i2c == NULL) {
        return -1;
    }

    wm8978_i2c = i2c;

    /*!< reset wm8978 */
    if (bsp_wm8978_write_reg(0, 0)) {
        return -1;
    }

    bsp_wm8978_write_reg(1, 0X1B);  //R1,MICEN设置为1(MIC使能),BIASEN设置为1(模拟器工作),VMIDSEL[1:0]设置为:11(5K)
    bsp_wm8978_write_reg(2, 0X1B0); //R2,ROUT1,LOUT1输出使能(耳机可以工作),BOOSTENR,BOOSTENL使能
    bsp_wm8978_write_reg(3, 0X6C);  //R3,LOUT2,ROUT2输出使能(喇叭工作),RMIX,LMIX使能
    // bsp_wm8978_write_reg(6, 0x048);   //R6,MCLK由外部提供
    bsp_wm8978_write_reg(6, 0);       //R6,MCLK由外部提供
    // bsp_wm8978_write_reg(6, 0x6D);    //R6,MCLK由外部提供
    bsp_wm8978_write_reg(43, 1 << 4); //R43,INVROUT2反向,驱动喇叭
    bsp_wm8978_write_reg(47, 1 << 8); //R47设置,PGABOOSTL,左通道MIC获得20倍增益
    bsp_wm8978_write_reg(48, 1 << 8); //R48设置,PGABOOSTR,右通道MIC获得20倍增益
    bsp_wm8978_write_reg(49, 1 << 1); //R49,TSDEN,开启过热保护
    bsp_wm8978_write_reg(10, 1 << 3); //R10,SOFTMUTE关闭,128x采样,最佳SNR
    bsp_wm8978_write_reg(14, 1 << 3); //R14,ADC 128x采样率
    return 0;
}

/*****************************************************************************
* @brief        write wm8978 reg
* 
* @param[in]    reg         reg addr
* @param[in]    val         reg data (7bit addr, 9bit data)
* 
* @retval int               0:Success -1:Error
*****************************************************************************/
int bsp_wm8978_write_reg(uint8_t reg, uint16_t val)
{
    struct bflb_i2c_msg_s msgs[2];

    uint8_t reg_addr = (reg << 1) | ((val >> 8) & 0x01);
    uint8_t reg_data = val & 0xFF;

    msgs[0].addr = WM8978_ADDR;
    msgs[0].flags = I2C_M_NOSTOP;
    msgs[0].buffer = &reg_addr;
    msgs[0].length = 1;

    msgs[1].flags = 0;
    msgs[1].buffer = &reg_data;
    msgs[1].length = 1;

    bsp_wm8978_reg_tbl[reg] = val; //保存寄存器值到本地

    bflb_mtimer_delay_ms(1);
    return bflb_i2c_transfer(wm8978_i2c, msgs, 2);
}

/*****************************************************************************
* @brief        read wm8978 reg
* 
* @param[in]    reg         reg addr
* 
* @retval uint16_t          reg value
*****************************************************************************/
uint16_t bsp_wm8978_read_reg(uint8_t reg)
{
    return bsp_wm8978_reg_tbl[reg];
}

/*****************************************************************************
* @brief        enable adc and dac
* 
* @param[in]    dacen       
* @param[in]    adcen       
* 
*****************************************************************************/
void bsp_wm8978_config_adda(uint8_t dacen, uint8_t adcen)
{
    uint16_t regval;
    regval = bsp_wm8978_read_reg(3); //读取R3
    if (dacen) {
        regval |= 3 << 0; //R3最低2个位设置为1,开启DACR&DACL
    } else {
        regval &= ~(3 << 0); //R3最低2个位清零,关闭DACR&DACL.
    }
    bsp_wm8978_write_reg(3, regval); //设置R3
    regval = bsp_wm8978_read_reg(2); //读取R2
    if (adcen) {
        regval |= 3 << 0; //R2最低2个位设置为1,开启ADCR&ADCL
    } else {
        regval &= ~(3 << 0); //R2最低2个位清零,关闭ADCR&ADCL.
    }
    bsp_wm8978_write_reg(2, regval); //设置R2
}

/*****************************************************************************
* @brief        enable mic linein and aux 
* 
* @param[in]    micen       
* @param[in]    lineinen    
* @param[in]    auxen       
* 
*****************************************************************************/
void bsp_wm8978_config_input(uint8_t micen, uint8_t lineinen, uint8_t auxen)
{
    uint16_t regval;
    regval = bsp_wm8978_read_reg(2); //读取R2
    if (micen) {
        regval |= 3 << 2; //开启INPPGAENR,INPPGAENL(MIC的PGA放大)
    } else {
        regval &= ~(3 << 2); //关闭INPPGAENR,INPPGAENL.
    }
    bsp_wm8978_write_reg(2, regval); //设置R2

    regval = bsp_wm8978_read_reg(44); //读取R44
    if (micen) {
        regval |= 3 << 4 | 3 << 0; //开启LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
    } else {
        regval &= ~(3 << 4 | 3 << 0); //关闭LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
    }
    bsp_wm8978_write_reg(44, regval); //设置R44

    if (lineinen) {
        bsp_wm8978_setgain_linein(5); //LINE IN 0dB增益
    } else {
        bsp_wm8978_setgain_linein(0); //关闭LINE IN
    }
    if (auxen)
        bsp_wm8978_setgain_aux(7); //AUX 6dB增益
    else
        bsp_wm8978_setgain_aux(0); //关闭AUX输入
}

/*****************************************************************************
* @brief        enable dac and bypass(record, mic linein aux)
* 
* @param[in]    dacen       
* @param[in]    bpsen       
* 
*****************************************************************************/
void bsp_wm8978_config_output(uint8_t dacen, uint8_t bpsen)
{
    uint16_t regval = 0;
    if (dacen) {
        regval |= 1 << 0; //DAC输出使能
    }
    if (bpsen) {
        regval |= 1 << 1; //BYPASS使能
        regval |= 5 << 2; //0dB增益
    }
    bsp_wm8978_write_reg(50, regval); //R50设置
    bsp_wm8978_write_reg(51, regval); //R51设置
}

/*****************************************************************************
* @brief        config mic gain (mic adc)
* 
* @param[in]    gain        [0:63] -> -12dB ~ 35.25dB, 0.75dB/step
* 
*****************************************************************************/
void bsp_wm8978_setgain_mic(uint8_t gain)
{
    gain &= 0X3F;
    bsp_wm8978_write_reg(45, gain);          //R45,左通道PGA设置
    bsp_wm8978_write_reg(46, gain | 1 << 8); //R46,右通道PGA设置
}

/*****************************************************************************
* @brief        config linein gain (L2/R2 adc)
* 
* @param[in]    gain        0:disable, [1:7] -> -12dB~6dB, 3dB/step
* 
*****************************************************************************/
void bsp_wm8978_setgain_linein(uint8_t gain)
{
    uint16_t regval;
    gain &= 0X07;
    regval = bsp_wm8978_read_reg(47);             //读取R47
    regval &= ~(7 << 4);                          //清除原来的设置
    bsp_wm8978_write_reg(47, regval | gain << 4); //设置R47
    regval = bsp_wm8978_read_reg(48);             //读取R48
    regval &= ~(7 << 4);                          //清除原来的设置
    bsp_wm8978_write_reg(48, regval | gain << 4); //设置R48
}

/*****************************************************************************
* @brief        config aux gain (auxR/auxL adc)
* 
* @param[in]    gain        0:disable, [1:7] -> -12dB~6dB, 3dB/step
* 
*****************************************************************************/
void bsp_wm8978_setgain_aux(uint8_t gain)
{
    uint16_t regval;
    gain &= 0X07;
    regval = bsp_wm8978_read_reg(47);             //读取R47
    regval &= ~(7 << 0);                          //清除原来的设置
    bsp_wm8978_write_reg(47, regval | gain << 0); //设置R47
    regval = bsp_wm8978_read_reg(48);             //读取R48
    regval &= ~(7 << 0);                          //清除原来的设置
    bsp_wm8978_write_reg(48, regval | gain << 0); //设置R48
}

/*****************************************************************************
* @brief        config i2s format ang bit
* 
* @param[in]    fmt         0:右对齐 LSB 1:左对齐 MSB 2:飞利浦标准 3:PCM/DSP
* @param[in]    bit         0:16bit 1:20bit 2:24bit 3:32bit
* 
*****************************************************************************/
void bsp_wm8978_config_i2s(uint8_t fmt, uint8_t bit)
{
    fmt &= 0X03;
    bit &= 0X03;                                      //限定范围
    bsp_wm8978_write_reg(4, (fmt << 3) | (bit << 5)); //R4,WM8978工作模式设置
}

/*****************************************************************************
* @brief        config headset volume
* 
* @param[in]    voll        [0:63]
* @param[in]    volr        [0:63]
* 
*****************************************************************************/
void bsp_wm8978_setvol_headset(uint8_t voll, uint8_t volr)
{
    voll &= 0X3F;
    volr &= 0X3F; //限定范围
    if (voll == 0)
        voll |= 1 << 6; //音量为0时,直接mute
    if (volr == 0)
        volr |= 1 << 6;                        //音量为0时,直接mute
    bsp_wm8978_write_reg(52, voll);            //R52,耳机左声道音量设置
    bsp_wm8978_write_reg(53, volr | (1 << 8)); //R53,耳机右声道音量设置,同步更新(HPVU=1)
}

/*****************************************************************************
* @brief        config speaker
* 
* @param[in]    volx        [0:63]
* 
*****************************************************************************/
void bsp_wm8978_setvol_speaker(uint8_t volx)
{
    volx &= 0X3F; //限定范围
    if (volx == 0)
        volx |= 1 << 6;                        //音量为0时,直接mute
    bsp_wm8978_write_reg(54, volx);            //R54,喇叭左声道音量设置
    bsp_wm8978_write_reg(55, volx | (1 << 8)); //R55,喇叭右声道音量设置,同步更新(SPKVU=1)
}

/*****************************************************************************
* @brief        config 3d effact
* 
* @param[in]    depth       [0:15]
* 
*****************************************************************************/
void bsp_wm8978_config_3d(uint8_t depth)
{
    depth &= 0XF;                    //限定范围
    bsp_wm8978_write_reg(41, depth); //R41,3D环绕设置
}

/*****************************************************************************
* @brief        confgi eq 3d
* 
* @param[in]    dir         0:adc 1:dac (default)
* 
*****************************************************************************/
void bsp_wm8978_config_3deq(uint8_t dir)
{
    uint16_t regval;
    regval = bsp_wm8978_read_reg(0X12);
    if (dir)
        regval |= 1 << 8;
    else
        regval &= ~(1 << 8);
    bsp_wm8978_write_reg(18, regval); //R18,EQ1的第9位控制EQ/3D方向
}

/*****************************************************************************
* @brief        config eq 1
* 
* @param[in]    cfreq      [0:3] 80/105/135/175Hz
* @param[in]    gain       [0:24] -12dB~12dB
* 
*****************************************************************************/
void bsp_wm8978_config_eq1(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval = bsp_wm8978_read_reg(18);
    regval &= 0X100;
    regval |= cfreq << 5;             //设置截止频率
    regval |= gain;                   //设置增益
    bsp_wm8978_write_reg(18, regval); //R18,EQ1设置
}

/*****************************************************************************
* @brief        config eq 2
* 
* @param[in]    cfreq      [0:3] 230/300/385/500Hz
* @param[in]    gain       [0:24] -12dB~12dB
* 
*****************************************************************************/
void bsp_wm8978_config_eq2(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             //设置截止频率
    regval |= gain;                   //设置增益
    bsp_wm8978_write_reg(19, regval); //R19,EQ2设置
}

/*****************************************************************************
* @brief        config eq 3
* 
* @param[in]    cfreq      [0:3] 650/850/1100/1400Hz
* @param[in]    gain       [0:24] -12dB~12dB
* 
*****************************************************************************/
void bsp_wm8978_config_eq3(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             //设置截止频率
    regval |= gain;                   //设置增益
    bsp_wm8978_write_reg(20, regval); //R20,EQ3设置
}

/*****************************************************************************
* @brief        config eq 4
* 
* @param[in]    cfreq      [0:3] 1800/2400/3200/4100Hz
* @param[in]    gain       [0:24] -12dB~12dB
* 
*****************************************************************************/
void bsp_wm8978_config_eq4(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             //设置截止频率
    regval |= gain;                   //设置增益
    bsp_wm8978_write_reg(21, regval); //R21,EQ4设置
}

/*****************************************************************************
* @brief        config eq 5
* 
* @param[in]    cfreq      [0:3] 5300/6900/9000/11700Hz
* @param[in]    gain       [0:24] -12dB~12dB
* 
*****************************************************************************/
void bsp_wm8978_config_eq5(uint8_t cfreq, uint8_t gain)
{
    uint16_t regval = 0;
    cfreq &= 0X3; //限定范围
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             //设置截止频率
    regval |= gain;                   //设置增益
    bsp_wm8978_write_reg(22, regval); //R22,EQ5设置
}
