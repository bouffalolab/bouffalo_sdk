#ifndef LIBPEER_BASE64_H_
#define LIBPEER_BASE64_H_

int libpeer_base64_encode(const unsigned char* input, int input_len, char* output, int output_len);

int libpeer_base64_decode(const char* input, int input_len, unsigned char* output, int output_len);

#endif  // LIBPEER_BASE64_H_
