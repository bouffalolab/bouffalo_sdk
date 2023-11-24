#include <stdlib.h>
#include <string.h>
#include <stdio.h>
//#include <FreeRTOS.h>
//#include <task.h>
#include <bflb_uart.h>
#include <utils_base64.h>
#include <utils_crc.h>
#include <utils_hex.h>

/* 1KB Block */
#define BL_COREDUMP_PRINT_SEG_N_K 1

#define COREDUMP_UART 0

#define REVERSE(a) (((a)&0xff) << 24 | ((a)&0xff00) << 8 | ((a)&0xff0000) >> 8 | ((a)&0xff000000) >> 24)

#undef read_const_csr
#define read_const_csr(reg) ({ register uintptr_t __tmp; \
  asm ("csrr %0, " #reg : "=r"(__tmp)); __tmp; })

#undef read_csr
#define read_csr(reg) ({ register uintptr_t __tmp; \
  asm volatile ("csrr %0, " #reg : "=r"(__tmp)); __tmp; })

#undef write_csr
#define write_csr(reg, val) ({ \
  asm volatile ("csrw " #reg ", %0" :: "rK"(val)); })

#undef swap_csr
#define swap_csr(reg, val) ({ register uintptr_t __tmp; \
  asm volatile ("csrrw %0, " #reg ", %1" : "=r"(__tmp) : "rK"(val)); __tmp; })

#undef set_csr
#define set_csr(reg, bit) ({ register uintptr_t __tmp; \
  asm volatile ("csrrs %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); __tmp; })

#undef clear_csr
#define clear_csr(reg, bit) ({ register uintptr_t __tmp; \
  asm volatile ("csrrc %0, " #reg ", %1" : "=r"(__tmp) : "rK"(bit)); __tmp; })

#define COREDUM_CMD_BUF_LEN (128)

#define COREDUMP_VERSION "0.0.1"

#define COREDUMP_BLOCK_START_STR "\r\n------ DATA BEGIN "
#define COREDUMP_BLOCK_END_STR "\r\n------ END "
#define COREDUMP_BLOCK_CLOSE_STR " ------\r\n"

#define COREDUMP_START_STR "\r\n-+-+-+- BFLB COREDUMP v" COREDUMP_VERSION " +-+-+-+\r\n"

#define BASE64_LINE_WRAP  100

/* For identify coredump is match the elf */
//uint32_t _$coredump_binary_id$_ __attribute__((used, section(".coredump_binary_id"))) = BFLB_COREDUMP_BINARY_ID;

/**
 * ------ DATA BEGIN addr@len@desc ------
 * contents ....
 * ------ END crc ------
 */

/* For stack check */
extern uintptr_t _sp_main, _sp_base, __StackLimit;
extern uint8_t _ld_ram_size1, _ld_ram_addr1;
extern uint8_t _ld_ram_size2, _ld_ram_addr2;
extern uint8_t _ld_ram_size3, _ld_ram_addr3;
extern uint8_t _ld_ram_size4, _ld_ram_addr4;
extern uint8_t _ld_ram_size5, _ld_ram_addr5;
extern uint8_t _ld_ram_size6, _ld_ram_addr6;
//#define DEBUG
extern struct bflb_device_s *console;

#ifdef DEBUG

uint32_t test_data[] = {
    0x11111111,
    0x22222222,
    0x33333333,
    0x44444444,
    0x55555555,
    0x66666666,
    0x77777777,
    0x88888888,
    0x99999999,
    0xaaaaaaaa,
    0xbbbbbbbb,
    0xcccccccc,
    0xdddddddd,
    0xeeeeeeee,
    0xffffffff,
    0xdeadbeef,
    0xa5a5a5a5,
    0x5a5a5a5a,
    0x0000ffff,
    0xffff0000};

#endif

/* Coredump state machine */
enum coredump_status {
  COREDUMP_IDLE = 0,
  COREDUMP_ACTIVE,
  COREDUMP_PARSE,
};

/* Dump type of given content */
enum dump_type {
  DUMP_ASCII,       /* Dump string */
  DUMP_BASE64_BYTE, /* Dump memory in byte units, in base64 format. */
  DUMP_BASE64_WORD, /* Dump memory in word units, in base64 format. */
  DUMP_REG_OTHERS,
  DUMP_CSR,         /* Dump string */
  DUMP_TYPE_MAX,
};

typedef void (*dump_handler_t)(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);

static void dump_ascii(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);
static void dump_base64_byte(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);
static void dump_base64_word(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);
static void dump_wifi_reg_others(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);
static void dump_csr(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx);

static const dump_handler_t dump_handler_list[DUMP_TYPE_MAX] = {
    dump_ascii,
    dump_base64_byte,
    dump_base64_word,
    dump_wifi_reg_others,
    dump_csr,
};

extern uint8_t __LD_CONFIG_EM_SEL;

/* Define default ram dump list */
static const struct mem_hdr {
  uintptr_t addr;
  unsigned int length;
  enum dump_type type;
  const char *desc;
} mem_hdr[] = {
#ifdef DEBUG
    {(uintptr_t)test_data, (unsigned int)sizeof(test_data), DUMP_BASE64_BYTE, "test_data_byte"},
    {(uintptr_t)test_data, (unsigned int)sizeof(test_data), DUMP_BASE64_WORD, "test_data_word"},
    {(uintptr_t) "asdasdasdasdasdsada", 0, DUMP_ASCII, "test_string"},
#elif defined(BL602)
    {(uintptr_t)&_ld_ram_addr1, (unsigned int)&_ld_ram_size1, DUMP_BASE64_BYTE, "ram"},
    {(uintptr_t)&_ld_ram_addr2, (unsigned int)&_ld_ram_size2, DUMP_BASE64_BYTE, "wifi_ram"},
    {(uintptr_t)0x24B00000, (unsigned int)0x55c, DUMP_BASE64_WORD, "0x24B00000-0x24B0055C"},
    {(uintptr_t)0x24B08000, (unsigned int)0x560, DUMP_BASE64_WORD, "0x24B08000-0x24B08560"},
    {(uintptr_t)0x24C00000, (unsigned int)0x3c, DUMP_BASE64_WORD, "0x24C00000-0x24C0003C"},
    {(uintptr_t)0x24C00800, (unsigned int)0xbc, DUMP_BASE64_WORD, "0x24C00800-0x24C008BC"},
    {(uintptr_t)0xf0000000, (unsigned int)0x20, DUMP_REG_OTHERS, "others_reg"},
    {(uintptr_t)0xf0004000, (unsigned int)12, DUMP_CSR, "dump riscv csr"},
    {(uintptr_t)0xf0004000, (unsigned int)12, DUMP_CSR, "dump riscv csr"},

    {(uintptr_t)0x40000000, (unsigned int)0x318, DUMP_BASE64_WORD, "GLB_reg"},
    {(uintptr_t)0x4000A100, (unsigned int)0x8F, DUMP_BASE64_WORD, "uart1_reg"},
    {(uintptr_t)0x4000A420, (unsigned int)0x98, DUMP_BASE64_WORD, "pwm_reg"},
    {(uintptr_t)0x4000E404, (unsigned int)0x04, DUMP_BASE64_WORD, "PDS_reg"},
    {(uintptr_t)0x4000F030, (unsigned int)0x04, DUMP_BASE64_WORD, "HBN_reg"},
    {(uintptr_t)0x28008000, (unsigned int)&__LD_CONFIG_EM_SEL, DUMP_BASE64_WORD, "EM_REG"},
#elif defined(BL702)
    {(uintptr_t)&_ld_ram_addr1, (unsigned int)&_ld_ram_size1, DUMP_BASE64_BYTE, "tcm_ocram"},
    {(uintptr_t)&_ld_ram_addr2, (unsigned int)&_ld_ram_size2, DUMP_BASE64_BYTE, "hbnram"},
    {(uintptr_t)&_ld_ram_addr3, (unsigned int)&_ld_ram_size3, DUMP_BASE64_BYTE, "stack"},
    {(uintptr_t)0x28008000, (unsigned int)&__LD_CONFIG_EM_SEL, DUMP_BASE64_WORD, "EM_REG"},
#elif defined(BL702L)
    {(uintptr_t)&_ld_ram_addr1, (unsigned int)&_ld_ram_size1, DUMP_BASE64_BYTE, "tcm_ocram"},
    {(uintptr_t)&_ld_ram_addr2, (unsigned int)&_ld_ram_size2, DUMP_BASE64_BYTE, "hbnram"},
    {(uintptr_t)&_ld_ram_addr3, (unsigned int)&_ld_ram_size3, DUMP_BASE64_BYTE, "stack"},
    {(uintptr_t)0x28008000, (unsigned int)&__LD_CONFIG_EM_SEL, DUMP_BASE64_WORD, "EM_REG"},
#elif defined(BL808)
    {(uintptr_t)&_ld_ram_addr1, (unsigned int)&_ld_ram_size1, DUMP_BASE64_BYTE, "ram_psram"},
    {(uintptr_t)&_ld_ram_addr2, (unsigned int)&_ld_ram_size2, DUMP_BASE64_BYTE, "ram_wifi"},
    {(uintptr_t)&_ld_ram_addr3, (unsigned int)&_ld_ram_size3, DUMP_BASE64_BYTE, "ram_memory"},
    {(uintptr_t)&_ld_ram_addr4, (unsigned int)&_ld_ram_size4, DUMP_BASE64_BYTE, "xram_memory"},
    {(uintptr_t)0x28010000, (unsigned int)&__LD_CONFIG_EM_SEL, DUMP_BASE64_WORD, "EM_REG"},
#elif defined(WB03)
    {(uintptr_t)&_ld_ram_addr1, (unsigned int)&_ld_ram_size1, DUMP_BASE64_BYTE, "ram_tcm"},
    {(uintptr_t)&_ld_ram_addr2, (unsigned int)&_ld_ram_size2, DUMP_BASE64_BYTE, "ram_wifi"},
#elif defined(BL616)
    {(uintptr_t)0xf0004000, (unsigned int)12, DUMP_CSR, "dump riscv csr"},
    {(uintptr_t)&_ld_ram_addr2, (unsigned int)&_ld_ram_size2, DUMP_BASE64_BYTE, "ram_tcm"},
    {(uintptr_t)&_ld_ram_addr3, (unsigned int)&_ld_ram_size3, DUMP_BASE64_BYTE, "dtcm_memory"}, //FIXME if use bluetooth EM
    {(uintptr_t)&_ld_ram_addr4, (unsigned int)&_ld_ram_size4, DUMP_BASE64_BYTE, "nocache_ram_memory"},
    {(uintptr_t)&_ld_ram_addr5, (unsigned int)&_ld_ram_size5, DUMP_BASE64_BYTE, "ram_memory"},
    {(uintptr_t)&_ld_ram_addr6, (unsigned int)&_ld_ram_size6, DUMP_BASE64_BYTE, "ram_wifi"},
    {(uintptr_t)0x28010000, (unsigned int)&__LD_CONFIG_EM_SEL, DUMP_BASE64_WORD, "EM_REG"},
#endif
};
/* Get current stack top */
static inline uintptr_t cd_getsp(void) {
  register uintptr_t sp;
  __asm__("add %0, x0, sp"
          : "=r"(sp));
  return sp;
}

static void cd_flush(void) {
  //bl_uart_flush(COREDUMP_UART);
}

static int cd_getchar(char *inbuf) {
  int ret;
  ret = bflb_uart_getchar(console);
  if (ret < 0) {
    return 0;
  }
  *inbuf = (char)ret;
  return 1;
}

static void cd_putchar(const char *buf, size_t len) {
  size_t i;
  for (i=0; i<len; i++) {
    bflb_uart_putchar(console, buf[i]);
  }
}

static void cd_base64_wirte_block(const uint8_t buf[4], void *opaque) {
  int *line_wrap = (int *)opaque;
  cd_putchar((const char *)buf, 4);
  if (++(*line_wrap) > (BASE64_LINE_WRAP >> 2)) {
    cd_putchar("\r\n", 2);
    *line_wrap = 0;
  }
}

static void dump_ascii(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
  /* reuse len as index here, for calculate the crc */
  for (len = 0; len < (ssize_t)strlen((const char *)data); len++) {
    utils_crc32_stream_feed(crc_ctx, *((const char *)data + len));
  }

  cd_putchar(data, strlen((const char *)data));
}

struct base64_byte_ctx {
  /* for output */
  int line_wrap;  /* Base64 line wrap */

  /* for input */
  uintptr_t addr_curr;
  uintptr_t addr_end;
  struct crc32_stream_ctx *crc_ctx;
};

static int read_byte_cb(uint8_t *data, void *opaque) {
  struct base64_byte_ctx *ctx = (struct base64_byte_ctx *)opaque;
  if (ctx->addr_curr < ctx->addr_end) {
    *data = *(uint8_t *)ctx->addr_curr++;

    /* update crc checksum */
    utils_crc32_stream_feed(ctx->crc_ctx, *data);

    return 0;
  } else {
    return 1;
  }
}

static void dump_base64_byte(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
  struct base64_byte_ctx ctx = {0};

  ctx.addr_curr = (uintptr_t)data;
  ctx.addr_end = (uintptr_t)data + len;
  ctx.crc_ctx = crc_ctx;

  utils_base64_encode_stream(read_byte_cb, cd_base64_wirte_block, (void *)&ctx);
}

struct base64_word_ctx {
  /* for output */
  int line_wrap; /* base64 line wrap */

  /* for input */
  uintptr_t addr_base;
  uintptr_t addr_curr;
  uintptr_t addr_end;
  uint8_t buf[4] __attribute__((aligned(4))); /* cache */
  struct crc32_stream_ctx *crc_ctx;
};

static int read_word_cb(uint8_t *data, void *opaque) {
  struct base64_word_ctx *ctx = (struct base64_word_ctx *)opaque;
  uintptr_t base;

  if (ctx->addr_curr < ctx->addr_end) {
    base = (ctx->addr_curr >> 2) << 2;
    if (base != ctx->addr_base) {
      ctx->addr_base = base;
      *(uint32_t *)ctx->buf = *(uint32_t *)base;
    }

    *data = ctx->buf[ctx->addr_curr & 0x3];
    ctx->addr_curr++;

    /* update crc checksum */
    utils_crc32_stream_feed(ctx->crc_ctx, *data);

    return 0;
  } else {
    return 1;
  }
}

static void dump_base64_word(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
  struct base64_word_ctx ctx = {0};

  ctx.addr_base = ((uintptr_t)data >> 2) << 2;
  ctx.addr_curr = (uintptr_t)data;
  ctx.addr_end = ctx.addr_base + len;
  *(uint32_t *)ctx.buf = *(uint32_t *)ctx.addr_base;
  ctx.crc_ctx = crc_ctx;

  utils_base64_encode_stream(read_word_cb, cd_base64_wirte_block, (void *)&ctx);
}

static uint32_t read_reg(uint32_t reg_addr)
{
    return *(volatile uint32_t *)(reg_addr);
}

static void write_reg(uint32_t reg_addr, uint32_t val)
{
    *(volatile uint32_t *)(reg_addr) = val;
}

static void dump_wifi_reg(uint32_t *ptr)
{
    uint32_t val;

    //record rc rf
    val = read_reg(0x40001220);
    val &= ~((uint32_t)0xf0000000);
    val |= (1 << 28) & ((uint32_t)0xf0000000);
    write_reg(0x40001220, val);

    val = read_reg(0x40001224);

    val = read_reg(0x40001220);
    val &= ~((uint32_t)0xf0000000);
    val |= (2 << 28) & ((uint32_t)0xf0000000);
    write_reg(0x40001220, val);

    val = read_reg(0x40001224);
    ptr[0] = ((val & ((uint32_t)0xffffffff)) >> 28) & 0x3;
    ptr[1] = ((val & ((uint32_t)0xffffffff)) >> 25) & 0x3;

    //mac hw sm1 sm2
    ptr[2] = read_reg(0x44B00500);
    ptr[3] = read_reg(0x44B00504);

    //coex
    write_reg(0x44B00510, (read_reg(0x44B00510) & ~((uint32_t)0x000000FF)) | ((uint32_t)49 << 0));
    write_reg(0x44B00510, (read_reg(0x44B00510) & ~((uint32_t)0x0000FF00)) | ((uint32_t)0 << 8));
    ptr[4] = read_reg(0x44B0050C);

    write_reg(0x44B00510, (read_reg(0x44B00510) & ~((uint32_t)0x000000FF)) | ((uint32_t)11 << 0));
    write_reg(0x44B00510, (read_reg(0x44B00510) & ~((uint32_t)0x0000FF00)) | ((uint32_t)47 << 8));
    ptr[5] = read_reg(0x44B0050C);

    // config phydiagmux
    write_reg(0x44900070 + 0x4, 0 | 58 << 0 | 3 << 8 | 0 << 16 |  0 << 24);
    write_reg(0x44900068, (read_reg(0x44900068) & ~((uint32_t)0x0000FFFF)) | ((uint32_t)(0 | 20 << 0) << 0));

    val = read_reg(0x400000d0);
    val &= ~((uint32_t)0xffffffff);
    val |= ((0 | 0 << 30 | 1 << 2) << 0)&((uint32_t)0xffffffff);
    write_reg(0x400000d0, val);

    val= read_reg(0x400000d4);
    val &= ~((uint32_t)0xffffffff);
    val |= ((0 | 1 << 30 | 1 << 2) << 0)&((uint32_t)0xffffffff);
    write_reg(0x400000d4, val);

    val = read_reg(0x400000d8);
    val &= ~((uint32_t)0xffffffff);
    val |= ((0 | (uint32_t)2 << 30 | 1 << 2) << 0)&((uint32_t)0xffffffff);
    write_reg(0x400000d8, val);

    val = read_reg(0x400000dc);
    val &= ~((uint32_t)0xffffffff);
    val |= ((0 | (uint32_t)3 << 30 | 1 << 2) << 0)&((uint32_t)0xffffffff);
    write_reg(0x400000dc, val);

    val = read_reg(0x400000e0);
    val &= ~((uint32_t)0x00000001);
    val |= (0 << 0)&((uint32_t)0x00000001);
    write_reg(0x400000e0, val);
    val = read_reg(0x400000e0);
    ptr[6] = ((val & ((uint32_t)0xfffffffe)) >> 1);

    // config phydiagmux
    write_reg(0x44900070+0x4, 0 | 9 << 0 | 11 << 8 | 0 << 16 | 0 << 24);
    val = read_reg(0x400000e0);
    val &= ~((uint32_t)0x00000001);
    val |= (0 << 0)&((uint32_t)0x00000001);
    write_reg(0x400000e0, val);

    val = read_reg(0x400000e0);
    ptr[7] = ((val & ((uint32_t)0xfffffffe)) >> 1);
}

static void dump_wifi_reg_others(const void *data, ssize_t len, struct crc32_stream_ctx *crc_ctx) {
  struct base64_word_ctx ctx = {0};
  uint32_t reg_arr[8];

  dump_wifi_reg(reg_arr);
  ctx.addr_base = ((uintptr_t)reg_arr >> 2) << 2;
  ctx.addr_curr = (uintptr_t)reg_arr;
  ctx.addr_end = ctx.addr_base + 32;
  *(uint32_t *)ctx.buf = *(uint32_t *)ctx.addr_base;
  ctx.crc_ctx = crc_ctx;

  utils_base64_encode_stream(read_word_cb, cd_base64_wirte_block, (void *)&ctx);
}

/* Dump CSR */
static int read_csr_cb(uint8_t *data, void *opaque)
{
  struct base64_word_ctx *ctx = (struct base64_word_ctx *)opaque;
  uintptr_t base;

  if (ctx->addr_curr < ctx->addr_end)
    {
      base = (ctx->addr_curr >> 2) << 2;
      if (base != ctx->addr_base)
        {
          ctx->addr_base = base;
          switch (base)
            {
            case 0x0:
              *(uint32_t *)ctx->buf = read_csr(mcause);
              break;
            case 0x4:
              *(uint32_t *)ctx->buf = read_csr(mtval);
              break;
            case 0x8:
              *(uint32_t *)ctx->buf = read_csr(mepc);
              break;
            default:
              *(uint32_t *)ctx->buf = read_csr(mstatus);
            }
        }

      *data = ctx->buf[ctx->addr_curr & 0x3];
      ctx->addr_curr++;

      /* update crc checksum */

      utils_crc32_stream_feed(ctx->crc_ctx, *data);

      return 0;
    }
  else
    {
      return 1;
    }
}

static void dump_csr(const void *data, ssize_t len,
    struct crc32_stream_ctx *crc_ctx)
{
  struct base64_word_ctx ctx = {0};

  ctx.addr_base = -1;
  ctx.addr_curr = ((uintptr_t)data & 0xFFF);
  ctx.addr_end = ctx.addr_curr + len;
  ctx.crc_ctx = crc_ctx;

  utils_base64_encode_stream(read_csr_cb, cd_base64_wirte_block,
      (void *)&ctx);
}

/**
 * Coredump initialize.
 *
 * @return result
 */
static void bl_coredump_print(uintptr_t addr, uint32_t len, const char *desc, enum dump_type type) {
  struct crc32_stream_ctx crc_ctx;
  uint32_t crc;

  uintptr_t tmp;
  char tmp_buf[(sizeof(tmp) << 1)] = {0};

  /* XXX stack overflow check */

  while (cd_getsp() <= (uintptr_t)&__StackLimit);

  /* Print block header */
  cd_putchar(COREDUMP_BLOCK_START_STR, strlen(COREDUMP_BLOCK_START_STR));

  /* print address */
  tmp = REVERSE(addr);
  utils_bin2hex(tmp_buf, &tmp, sizeof(uintptr_t));
  cd_putchar(tmp_buf, sizeof(uintptr_t) << 1);

  cd_putchar("@", 1);

  tmp = REVERSE(len);
  utils_bin2hex(tmp_buf, &tmp, sizeof(uint32_t));
  cd_putchar(tmp_buf, sizeof(uint32_t) << 1);

  if (desc != NULL) {
    cd_putchar("@", 1);
    cd_putchar(desc, strlen(desc));
  }

  cd_putchar(COREDUMP_BLOCK_CLOSE_STR, strlen(COREDUMP_BLOCK_CLOSE_STR));

  /* Init crc context */
  utils_crc32_stream_init(&crc_ctx);

  /* Dump block content */
  dump_handler_list[type]((const void *)addr, (ssize_t)len, &crc_ctx);

  /* Dump end */
  cd_putchar(COREDUMP_BLOCK_END_STR, strlen(COREDUMP_BLOCK_END_STR));

  crc = utils_crc32_stream_results(&crc_ctx);

  dump_handler_list[DUMP_BASE64_WORD]((const void *)&crc, (ssize_t)sizeof(uint32_t), &crc_ctx);

  cd_putchar(COREDUMP_BLOCK_CLOSE_STR, sizeof(COREDUMP_BLOCK_CLOSE_STR));

  cd_flush();
}

#ifndef BL_COREDUMP_PRINT_SEG_N_K
#define BL_COREDUMP_PRINT_SEG_N_K 32
#endif
static void bl_coredump_print_n_k(uintptr_t addr, uint32_t len, const char *desc, enum dump_type type)
{
    uint32_t printed_len;
    uint32_t seg;

    for(seg = 0; seg * BL_COREDUMP_PRINT_SEG_N_K * 1024 < len; seg++) {
        printed_len = seg*BL_COREDUMP_PRINT_SEG_N_K*1024;
        bl_coredump_print(addr + printed_len, len - printed_len >= BL_COREDUMP_PRINT_SEG_N_K*1024 ? BL_COREDUMP_PRINT_SEG_N_K*1024: len - printed_len, desc, type);
    }
}

/**
 * Coredump initialize.
 *
 * @return result
 */
void bl_coredump_parse(const uint8_t *buf, unsigned int len) {
  char command;
  unsigned int i = 0;

  command = buf[i++];

  switch (command) {
  case 'x':
    do {
      unsigned int addr, length = 0;

      if (len - i >= 8) {
        utils_hex2bin((char *)&buf[i], 8, (uint8_t *)&addr, sizeof(addr));
        addr = REVERSE(addr);
        i += 8;

        command = buf[i++];
        if (command == 'l' && (len - i) >= 8) {
          utils_hex2bin((char *)&buf[i], 8, (uint8_t *)&length, sizeof(length));
          length = REVERSE(length);
        } else {
          length = 0x1000;
        }
        bl_coredump_print_n_k(addr, length, NULL, DUMP_BASE64_WORD);
      }
    } while (0);
    return;

  case 'd':
    do {
      for (i = 0; i < (sizeof(mem_hdr) / sizeof(mem_hdr[0])); i++) {
        bl_coredump_print_n_k(mem_hdr[i].addr, mem_hdr[i].length, mem_hdr[i].desc, mem_hdr[i].type);
      }
    } while (0);
    return;

  default:
    return;
  }
}

/**
 * Coredump initialize.
 *
 * @return result
 */
void bl_coredump_run() {
  char c;
  uint8_t cmd_buf[COREDUM_CMD_BUF_LEN + 1] = {0};
  enum coredump_status status = COREDUMP_IDLE;
  uint8_t cmd_pos = 0;

  cd_putchar(COREDUMP_START_STR, strlen(COREDUMP_START_STR));

  /* Dump all pre-defined memory region by default */
  for (cmd_pos = 0; cmd_pos < (sizeof(mem_hdr) / sizeof(mem_hdr[0])); cmd_pos++) {
    if (mem_hdr[cmd_pos].length == 0) {
      continue;
    }
    bl_coredump_print_n_k(mem_hdr[cmd_pos].addr, mem_hdr[cmd_pos].length, mem_hdr[cmd_pos].desc, mem_hdr[cmd_pos].type);
  }

  while (1) {
    if (cd_getchar(&c) == 0) {
      continue;
    }

  _reactive: /* In some scenarios we need to reactivate the state machine */
    switch (status) {
    case COREDUMP_IDLE:
      if (c == '&') {
        status = COREDUMP_ACTIVE;
        cmd_pos = 0;
      }
      continue;

    case COREDUMP_ACTIVE:
      if (c == '#') {
        status = COREDUMP_PARSE;
        cmd_buf[cmd_pos] = 0;
        goto _reactive;
      }

      cmd_buf[cmd_pos++] = c;
      if (cmd_pos > COREDUM_CMD_BUF_LEN) {
        status = COREDUMP_IDLE;
      }
      continue;

    case COREDUMP_PARSE:
      status = COREDUMP_IDLE;
      bl_coredump_parse(cmd_buf, cmd_pos);
      continue;
    }
  }
}
