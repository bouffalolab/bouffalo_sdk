#include "bflb_mtimer.h"
#include "board.h"

#define DBG_TAG "MAIN"
#include "log.h"

#define main       riscv_dsp2_fir_q15
#define ref_result ref_result_riscv_dsp2_fir_q15
#define result     result_riscv_dsp2_fir_q15
#define src        src_riscv_dsp2_fir_q15
#include "cases/riscv_dsp2_fir_q15.c"
#undef main
#undef ref_result
#undef result
#undef src
#undef TEST_SIZE

#define main       riscv_dsp2_matrix_q31
#define ref_result ref_result_riscv_dsp2_matrix_q31
#define result     result_riscv_dsp2_matrix_q31
#define src        src_riscv_dsp2_matrix_q31
#include "cases/riscv_dsp2_matrix_q31.c"
#undef main
#undef ref_result
#undef result
#undef src
#undef TEST_SIZE

#define main       riscv_dsp2_rfft_q15
#define ref_result ref_result_riscv_dsp2_rfft_q15
#define result     result_riscv_dsp2_rfft_q15
#define src        src_riscv_dsp2_rfft_q15
#include "cases/riscv_dsp2_rfft_q15.c"
#undef main
#undef ref_result
#undef result
#undef src
#undef TEST_SIZE

#define main       riscv_dsp2_sin_q31
#define ref_result ref_result_riscv_dsp2_sin_q31
#define result     result_riscv_dsp2_sin_q31
#define src        src_riscv_dsp2_sin_q31
#include "cases/riscv_dsp2_sin_q31.c"
#undef main
#undef ref_result
#undef result
#undef src
#undef TEST_SIZE

#define main       riscv_dsp2_variance_q15
#define ref_result ref_result_riscv_dsp2_variance_q15
#define result     result_riscv_dsp2_variance_q15
#define src        src_riscv_dsp2_variance_q15
#include "cases/riscv_dsp2_variance_q15.c"
#undef main
#undef ref_result
#undef result
#undef src
#undef TEST_SIZE

int main(void)
{
    board_init();

    printf("Example run riscv_dsp2_fir_q15!\r\n");
    riscv_dsp2_fir_q15();
    printf("Example run riscv_dsp2_matrix_q31!\r\n");
    riscv_dsp2_matrix_q31();
    printf("Example run riscv_dsp2_rfft_q15!\r\n");
    riscv_dsp2_rfft_q15();
    printf("Example run riscv_dsp2_fir_q15!\r\n");
    riscv_dsp2_sin_q31();
    printf("Example run riscv_dsp2_variance_q15!\r\n");
    riscv_dsp2_variance_q15();
    printf("Example all successfully!\r\n");

    while (1) {
        bflb_mtimer_delay_ms(1000);
    }
}
