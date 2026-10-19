#include <string.h>

#include "base64.h"

static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int libpeer_base64_encode(const unsigned char* input, int input_len, char* output, int output_len) {
  int i, j;
  unsigned char buf[3];
  int buf_len;
  int output_index = 0;

  if (input == NULL || input_len < 0 || output == NULL || output_len <= 0 ||
      input_len > (output_len - 1) / 4 * 3 + 2) {
    return -1;
  }

  for (i = 0; i < input_len; i += 3) {
    buf_len = 0;
    for (j = 0; j < 3; j++) {
      if (i + j < input_len) {
        buf[j] = input[i + j];
        buf_len++;
      } else {
        buf[j] = 0;
      }
    }

    if (output_index + 4 >= output_len) {
      output[0] = '\0';
      return -1;
    }

    output[output_index++] = base64_table[(buf[0] & 0xFC) >> 2];
    output[output_index++] = base64_table[((buf[0] & 0x03) << 4) | ((buf[1] & 0xF0) >> 4)];
    output[output_index++] = (buf_len > 1) ? base64_table[((buf[1] & 0x0F) << 2) | ((buf[2] & 0xC0) >> 6)] : '=';
    output[output_index++] = (buf_len > 2) ? base64_table[buf[2] & 0x3F] : '=';
  }

  output[output_index] = '\0';
  return output_index;
}

static int libpeer_base64_value(char value) {
  const char* pos = strchr(base64_table, value);
  return pos == NULL ? -1 : (int)(pos - base64_table);
}

int libpeer_base64_decode(const char* input, int input_len, unsigned char* output, int output_len) {
  int i;
  int output_index = 0;

  if (input == NULL || input_len < 0 || output == NULL || output_len < 0 ||
      input_len % 4 != 0) {
    return -1;
  }

  for (i = 0; i < input_len; i += 4) {
    int values[4];
    int decoded_len = 3;
    int is_last = i + 4 == input_len;

    values[0] = libpeer_base64_value(input[i]);
    values[1] = libpeer_base64_value(input[i + 1]);
    values[2] = input[i + 2] == '=' ? 0 : libpeer_base64_value(input[i + 2]);
    values[3] = input[i + 3] == '=' ? 0 : libpeer_base64_value(input[i + 3]);

    if (values[0] < 0 || values[1] < 0 || values[2] < 0 || values[3] < 0) {
      return -1;
    }
    if (input[i + 2] == '=') {
      if (!is_last || input[i + 3] != '=') {
        return -1;
      }
      decoded_len = 1;
    } else if (input[i + 3] == '=') {
      if (!is_last) {
        return -1;
      }
      decoded_len = 2;
    }

    if (output_index + decoded_len > output_len) {
      return -1;
    }

    output[output_index++] = (unsigned char)((values[0] << 2) | (values[1] >> 4));
    if (decoded_len > 1) {
      output[output_index++] = (unsigned char)((values[1] << 4) | (values[2] >> 2));
    }
    if (decoded_len > 2) {
      output[output_index++] = (unsigned char)((values[2] << 6) | values[3]);
    }
  }

  return output_index;
}
