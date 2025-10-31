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

    bsp_wm8978_write_reg(1, 0X1B);  // Set R1, MICEN to 1 (enable MIC), BIASEN to 1 (enable emulator operation), and VMIDSEL[1:0] to 11 (5K)
    bsp_wm8978_write_reg(2, 0X1B0); // Set R2, enable ROUT1 and LOUT1 outputs (headphones can operate), enable BOOSTENR and BOOST
    bsp_wm8978_write_reg(3, 0X6C);  // In R3, enable LOUT2 and ROUT2 outputs (speaker operation), enable RMIX and LMIX
    // bsp_wm8978_write_reg(6, 0x048);   // In R6, MCLK is provided externally
    bsp_wm8978_write_reg(6, 0);       // In R6, MCLK is provided externally
    // bsp_wm8978_write_reg(6, 0x6D);    // In R6, MCLK is provided externally
    bsp_wm8978_write_reg(43, 1 << 4); // In R43, invert ROUT2 to drive the speaker
    bsp_wm8978_write_reg(47, 1 << 8); // Configure R47: Set PGABOOSTL to provide 20x gain for the left channel MIC
    bsp_wm8978_write_reg(48, 1 << 8); // Configure R48: Set PGABOOSTR to provide 20x gain for the right channel MIC
    bsp_wm8978_write_reg(49, 1 << 1); // In R49, enable TSDEN to turn on thermal shutdown protection
    bsp_wm8978_write_reg(10, 1 << 3); // Configure R10: Disable SOFTMUTE, set 128x oversampling for optimal SNR
    bsp_wm8978_write_reg(14, 1 << 3); // Configure R14: Set ADC to 128x oversampling rate
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

    bsp_wm8978_reg_tbl[reg] = val; // Save register values to local storage

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
    regval = bsp_wm8978_read_reg(3); //Read R3
    if (dacen) {
        regval |= 3 << 0; //The lowest 2 bits of R3 are set to 1 to enable DACR and DACL.
    } else {
        regval &= ~(3 << 0); // Clear the lowest 2 bits of R3 to disable DACR & DACL.
    }
    bsp_wm8978_write_reg(3, regval); //Set R3
    regval = bsp_wm8978_read_reg(2); //Read R2
    if (adcen) {
        regval |= 3 << 0; // Set the lowest 2 bits of R2 to 1 to enable ADCR & ADCL.
    } else {
        regval &= ~(3 << 0); // Clear the lowest 2 bits of R2 to disable ADCR & ADCL.
    }
    bsp_wm8978_write_reg(2, regval); //Set R2
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
    regval = bsp_wm8978_read_reg(2); //Read R2
    if (micen) {
        regval |= 3 << 2; // Enable INPPGAENR and INPPGAENL (MIC PGA amplifiers).
    } else {
        regval &= ~(3 << 2); // Disable INPPGAENR and INPPGAENL.
    }
    bsp_wm8978_write_reg(2, regval); //Set R2

    regval = bsp_wm8978_read_reg(44); //Read R44
    if (micen) {
        regval |= 3 << 4 | 3 << 0; //Open LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
    } else {
        regval &= ~(3 << 4 | 3 << 0); //Close LIN2INPPGA,LIP2INPGA,RIN2INPPGA,RIP2INPGA.
    }
    bsp_wm8978_write_reg(44, regval); //Set R44

    if (lineinen) {
        bsp_wm8978_setgain_linein(5); // Set LINE IN gain to 0dB.
    } else {
        bsp_wm8978_setgain_linein(0); //Close LINE IN
    }
    if (auxen)
        bsp_wm8978_setgain_aux(7); // Set AUX gain to 6dB.
    else
        bsp_wm8978_setgain_aux(0); // Disable AUX input.
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
        regval |= 1 << 0; // Enable DAC output.
    }
    if (bpsen) {
        regval |= 1 << 1; // Enable BYPASS mode.
        regval |= 5 << 2; // Set gain to 0dB.
    }
    bsp_wm8978_write_reg(50, regval); //Set R50
    bsp_wm8978_write_reg(51, regval); //Set R51
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
    bsp_wm8978_write_reg(45, gain);          // R45, Left channel PGA configuration.
    bsp_wm8978_write_reg(46, gain | 1 << 8); // R46, Right channel PGA configuration.
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
    regval = bsp_wm8978_read_reg(47);             //Read R47
    regval &= ~(7 << 4);                          // Clear previous configurations.
    bsp_wm8978_write_reg(47, regval | gain << 4); //Set R47
    regval = bsp_wm8978_read_reg(48);             //Read R48
    regval &= ~(7 << 4);                          // Clear previous configurations.
    bsp_wm8978_write_reg(48, regval | gain << 4); //Set R48
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
    regval = bsp_wm8978_read_reg(47);             //Read R47
    regval &= ~(7 << 0);                          // Clear previous configurations.
    bsp_wm8978_write_reg(47, regval | gain << 0); //Set R47
    regval = bsp_wm8978_read_reg(48);             //Read R48
    regval &= ~(7 << 0);                          // Clear previous configurations.
    bsp_wm8978_write_reg(48, regval | gain << 0); //Set R48
}

/*****************************************************************************
* @brief        config i2s format ang bit
*
* @param[in]    fmt         0: Right-justified LSB, 1: Left-justified MSB, 2: Philips standard, 3: PCM/DSP
* @param[in]    bit         0:16bit 1:20bit 2:24bit 3:32bit
* 
*****************************************************************************/
void bsp_wm8978_config_i2s(uint8_t fmt, uint8_t bit)
{
    fmt &= 0X03;
    bit &= 0X03;                                      // Limit the range.
    bsp_wm8978_write_reg(4, (fmt << 3) | (bit << 5)); // R4, WM8978 operating mode configuration.
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
    volr &= 0X3F; // Limit the range.
    if (voll == 0)
        voll |= 1 << 6; // Mute directly when volume is 0.
    if (volr == 0)
        volr |= 1 << 6;                        // Mute directly when volume is 0.
    bsp_wm8978_write_reg(52, voll);            // R52, Headphone left channel volume setting.
    bsp_wm8978_write_reg(53, volr | (1 << 8)); // R53, Headphone right channel volume setting, update synchronously (HPVU=1).
}

/*****************************************************************************
* @brief        config speaker
* 
* @param[in]    volx        [0:63]
* 
*****************************************************************************/
void bsp_wm8978_setvol_speaker(uint8_t volx)
{
    volx &= 0X3F; // Limit the range.
    if (volx == 0)
        volx |= 1 << 6;                        // Mute directly when volume is 0.
    bsp_wm8978_write_reg(54, volx);            // R54, Speaker left channel volume setting.
    bsp_wm8978_write_reg(55, volx | (1 << 8)); // R55, Speaker right channel volume setting, update synchronously (SPKVU=1).
}

/*****************************************************************************
* @brief        config 3d effact
* 
* @param[in]    depth       [0:15]
* 
*****************************************************************************/
void bsp_wm8978_config_3d(uint8_t depth)
{
    depth &= 0XF;                    // Limit the range.
    bsp_wm8978_write_reg(41, depth); // R41, 3D surround configuration.
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
    bsp_wm8978_write_reg(18, regval); // R18, Bit 9 of EQ1 controls EQ/3D direction.
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
    cfreq &= 0X3; // Limit the range.
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval = bsp_wm8978_read_reg(18);
    regval &= 0X100;
    regval |= cfreq << 5;             // Set cutoff frequency.
    regval |= gain;                   // Set gain.
    bsp_wm8978_write_reg(18, regval); // R18, EQ1 configuration.
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
    cfreq &= 0X3; // Limit the range.
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             // Set cutoff frequency.
    regval |= gain;                   // Set gain.
    bsp_wm8978_write_reg(19, regval); // R19, EQ2 configuration.
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
    cfreq &= 0X3; // Limit the range.
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             // Set cutoff frequency.
    regval |= gain;                   // Set gain.
    bsp_wm8978_write_reg(20, regval); // R20, EQ3 configuration.
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
    cfreq &= 0X3; // Limit the range.
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             // Set cutoff frequency.
    regval |= gain;                   // Set gain.
    bsp_wm8978_write_reg(21, regval); //R21,EQ4 configuration.
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
    cfreq &= 0X3; // Limit the range.
    if (gain > 24)
        gain = 24;
    gain = 24 - gain;
    regval |= cfreq << 5;             // Set cutoff frequency.
    regval |= gain;                   // Set gain.
    bsp_wm8978_write_reg(22, regval); //R22,EQ5 configuration.
}
