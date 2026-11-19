#ifndef WASM_CMD_COREMARK_BENCH_H
#define WASM_CMD_COREMARK_BENCH_H

#include <stdint.h>

uint32_t coremark_prepare(uint32_t iterations);
uint32_t coremark_execute(void);
uint32_t coremark_get_crc(void);
uint32_t coremark_get_list_crc(void);
uint32_t coremark_get_matrix_crc(void);
uint32_t coremark_get_state_crc(void);
uint32_t coremark_get_error(void);

#endif
