#ifndef __SDH_SPEC_H__
#define __SDH_SPEC_H__

/* class 1 */
#define CMD_GO_IDLE_STATE           0  /* bc                          */
#define CMD_SEND_OP_COND            1  /* bcr  [31:0] OCR         R3  */
#define CMD_ALL_SEND_CID            2  /* bcr                     R2  */
#define CMD_SET_RELATIVE_ADDR       3  /* ac   [31:16] RCA        R1  */
#define CMD_SET_DSR                 4  /* bc   [31:16] RCA            */
#define CMD_SWITCH                  6  /* ac   [31:0] See below   R1b */
#define CMD_SELECT_CARD             7  /* ac   [31:16] RCA        R1  */
#define CMD_SEND_EXT_CSD            8  /* adtc                    R1  */
#define CMD_SEND_CSD                9  /* ac   [31:16] RCA        R2  */
#define CMD_SEND_CID                10 /* ac   [31:16] RCA        R2  */
#define CMD_READ_DAT_UNTIL_STOP     11 /* adtc [31:0] dadr        R1  */
#define CMD_STOP_TRANSMISSION       12 /* ac                      R1b */
#define CMD_SEND_STATUS             13 /* ac   [31:16] RCA        R1  */
#define CMD_GO_INACTIVE_STATE       15 /* ac   [31:16] RCA            */
#define CMD_SPI_READ_OCR            58 /* spi                  spi_R3 */
#define CMD_SPI_CRC_ON_OFF          59 /* spi  [0:0] flag      spi_R1 */

/* class 2 */
#define CMD_SET_BLOCKLEN            16 /* ac   [31:0] block len   R1  */
#define CMD_READ_SINGLE_BLOCK       17 /* adtc [31:0] data addr   R1  */
#define CMD_READ_MULTIPLE_BLOCK     18 /* adtc [31:0] data addr   R1  */
#define CMD_SEND_TUNING_BLOCK       19 /* adtc                    R1 */
#define CMD_SEND_TUNING_BLOCK_HS200 21 /* adtc R1*/
                                       /* class 3 */
#define CMD_WRITE_DAT_UNTIL_STOP    20 /* adtc [31:0] data addr   R1  */

/* class 4 */
#define CMD_SET_BLOCK_COUNT         23 /* adtc [31:0] data addr   R1  */
#define CMD_WRITE_BLOCK             24 /* adtc [31:0] data addr   R1  */
#define CMD_WRITE_MULTIPLE_BLOCK    25 /* adtc                    R1  */
#define CMD_PROGRAM_CID             26 /* adtc                    R1  */
#define CMD_PROGRAM_CSD             27 /* adtc                    R1  */

/* class 6 */
#define CMD_SET_WRITE_PROT          28 /* ac   [31:0] data addr   R1b */
#define CMD_CLR_WRITE_PROT          29 /* ac   [31:0] data addr   R1b */
#define CMD_SEND_WRITE_PROT         30 /* adtc [31:0] wpdata addr R1  */

/* class 5 */
#define CMD_ERASE_GROUP_START       35 /* ac   [31:0] data addr   R1  */
#define CMD_ERASE_GROUP_END         36 /* ac   [31:0] data addr   R1  */
#define CMD_ERASE                   38 /* ac                      R1b */

/* class 9 */
#define CMD_FAST_IO                 39 /* ac   <Complex>          R4  */
#define CMD_GO_IRQ_STATE            40 /* bcr                     R5  */

/* class 7 */
#define CMD_LOCK_UNLOCK             42 /* adtc                    R1b */

/* class 8 */
#define CMD_APP_CMD                 55 /* ac   [31:16] RCA        R1  */
#define CMD_GEN_CMD                 56 /* adtc [0] RD/WR          R1  */

/* SD commands                           type  argument      response */
/* class 0 */
/* This is basically the same command as for MMC with some quirks. */
#define CMD_SD_SEND_RELATIVE_ADDR   3 /* bcr                      R6  */
#define CMD_SD_SEND_IF_COND         8 /* bcr  [11:0] See below    R7  */

/* class 10 */
#define CMD_SD_SWITCH               6 /* adtc [31:0] See below    R1  */

#define CMD_SD_ERASE_WR_BLK_START   32
#define CMD_SD_ERASE_WR_BLK_END     33
#define CMD_SD_ERASE                38

/* Application commands */
#define CMD_SD_APP_SET_BUS_WIDTH    6  /* ac   [1:0] bus width    R1  */
#define CMD_SD_APP_SD_STATUS        13 /* ac                      R2  */
#define CMD_SD_APP_SEND_NUM_WR_BLKS 22 /* adtc                    R1  */
#define CMD_SD_APP_OP_COND          41 /* bcr  [31:0] OCR         R3  */
#define CMD_SD_APP_SEND_SCR         51 /* adtc                    R1  */

/* SDIO commands                    type  argument           response */
#define CMD_SD_IO_SEND_OP_COND      5  /* bcr  [23:0] OCR         R4  */
#define CMD_SD_IO_RW_DIRECT         52 /* ac   [31:0] See below   R5  */
#define CMD_SD_IO_RW_EXTENDED       53 /* adtc [31:0] See below   R5  */

/* CMD52 arguments */
#define SDIO_ARG_CMD52_READ         (0 << 31)
#define SDIO_ARG_CMD52_WRITE        (1 << 31)
#define SDIO_ARG_CMD52_FUNC_SHIFT   28
#define SDIO_ARG_CMD52_FUNC_MASK    0x7
#define SDIO_ARG_CMD52_RAW_FLAG     (1 << 27)
#define SDIO_ARG_CMD52_REG_SHIFT    9
#define SDIO_ARG_CMD52_REG_MASK     0x1ffff
#define SDIO_ARG_CMD52_DATA_SHIFT   0
#define SDIO_ARG_CMD52_DATA_MASK    0xff
#define SDIO_R5_DATA(resp)          ((resp)[0] & 0xff)

/* CMD53 arguments */
#define SDIO_ARG_CMD53_READ         (0 << 31)
#define SDIO_ARG_CMD53_WRITE        (1 << 31)
#define SDIO_ARG_CMD53_FUNC_SHIFT   28
#define SDIO_ARG_CMD53_FUNC_MASK    0x7
#define SDIO_ARG_CMD53_BLOCK_MODE   (1 << 27)
#define SDIO_ARG_CMD53_INCREMENT    (1 << 26)
#define SDIO_ARG_CMD53_REG_SHIFT    9
#define SDIO_ARG_CMD53_REG_MASK     0x1ffff
#define SDIO_ARG_CMD53_LENGTH_SHIFT 0
#define SDIO_ARG_CMD53_LENGTH_MASK  0x1ff
#define SDIO_ARG_CMD53_LENGTH_MAX   511

/* resp */
#define RESP_R1                     (1 << 0)
#define RESP_R1B                    (2 << 0)
#define RESP_R2                     (3 << 0)
#define RESP_R3                     (4 << 0)
#define RESP_R4                     (5 << 0)
#define RESP_R6                     (6 << 0)
#define RESP_R7                     (7 << 0)
#define RESP_R5                     (8 << 0) /*SDIO command response type*/

/*the following is response bit*/
#define R1_OUT_OF_RANGE             (1 << 31)        /* er,  Out of range status */
#define R1_ADDRESS_ERROR            (1 << 30)        /* erx, Address error status bit */
#define R1_BLOCK_LEN_ERROR          (1 << 29)        /* er,  Block length error status bit */
#define R1_ERASE_SEQ_ERROR          (1 << 28)        /* er,  Erase sequence error status bit */
#define R1_ERASE_PARAM              (1 << 27)        /* ex,  Erase parameter error status bit */
#define R1_WP_VIOLATION             (1 << 26)        /* erx, Write protection violation status bit */
#define R1_CARD_IS_LOCKED           (1 << 25)        /* sx,  Card locked status bit */
#define R1_LOCK_UNLOCK_FAILED       (1 << 24)        /* erx, lock/unlock error status bit */
#define R1_COM_CRC_ERROR            (1 << 23)        /* er,  CRC error status bit */
#define R1_ILLEGAL_COMMAND          (1 << 22)        /* er,  Illegal command status bit */
#define R1_CARD_ECC_FAILED          (1 << 21)        /* ex,  Card ecc error status bit */
#define R1_CC_ERROR                 (1 << 20)        /* erx, Internal card controller error status bit */
#define R1_ERROR                    (1 << 19)        /* erx, */
#define R1_UNDERRUN                 (1 << 18)        /* ex,  A general or an unknown error status bit */
#define R1_OVERRUN                  (1 << 17)        /* ex,  c */
#define R1_CID_CSD_OVERWRITE        (1 << 16)        /* erx, Cid/csd overwrite status bit */
#define R1_WP_ERASE_SKIP            (1 << 15)        /* sx,  Write protection erase skip status bit */
#define R1_CARD_ECC_DISABLED        (1 << 14)        /* sx,  Card ecc disabled status bit */
#define R1_ERASE_RESET              (1 << 13)        /* sr,  Erase reset status bit */
#define R1_ERR_STATUS(x)            (x & 0xFFFE0000) /*  */
#define R1_READY_FOR_DATA           (1 << 8)         /* sx, Ready for data status bit */
#define R1_SWITCH_ERR               (1 << 7)         /* Switch error status bit */
#define R1_APP_CMD                  (1 << 5)         /* sr, c */

#define R1_CURRENT_STATE(x)         ((x & 0x1E00) >> 9) /* sx, b (4 bits) */
#define R1_STATE_IDLE               0                   /* R1: current state: idle */
#define R1_STATE_READY              1                   /* R1: current state: ready */
#define R1_STATE_INDENTIFY          2                   /* R1: current state: identification */
#define R1_STATE_STANDBY            3                   /* R1: current state: standby */
#define R1_STATE_TRANSFER           4                   /* R1: current state: transfer */
#define R1_STATE_SEND_DATA          5                   /* R1: current state: sending data */
#define R1_STATE_RECE_DATA          6                   /* R1: current state: receiving data */
#define R1_STATE_RECE_PROGRAM       7                   /* R1: current state: programming */
#define R1_STATE_DISCONNECT         8                   /* R1: current state: disconnect */

#define R5_FLAGS_STATUS(x)          (x & 0xCB00)
#define R5_COM_CRC_ERROR            (1 << 15) /* R5: CMD CRC check failed */
#define R5_ILLEGAL_COMMAND          (1 << 14) /* R5: CMD not legal for the card state */
#define R5_ERROR                    (1 << 11) /* R5: unknow error */
#define R5_FUNCTION_NUMBER          (1 << 9)  /* R5: function number was invalid */
#define R5_OUT_OF_RANGE             (1 << 8)  /* R5: CMD argument was out of the allowed range of card */

#define R5_IO_CURRENT_STATE(x)      ((x & 0x3000) >> 12)
#define R5_IO_CURRENT_STATE_DIS     0 /* card not selected */
#define R5_IO_CURRENT_STATE_CMD     1 /* DAT lines free */
#define R5_IO_CURRENT_STATE_TRN     2 /* CMD executing with data transfer */
#define R5_IO_CURRENT_STATE_RFU     3

#endif
