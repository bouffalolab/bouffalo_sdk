/* output format: UYVY */
struct bflb_pec_qspi_cam_s cam_cfg_bf20a6 = {
    .mem = 0,                                /*!< memory address of first instruction */
    .div = 0,                                /*!< divisor, N = div + 1 */
    .dma_enable = true,                      /*!< enable or disable dma */
    .fifo_threshold = 8 - 1,                 /*!< tx fifo threshold */
    .pixel_bits = 16,                        /*!< clock count of every pixel */
    .pin_pclk = PEC_QSPI_CAM_PCLK_PIN,       /*!< QSPI CAM PCLK pin index */
    .pin_d0 = PEC_QSPI_CAM_D0_PIN,           /*!< QSPI CAM D0 pin index */
    .pin_data_count = 4,                     /*!< QSPI CAM data pin count, must be power of 2, such as 1, 2, 4, 8 */
    .pin_pclk_idle_level = 1,                /*!< QSPI CAM pixel clock level when idle, 0: low level, others: high level */
    .sample_edge = 1,                        /*!< QSPI CAM data sample after clock pin edge, 0: sample after falling edge, others: sample after rising edge */
    .fifo_direction = PEC_SHIFT_DIR_TO_LEFT, /*!< FIFO direction, PEC_SHIFT_DIR_TO_LEFT or PEC_SHIFT_DIR_TO_RIGHT */
    .bits_every_push = 16,                   /*!< bits of every push when sample data into ISR, 1~32, must be multiple of pin_data_count */
    .delay_first_us = 200000,                /*!< delay time in microsecond before start capature camera data */
    .det_frame_ns = 1000 * 1000,             /*!< time in nanosecond for inter-frame space detect */
    .det_line_ns = 10 * 1000,                /*!< time in nanosecond for inter-line space detect */
    .skip_clk_line = 8 - 1,                  /*!< skip clock count before every line */
    .skip_clk_first_line = 8 - 1,            /*!< skip clock count before first line */
    .skip_dly = 0,                           /*! delay count in every clock skip */
    .sample_dly = 0,                         /*!< QSPI CAM data sample delay time after clock pin edge, in unit of PEC clock */
};
