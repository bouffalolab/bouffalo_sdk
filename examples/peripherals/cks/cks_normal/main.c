#include "board.h"
#include "bflb_cks.h"
#include "bflb_mtimer.h"
#include "bflb_core.h"

#define USER_UNUSED(a) ((void)(a))

struct bflb_device_s *cks;

static void test_case1(struct bflb_device_s *dev) {
    /*case from wiki: https://en.wikipedia.org/wiki/IPv4_header_checksum*/
    static const uint8_t data_src1[] = {0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40,
                                        0x00, 0x40, 0x11, 0x00, 0x00, 0xc0, 0xa8,
                                        0x00, 0x01, 0xc0, 0xa8, 0x00, 0xc7};
    static const uint8_t data_src1_cks[] = {0xB8, 0x61};
    uint16_t cks;
    USER_UNUSED(cks);
    USER_UNUSED(data_src1_cks);
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_LITTLE_ENDIAN);
    cks = bflb_cks_compute(dev, (uint8_t *)data_src1, sizeof(data_src1));
    
    if (cks != (data_src1_cks[0] << 8 | data_src1_cks[1])) {
        printf("Error! CKS result with LE is %04x, should be %02x%02x\r\n", cks, data_src1_cks[0], data_src1_cks[1]);
    } else {
        printf("Pass\r\n");
    }
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_BIG_ENDIAN);
    cks = bflb_cks_compute(dev, (uint8_t *)data_src1, sizeof(data_src1));
    
    if (cks != (data_src1_cks[1] << 8 | data_src1_cks[0])) {
        printf("Error! CKS result with BE is %04x, should be %02x%02x\r\n", cks, data_src1_cks[1], data_src1_cks[0]);
    } else {
        printf("Pass\r\n");
    }
}

static void test_case2(struct bflb_device_s *dev) {
    uint16_t data_segment_one = 0x3F1F;
    int i;
    uint32_t checksum;
    uint16_t cks;
    uint8_t data;
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_LITTLE_ENDIAN);
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        data = data_segment_one & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
        data = data_segment_one >> 8 & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    
    if (cks != (uint16_t)(~checksum << 8 | (~checksum >> 8 & 0xff))) {
        printf("Error! CKS result with LE is %04x, should be %04x\r\n", cks, (uint16_t)(~checksum << 8 | (~checksum >> 8 & 0xff)));
    } else {
        printf("Pass\r\n");
    }
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_BIG_ENDIAN);
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        data = data_segment_one & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
        data = data_segment_one >> 8 & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    
    if (cks != (uint16_t)~checksum) {
        printf("Error! CKS result with BE is %04x, should be %04x\r\n", cks, (uint16_t)~checksum);
    } else {
        printf("Pass\r\n");
    }
}

static void test_case3(struct bflb_device_s *dev) {
    uint16_t data_segment_one = 0x3F1F;
    int i;
    uint32_t checksum;
    uint16_t cks;
    uint8_t data;
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_LITTLE_ENDIAN);
    checksum = 0;
    for (i = 0; i < 1000; i++) {
        checksum += data_segment_one;
        data = data_segment_one & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
        data = data_segment_one >> 8 & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    
    if (cks != (uint16_t)(~checksum << 8 | (~checksum >> 8 & 0xff))) {
        printf("Error! CKS result with LE is %04x, should be %04x\r\n", cks, (uint16_t)(~checksum << 8 | (~checksum >> 8 & 0xff)));
    } else {
        printf("Pass\r\n");
    }
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_BIG_ENDIAN);
    checksum = 0;
    for (i = 0; i < 1000; i++) {
        checksum += data_segment_one;
        data = data_segment_one & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
        data = data_segment_one >> 8 & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
    }
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    
    if (cks != (uint16_t)~checksum) {
        printf("Error! CKS result with BE is %04x, should be %04x\r\n", cks, (uint16_t)~checksum);
    } else {
        printf("Pass\r\n");
    }
}

static void test_case4(struct bflb_device_s *dev) {
    uint16_t data_segment_one = 0x3F1F;
    uint8_t data_segment_two = 0xa1;
    int i;
    uint32_t checksum;
    uint16_t cks;
    uint8_t data;
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_LITTLE_ENDIAN);
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        data = data_segment_one & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
        data = data_segment_one >> 8 & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
    }
    checksum += data_segment_two;
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    cks = bflb_cks_compute(dev, &data_segment_two, 1);
    
    if (cks != (uint16_t)(~checksum << 8 | (~checksum >> 8 & 0xff))) {
        printf("Error! CKS result with LE is %04x, should be %04x\r\n", cks, (uint16_t)(~checksum << 8 | (~checksum >> 8 & 0xff)));
    } else {
        printf("Pass\r\n");
    }
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_BIG_ENDIAN);
    checksum = 0;
    for (i = 0; i < 1; i++) {
        checksum += data_segment_one;
        data = data_segment_one & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
        data = data_segment_one >> 8 & 0xff;
        cks = bflb_cks_compute(dev, &data, 1);
    }
    checksum += data_segment_two;
    while (checksum >> 16) {
        checksum = (checksum >> 16) + (checksum & 0xFFFF);
    }
    cks = bflb_cks_compute(dev, &data_segment_two, 1);
    
    if (cks != (uint16_t)~checksum) {
        printf("Error! CKS result with BE is %04x, should be %04x\r\n", cks, (uint16_t)~checksum);
    } else {
        printf("Pass\r\n");
    }
}

uint16_t sw_chksum(uint8_t *data, uint32_t len) {
  uint32_t sum = 0;
  uint16_t chksum = 0;
  uint32_t size = len;

  if (len % 2 == 1) {
    size=len-1;
    sum += data[size];
  }

  for (uint32_t i = 0; i < size; i = i + 2) {
    sum += ((uint32_t)data[i]);
    sum += ((uint32_t)data[i + 1] << 8);
  }

  while (sum >> 16) {
    sum = (sum >> 16) + (sum & 0x0000FFFF);
  }

  chksum = (uint16_t)sum;
  return ~chksum;
}

static void test_case5(struct bflb_device_s *dev) {
    uint16_t sw_cks = 0;
    uint16_t hw_cks = 0;
    uint32_t time = 0;
    
    static const uint8_t data_src1[] = {0x45, 0x00, 0x00, 0x73, 0x00, 0x00, 0x40,
                                        0x00, 0x40, 0x11, 0x00, 0x00, 0xc0, 0xa8,
                                        0x00, 0x01, 0xc0, 0xa8, 0x00, 0xc7};
    time = (unsigned int)bflb_mtimer_get_time_us();
    
    sw_cks = sw_chksum((uint8_t *)data_src1, sizeof(data_src1));
    
    printf("software checksum time=%ldus\r\n", (unsigned int)bflb_mtimer_get_time_us() - time);
    
    printf("sw_cks is %04x\r\n", sw_cks);
    
    bflb_cks_reset(dev);
    bflb_cks_set_endian(dev, CKS_BIG_ENDIAN);
    
    time = (unsigned int)bflb_mtimer_get_time_us();
    
    hw_cks = bflb_cks_compute(dev, (uint8_t *)data_src1, sizeof(data_src1));
    
    printf("hardware checksum time=%ldus\r\n", (unsigned int)bflb_mtimer_get_time_us() - time);
    
    printf("hw_cks is %04x\r\n", hw_cks);
    
    if (sw_cks != hw_cks) {
        printf("Error!\r\n");
    } else {
        printf("Pass\r\n");
    }
}

/* main */
int main(void)
{
    board_init();
    
    printf("CKS normal case:\r\n");

    cks = bflb_device_get_by_name("cks");

    printf("\r\n--->>> case1 test\r\n");
    test_case1(cks);
    printf("\r\n--->>> case2 test\r\n");
    test_case2(cks);
    printf("\r\n--->>> case3 test\r\n");
    test_case3(cks);
    printf("\r\n--->>> case4 test\r\n");
    test_case4(cks);
    printf("\r\n--->>> case5 test\r\n");
    test_case5(cks);

    printf("\r\nend\r\n");

    while (1) {
    }
}
