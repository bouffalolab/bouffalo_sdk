/**
  ******************************************************************************
  * @file    efuse_slot.c
  * @version V1.0
  * @date
  * @brief   This file is the peripheral case c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifdef BL602
#include <stdbool.h>
#include <bl602_ef_ctrl.h>
#include <bl602_sec_eng.h>

bool efuse_slot_decrypt(uint8_t *p, uint32_t len, uint32_t *pIv)
{
    SEC_Eng_AES_Ctx aesCtx;
    uint8_t keySel = 1;

    if (0 == len) {
        return false;
    }

    Sec_Eng_AES_Enable_BE(SEC_ENG_AES_ID0);

    Sec_Eng_AES_Init(&aesCtx, SEC_ENG_AES_ID0, SEC_ENG_AES_CBC, SEC_ENG_AES_KEY_128BITS, SEC_ENG_AES_DECRYPTION);
    Sec_Eng_AES_Set_Key_IV_BE(SEC_ENG_AES_ID0, SEC_ENG_AES_KEY_HW, &keySel, (const uint8_t *)pIv);

    Sec_Eng_AES_Crypt(&aesCtx, SEC_ENG_AES_ID0, p, len, p);
    Sec_Eng_AES_Finish(SEC_ENG_AES_ID0);

    return true;
}

#elif defined BL616

#include <stdbool.h>
#include <bl616.h>
#include <bflb_flash.h>
#include <bflb_sec_aes.h>

bool efuse_slot_decrypt(uint8_t *p, uint32_t len, uint32_t *pIv)
{
    struct bflb_device_s *aes;

    aes = bflb_device_get_by_name("aes");
    if (0 == len || NULL == aes) {
        return false;
    }

    bflb_group0_request_aes_access(aes);

    bflb_aes_init(aes);
    bflb_aes_set_mode(aes, AES_MODE_CBC);
    bflb_aes_set_hwkey(1);
    bflb_aes_setkey(aes, NULL, 128);
    bflb_aes_decrypt(aes, p, (uint8_t*)pIv, p, len);

    bflb_aes_deinit(aes);
    bflb_group0_release_aes_access(aes);

    return true;
}

#endif