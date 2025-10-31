/*
 * BSD 3-Clause License
 *
 * Copyright (c) 2025, the respective contributors, as shown by the AUTHORS file.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
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
 */

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////
static int8_t g_encodingTable[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/'};

static int8_t g_decodingTable[256];

static void build_decoding_table() {
  static int32_t signal = 0;
  int32_t i = 0;

  if (signal != 0) {
    return;
  }

  for (i = 0; i < 64; i++) {
    g_decodingTable[(uint8_t)g_encodingTable[i]] = i;
  }

  signal = 1;
  return;
}

int base64decode(const uint8_t *inputData, uint32_t inputLength,
                 uint32_t outputLenMax, uint8_t *decodedData,
                 uint32_t *outputLength) {
  uint32_t i = 0;
  uint32_t j = 0;

  build_decoding_table();

  if (inputLength % 4 != 0) {
    return -1;
  }

  *outputLength = inputLength / 4 * 3;

  if (inputData[inputLength - 1] == '=') {
    (*outputLength)--;
  }

  if (inputData[inputLength - 2] == '=') {
    (*outputLength)--;
  }

  if (outputLenMax < *outputLength) {
    return -1;
  }

  uint32_t sextet_a = 0;
  uint32_t sextet_b = 0;
  uint32_t sextet_c = 0;
  uint32_t sextet_d = 0;
  uint32_t triple = 0;

  for (i = 0, j = 0; i < inputLength;) {
    sextet_a = inputData[i] == '=' ? 0 & i++ : g_decodingTable[inputData[i++]];
    sextet_b = inputData[i] == '=' ? 0 & i++ : g_decodingTable[inputData[i++]];
    sextet_c = inputData[i] == '=' ? 0 & i++ : g_decodingTable[inputData[i++]];
    sextet_d = inputData[i] == '=' ? 0 & i++ : g_decodingTable[inputData[i++]];

    triple = (sextet_a << 3 * 6) + (sextet_b << 2 * 6) + (sextet_c << 1 * 6) +
             (sextet_d << 0 * 6);

    if (j < *outputLength) {
      decodedData[j++] = (triple >> 2 * 8) & 0xFF;
    }

    if (j < *outputLength) {
      decodedData[j++] = (triple >> 1 * 8) & 0xFF;
    }

    if (j < *outputLength) {
      decodedData[j++] = (triple >> 0 * 8) & 0xFF;
    }
  }

  return 0;
}
