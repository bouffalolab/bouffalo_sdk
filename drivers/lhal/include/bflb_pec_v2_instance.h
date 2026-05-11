#ifndef __BFLB_PEC_V2_INSTANCE_H__
#define __BFLB_PEC_V2_INSTANCE_H__

#include "bflb_core.h"

/******************************************************************
COMMON MACRO
******************************************************************/
#define PEC_SHIFT_DIR_TO_LEFT    (0) /*!< shift OSR/ISR to left (data enters from right) */
#define PEC_SHIFT_DIR_TO_RIGHT   (1) /*!< shift OSR/ISR to right (data enters from left) */

/******************************************************************
COMMON API
******************************************************************/
int bflb_pec_memory_size_check(struct bflb_device_s *dev, uint8_t start, uint16_t size, uint8_t *actual_start);

/******************************************************************
low level API
******************************************************************/
/* fifo */
uint8_t bflb_pec_fifo_get_deepth_tx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_get_deepth_rx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_get_level_tx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_get_level_rx(struct bflb_device_s *dev);
void bflb_pec_fifo_clr_tx(struct bflb_device_s *dev);
void bflb_pec_fifo_clr_rx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_is_rdy_tx(struct bflb_device_s *dev);
uint8_t bflb_pec_fifo_is_rdy_rx(struct bflb_device_s *dev);
uint32_t bflb_pec_fifo_get_addr_tx(struct bflb_device_s *dev);
uint32_t bflb_pec_fifo_get_addr_rx(struct bflb_device_s *dev);
void bflb_pec_fifo_write(struct bflb_device_s *dev, uint32_t value);
uint32_t bflb_pec_fifo_read(struct bflb_device_s *dev);
uint32_t bflb_pec_fifo_write_multi_32bits(struct bflb_device_s *dev, uint32_t *data, uint32_t len);
uint32_t bflb_pec_fifo_write_multi_16bits(struct bflb_device_s *dev, uint16_t *data, uint32_t len);
uint32_t bflb_pec_fifo_write_multi_8bits(struct bflb_device_s *dev, uint8_t *data, uint32_t len);
void bflb_pec_fifo_write_multi_32bits_blocking(struct bflb_device_s *dev, uint32_t *data, uint32_t len);
void bflb_pec_fifo_write_multi_16bits_blocking(struct bflb_device_s *dev, uint16_t *data, uint32_t len);
void bflb_pec_fifo_write_multi_8bits_blocking(struct bflb_device_s *dev, uint8_t *data, uint32_t len);
uint32_t bflb_pec_fifo_read_multi_32bits(struct bflb_device_s *dev, uint32_t *data, uint32_t len);
uint32_t bflb_pec_fifo_read_multi_16bits(struct bflb_device_s *dev, uint16_t *data, uint32_t len);
uint32_t bflb_pec_fifo_read_multi_8bits(struct bflb_device_s *dev, uint8_t *data, uint32_t len);
void bflb_pec_fifo_set_threshold_tx(struct bflb_device_s *dev, uint8_t threshold);
void bflb_pec_fifo_set_threshold_rx(struct bflb_device_s *dev, uint8_t threshold);
/* dma */
void bflb_pec_dma_enable_tx(struct bflb_device_s *dev);
void bflb_pec_dma_enable_rx(struct bflb_device_s *dev);
void bflb_pec_dma_disable_tx(struct bflb_device_s *dev);
void bflb_pec_dma_disable_rx(struct bflb_device_s *dev);

/******************************************************************
PWM
******************************************************************/
struct bflb_pec_pwm_s {
    uint32_t mem;    /*!< memory address of first instruction */
    uint16_t div;    /*!< divisor, N = div + 1 */
    uint8_t pin;     /*!< PWM output assert pin index */
    uint8_t inverse; /*!< toggle pin level */
};

int bflb_pec_pwm_init(struct bflb_device_s *dev, struct bflb_pec_pwm_s *pwm);
void bflb_pec_pwm_deinit(struct bflb_device_s *dev);
void bflb_pec_pwm_start(struct bflb_device_s *dev, uint32_t high_cnt, uint32_t low_cnt);
void bflb_pec_pwm_stop(struct bflb_device_s *dev);

/******************************************************************
UART_TX
******************************************************************/
#define PEC_UART_STOPBITS_0P5 (0)
#define PEC_UART_STOPBITS_1P0 (1)
#define PEC_UART_STOPBITS_1P5 (2)
#define PEC_UART_STOPBITS_2P0 (3)

#define PEC_UART_PARITY_NONE (0)
#define PEC_UART_PARITY_ODD  (1)
#define PEC_UART_PARITY_EVEN (2)

struct bflb_pec_uart_tx_s {
    uint32_t mem;           /*!< memory address of first instruction */
    uint32_t baudrate;      /*!< uart baudrate */
    uint8_t databits;       /*!< uart data bits */
    uint8_t stopbits;       /*!< uart stop bits */
    uint8_t parity;         /*!< uart parity */
    uint8_t pin;            /*!< pin index of uart */
    uint8_t fifo_threshold; /*!< uart fifo threshold */
    uint8_t dma_enable;     /*!< enable or disable dma with uart */
    uint8_t pin_inverse;    /*!< enable or disable uart_tx output level inverse */
};

int bflb_pec_uart_tx_init(struct bflb_device_s *dev, struct bflb_pec_uart_tx_s *uart);
void bflb_pec_uart_tx_deinit(struct bflb_device_s *dev);

/******************************************************************
UART_RX
******************************************************************/
struct bflb_pec_uart_rx_s {
    uint32_t mem;           /*!< memory address of first instruction */
    uint32_t baudrate;      /*!< uart baudrate */
    uint8_t databits;       /*!< uart data bits */
    uint8_t stopbits;       /*!< uart stop bits, note: hardware don't check stopbits length */
    uint8_t parity;         /*!< uart parity */
    uint8_t pin;            /*!< pin index of uart */
    uint8_t fifo_threshold; /*!< uart fifo threshold */
    uint8_t dma_enable;     /*!< enable or disable dma with uart */
    uint8_t pin_inverse;    /*!< enable or disable uart_rx input level inverse */
    uint16_t rto_bits;      /*!< bits of RTO(receive timeout) */
};

int bflb_pec_uart_rx_init(struct bflb_device_s *dev, struct bflb_pec_uart_rx_s *uart);
void bflb_pec_uart_rx_deinit(struct bflb_device_s *dev);
void bflb_pec_uart_rx_start(struct bflb_device_s *dev);
void bflb_pec_uart_rx_stop(struct bflb_device_s *dev);

/******************************************************************
IR_NEC
******************************************************************/
struct bflb_pec_ir_s {
    uint32_t mem;          /*!< memory address of first instruction */
    uint16_t div;          /*!< ir clock dividor, div = (clk_source / (carrier_low + carrier_high)) */
    uint8_t carrier_low;   /*!< carrier low level time in erery carrier period, max 64 */
    uint8_t carrier_high;  /*!< carrier high level time in erery carrier period, max 64 */
    uint8_t pin;           /*!< pin of ir */
    uint8_t idle_level;    /*!< pin level when idle */
};

struct bflb_pec_ir_nec_timing_s {
    uint16_t data0_carrier; /*!< data_0 carrier length */
    uint16_t data0_idle;    /*!< data_0 idle length */
    uint16_t data1_carrier; /*!< data_1 carrier length */
    uint16_t data1_idle;    /*!< data_1 idle length */
    uint16_t start_carrier; /*!< start signal carrier length */
    uint16_t start_idle;    /*!< start signal idle length */
    uint16_t stop_carrier;  /*!< stop signal carrier length */
    uint16_t stop_idle;     /*!< stop signal idle length */
};

struct bflb_pec_ir_rc5_timing_s {
    uint16_t data0_carrier; /*!< data_0 carrier length */
    uint16_t data0_idle;    /*!< data_0 idle length */
    uint16_t data1_carrier; /*!< data_1 carrier length */
    uint16_t data1_idle;    /*!< data_1 idle length */
};

int bflb_pec_ir_nec_init(struct bflb_device_s *dev, struct bflb_pec_ir_s *ir);
void bflb_pec_ir_nec_set_timing(struct bflb_device_s *dev, struct bflb_pec_ir_nec_timing_s *nec_timing);
void bflb_pec_ir_nec_send(struct bflb_device_s *dev, uint32_t *data, uint32_t bits);
int bflb_pec_ir_nec_is_busy(struct bflb_device_s *dev);
int bflb_pec_ir_rc5_init(struct bflb_device_s *dev, struct bflb_pec_ir_s *ir);
void bflb_pec_ir_rc5_set_timing(struct bflb_device_s *dev, struct bflb_pec_ir_rc5_timing_s *rc5_timing);
void bflb_pec_ir_rc5_send(struct bflb_device_s *dev, uint32_t *data, uint32_t bits);
int bflb_pec_ir_rc5_is_busy(struct bflb_device_s *dev);

/******************************************************************
SPI
******************************************************************/
struct bflb_pec_spi_s {
    uint32_t mem;     /*!< memory address of first instruction */
    uint32_t freq;    /*!< spi clk pin frequency */
    uint8_t cpol;     /*!< clock polarity */
    uint8_t cpha;     /*!< clock phase */
    uint8_t pin_sck;  /*!< pin of spi clock */
    uint8_t pin_mosi; /*!< pin of spi master out slave in */
    uint8_t pin_miso; /*!< pin of spi master in slave out */
};

int bflb_pec_spi_init(struct bflb_device_s *dev, struct bflb_pec_spi_s *spi);
void bflb_pec_spi_set_bits(struct bflb_device_s *dev, uint8_t bits);

/******************************************************************
I2C
******************************************************************/
#define I2C_ERR_START_SDA_LOW (1 << 8)  /* error flag: sda is always low before start signal */
#define I2C_ERR_START_SCL_LOW (1 << 9)  /* error flag: scl is low in start phase */
#define I2C_ERR_TO_WRITE_DATA (1 << 10) /* SCL is low more than timeout when write data */
#define I2C_ERR_TO_WRITE_ACK  (1 << 11) /* SCL is low more than timeout in ack phase when write data */
#define I2C_ERR_TO_READ_DATA  (1 << 12) /* SCL is low more than timeout when read data */
#define I2C_ERR_TO_READ_ACK   (1 << 13) /* SCL is low more than timeout in ack phase when read data */
#define I2C_ERR_NAK           (1 << 14) /* error flag: slave send nak signal */
#define I2C_ERR_ALL           (I2C_ERR_START_SDA_LOW | \
                               I2C_ERR_START_SCL_LOW | \
                               I2C_ERR_TO_WRITE_DATA | \
                               I2C_ERR_TO_WRITE_ACK  | \
                               I2C_ERR_TO_READ_DATA  | \
                               I2C_ERR_TO_READ_ACK   | \
                               I2C_ERR_NAK          )

struct bflb_pec_i2c_s {
    uint32_t mem;    /*!< memory address of first instruction */
    uint8_t pin_scl; /*!< pin of i2c clock */
    uint8_t pin_sda; /*!< pin of i2c data */
};

struct bflb_pec_i2c_timing_s {
    uint32_t clk;        /*!< i2c SCL clock frequency, if this member is zero, auto calculating will not be used */
    uint16_t div;        /*!< clock divisor */
    uint16_t time_high;  /*!< high level time */
    uint16_t time_setup; /*!< setup time */
    uint16_t time_hold;  /*!< hold time */
    uint16_t timeout;    /*!< timeout, unit is clk frequency */
};

struct bflb_pec_i2c_data_s {
    uint8_t *buff;       /*!< i2c data buffer */
    uint16_t length;     /*!< i2c data length */
    uint16_t slave_addr; /*!< i2c slave address */
    uint8_t slave_10bit; /*!< i2c slave address is 10bit */
    uint8_t head_10;     /*!< send 2nd slave address byte or not */
    uint8_t read;        /*!< i2c read or write */
    uint8_t start;       /*!< generate start signal or not berfor frame */
    uint8_t stop;        /*!< generate stop signal or not after all data */
    uint8_t nak;         /*!< generate nak signal or not at last data when read */
};

int bflb_pec_i2c_init(struct bflb_device_s *dev, struct bflb_pec_i2c_s *i2c);
void bflb_pec_i2c_set_timing(struct bflb_device_s *dev, struct bflb_pec_i2c_timing_s *timing);
int bflb_pec_i2c_is_busy(struct bflb_device_s *dev);
uint32_t bflb_pec_i2c_get_err(struct bflb_device_s *dev);
void bflb_pec_i2c_clear_err(struct bflb_device_s *dev);
uint32_t bflb_pec_i2c_write(struct bflb_device_s *dev, struct bflb_pec_i2c_data_s *data);
uint32_t bflb_pec_i2c_read(struct bflb_device_s *dev, struct bflb_pec_i2c_data_s *data);

/******************************************************************
DPI
******************************************************************/
struct bflb_pec_dpi_s {
    uint32_t mem;              /*!< memory address of first instruction */
    uint16_t div;              /*!< divisor, N = div + 1 */
    uint8_t dma_enable;        /*!< enable or disable dma */
    uint8_t fifo_threshold;    /*!< tx fifo threshold */
    uint8_t pin_data_start[3]; /*!< DPI data pin start index, data pin number is continuous in one group */
    uint8_t pin_data_count[3]; /*!< DPI data group pin count */
    uint8_t pin_pclk;          /*!< DPI pixel clock pin */
    uint8_t pin_vsync;         /*!< DPI vsync pin */
    uint8_t pin_hsync;         /*!< DPI hsync pin */
    uint8_t pin_de;            /*!< DPI de pin */
    uint8_t level_pclk;        /*!< DPI pixel clock level, 0: sample on falling edge, others: sample on rising edge */
    uint8_t level_vsync;       /*!< DPI vsync level in vertical synchronization segment */
    uint8_t level_hsync;       /*!< DPI hsync level in horizontal synchronization segment */
    uint8_t level_de;          /*!< DPI de level when data is enabled */
    uint16_t hsync;            /*!< DPI pixel clock count in horizontal synchronization segment, max is 1024 */
    uint16_t hbp;              /*!< DPI pixel clock count in horizontal back porch segment, max is 2048 */
    uint16_t hfp;              /*!< DPI pixel clock count in horizontal front porch segment, max is 2048 */
    uint16_t vsync;            /*!< DPI row count in vertical synchronization segment, max is 1024 */
    uint16_t vbp;              /*!< DPI row count in vertical back porch segment, max is 2048 */
    uint16_t vfp;              /*!< DPI row count in vertical front porch segment, max is 2048 */
    uint16_t width;            /*!< DPI actual picture width, max is 65535 */
    uint16_t height;           /*!< DPI actual picture height, max is 65535 */
};

int bflb_pec_dpi_init(struct bflb_device_s *dev, struct bflb_pec_dpi_s *dpi);
void bflb_pec_dpi_deinit(struct bflb_device_s *dev);

/******************************************************************
sRGB
******************************************************************/
#define PEC_SRGB_FORMAT_RGB565  (0)
#define PEC_SRGB_FORMAT_BGR565  (1)
#define PEC_SRGB_FORMAT_RGB888  (2)
#define PEC_SRGB_FORMAT_BGR888  (3)
#define PEC_SRGB_FORMAT_NRGB888 (4)
#define PEC_SRGB_FORMAT_NBGR888 (5)
#define PEC_SRGB_FORMAT_MAX     (6)

#define PEC_SRGB_CLOCK_80MHZ (0)
#define PEC_SRGB_CLOCK_53MHZ (1)
#define PEC_SRGB_CLOCK_40MHZ (2)
#define PEC_SRGB_CLOCK_32MHZ (3)
#define PEC_SRGB_CLOCK_27MHZ (4)
#define PEC_SRGB_CLOCK_23MHZ (5)
#define PEC_SRGB_CLOCK_20MHZ (6)
#define PEC_SRGB_CLOCK_18MHZ (7)
#define PEC_SRGB_CLOCK_16MHZ (8)
#define PEC_SRGB_CLOCK_15MHZ (9)
#define PEC_SRGB_CLOCK_12MHZ (10)
#define PEC_SRGB_CLOCK_10MHZ (11)
#define PEC_SRGB_CLOCK_8MHZ  (12)
#define PEC_SRGB_CLOCK_5MHZ  (13)
#define PEC_SRGB_CLOCK_4MHZ  (14)
#define PEC_SRGB_CLOCK_2MHZ  (15)

struct bflb_pec_srgb_s {
    uint32_t mem;           /*!< memory address of first instruction */
    uint8_t format;         /*!< sRGB data format, PEC_SRGB_FORMAT_xxx */
    uint8_t clk_freq;       /*!< sRGB clock frequency, PEC_SRGB_CLOCK_xxx */
    uint8_t dma_enable;     /*!< enable or disable dma */
    uint8_t fifo_threshold; /*!< tx fifo threshold */
    uint8_t pin_data_start; /*!< sRGB data pin start index, data pin number must be continuous */
    uint8_t pin_clk;        /*!< sRGB clock pin */
    uint8_t pin_vsync;      /*!< sRGB vsync pin */
    uint8_t pin_hsync;      /*!< sRGB hsync pin */
    uint8_t pin_de;         /*!< sRGB de pin */
    uint8_t level_clk;      /*!< sRGB clock level, 0: sample on falling edge, others: sample on rising edge */
    uint8_t level_vsync;    /*!< sRGB vsync level in vertical synchronization segment */
    uint8_t level_hsync;    /*!< sRGB hsync level in horizontal synchronization segment */
    uint8_t level_de;       /*!< sRGB de level when data is enabled */
    uint16_t hsync;         /*!< sRGB clock count in horizontal synchronization segment, max is 16383 */
    uint16_t hbp;           /*!< sRGB clock count in horizontal back porch segment, max is 16383 */
    uint16_t hfp;           /*!< sRGB clock count in horizontal front porch segment, max is 16383 */
    uint16_t vsync;         /*!< sRGB row count in vertical synchronization segment, max is 65535 */
    uint16_t vbp;           /*!< sRGB row count in vertical back porch segment, max is 65535 */
    uint16_t vfp;           /*!< sRGB row count in vertical front porch segment, max is 65535 */
    uint16_t width;         /*!< sRGB actual picture width, max is 16383 */
    uint16_t height;        /*!< sRGB actual picture height, max is 65535 */
};

int bflb_pec_srgb_init(struct bflb_device_s *dev, struct bflb_pec_srgb_s *srgb);
void bflb_pec_srgb_deinit(struct bflb_device_s *dev);

/******************************************************************
QSPI_CAM
******************************************************************/
struct bflb_pec_qspi_cam_s {
    uint32_t mem;                 /*!< memory address of first instruction */
    uint16_t div;                 /*!< divisor, N = div + 1 */
    uint16_t resolution_x;        /*!< camera horizontal resolution */
    uint16_t resolution_y;        /*!< camera vertical resolution */
    uint8_t dma_enable;           /*!< enable or disable dma */
    uint8_t fifo_threshold;       /*!< tx fifo threshold */
    uint8_t pixel_bits;           /*!< clock count of every pixel */
    uint8_t pin_pclk;             /*!< QSPI CAM PCLK pin index */
    uint8_t pin_d0;               /*!< QSPI CAM D0 pin index */
    uint8_t pin_data_count;       /*!< QSPI CAM data pin count, must be power of 2, such as 1, 2, 4, 8 */
    uint8_t pin_pclk_idle_level;  /*!< QSPI CAM pixel clock level when idle, 0: low level, others: high level */
    uint8_t sample_edge;          /*!< QSPI CAM data sample after clock pin edge, 0: sample after falling edge, others: sample after rising edge */
    uint8_t fifo_direction;       /*!< FIFO direction, PEC_SHIFT_DIR_TO_LEFT or PEC_SHIFT_DIR_TO_RIGHT */
    uint8_t bits_every_push;      /*!< bits of every push when sample data into ISR, 1~32, must be multiple of pin_data_count */
    uint32_t delay_first_us;      /*!< delay time in microsecond before start capature camera data */
    uint32_t det_frame_ns;        /*!< time in nanosecond for inter-frame space detect */
    uint32_t det_line_ns;         /*!< time in nanosecond for inter-line space detect */
    uint16_t skip_clk_line;       /*!< skip clock count before every line */
    uint16_t skip_clk_first_line; /*!< skip clock count before first line */
    uint8_t skip_dly;             /*!< delay count in every clock skip */
    uint8_t sample_dly;           /*!< QSPI CAM data sample delay time after clock pin edge, in unit of PEC clock */
};

int bflb_pec_qspi_cam_init(struct bflb_device_s *dev, struct bflb_pec_qspi_cam_s *cam);
void bflb_pec_qspi_cam_start(struct bflb_device_s *dev, struct bflb_pec_qspi_cam_s *cam);
void bflb_pec_qspi_cam_stop(struct bflb_device_s *dev);

#endif /* __BFLB_PEC_V2_INSTANCE_H__ */
