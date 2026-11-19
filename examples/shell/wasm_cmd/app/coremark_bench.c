#include "coremark.h"
#include "coremark_bench.h"

#if defined(__wasm__)
#define COREMARK_EXPORT(name) __attribute__((export_name(name), used))
#else
#define COREMARK_EXPORT(name)
#endif

#define COREMARK_SEED1           0
#define COREMARK_SEED2           0
#define COREMARK_SEED3           0x66
#define COREMARK_CRC_LIST        0xe714
#define COREMARK_CRC_MATRIX      0x1fd7
#define COREMARK_CRC_STATE       0x8e3a

static core_results cm_result;
static ee_u8 cm_memblock[TOTAL_DATA_SIZE] __attribute__((aligned(8)));
static uint32_t cm_prepared;
static uint32_t cm_error;

static void cm_zero_result(void)
{
    ee_u8 *p = (ee_u8 *)&cm_result;
    ee_u32 i;

    for (i = 0; i < sizeof(cm_result); i++) {
        p[i] = 0;
    }
}

COREMARK_EXPORT("coremark_prepare")
uint32_t coremark_prepare(uint32_t iterations)
{
    ee_u32 seed;

    cm_prepared = 0;
    cm_error = check_data_types() ? 1u << 3 : 0;
    if (iterations == 0) {
        cm_error |= 1u << 4;
        return 1;
    }

    cm_zero_result();
    cm_result.seed1 = COREMARK_SEED1;
    cm_result.seed2 = COREMARK_SEED2;
    cm_result.seed3 = COREMARK_SEED3;
    cm_result.iterations = iterations;
    cm_result.execs = ALL_ALGORITHMS_MASK;
    cm_result.memblock[0] = cm_memblock;
    cm_result.size = sizeof(cm_memblock) / NUM_ALGORITHMS;

    cm_result.memblock[1] = cm_memblock;
    cm_result.memblock[2] = cm_memblock + cm_result.size;
    cm_result.memblock[3] = cm_memblock + cm_result.size * 2;

    cm_result.list =
        core_list_init(cm_result.size, (list_head *)cm_result.memblock[1],
                       cm_result.seed1);
    seed = (ee_s32)cm_result.seed1 | (((ee_s32)cm_result.seed2) << 16);
    core_init_matrix(cm_result.size, cm_result.memblock[2], seed, &cm_result.mat);
    core_init_state(cm_result.size, cm_result.seed1, cm_result.memblock[3]);

    cm_prepared = 1;
    return 0;
}

COREMARK_EXPORT("coremark_execute")
uint32_t coremark_execute(void)
{
    ee_u32 i;

    if (!cm_prepared) {
        cm_error |= 1u << 5;
        return 1;
    }

    cm_result.crc = 0;
    cm_result.crclist = 0;
    cm_result.crcmatrix = 0;
    cm_result.crcstate = 0;

    for (i = 0; i < cm_result.iterations; i++) {
        ee_u16 crc = core_bench_list(&cm_result, 1);
        cm_result.crc = crcu16(crc, cm_result.crc);
        crc = core_bench_list(&cm_result, -1);
        cm_result.crc = crcu16(crc, cm_result.crc);
        if (i == 0) {
            cm_result.crclist = cm_result.crc;
        }
    }

    if (cm_result.crclist != COREMARK_CRC_LIST) {
        cm_error |= 1u << 0;
    }
    if (cm_result.crcmatrix != COREMARK_CRC_MATRIX) {
        cm_error |= 1u << 1;
    }
    if (cm_result.crcstate != COREMARK_CRC_STATE) {
        cm_error |= 1u << 2;
    }

    cm_prepared = 0;
    return cm_error ? 1 : 0;
}

COREMARK_EXPORT("coremark_get_crc")
uint32_t coremark_get_crc(void)
{
    return cm_result.crc;
}

COREMARK_EXPORT("coremark_get_list_crc")
uint32_t coremark_get_list_crc(void)
{
    return cm_result.crclist;
}

COREMARK_EXPORT("coremark_get_matrix_crc")
uint32_t coremark_get_matrix_crc(void)
{
    return cm_result.crcmatrix;
}

COREMARK_EXPORT("coremark_get_state_crc")
uint32_t coremark_get_state_crc(void)
{
    return cm_result.crcstate;
}

COREMARK_EXPORT("coremark_get_error")
uint32_t coremark_get_error(void)
{
    return cm_error;
}
