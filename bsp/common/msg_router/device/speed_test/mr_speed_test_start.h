/**
 * @file mr_speed_test_start.h
 * @brief Speed test framework startup header file
 */

#ifndef __MR_SPEED_TEST_START_H__
#define __MR_SPEED_TEST_START_H__

#include "mr_msg_ctrl.h"
#include "mr_speed_test.h"

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Macro Definitions
 *****************************************************************************/

/** Maximum number of speed test instances */
#define SPEED_TEST_TEST_MAX 3

#if !SPEED_TEST_ZEROCOPY_LOOPBACK
/** Frame type identifier for speed test */
#define TEST_FRAME_TYPE   0xA5
/** Number of frame elements in the pool */
#define TEST_FRAME_NUM    8
/** Size of each frame including header room */
#define TEST_FRAME_SIZE   (512 * 3 + FRAME_BUFF_HEADER_ROOM)
/** Memory attribute for frame buffer (aligned and non-cached) */
#define FRAME_BUFFER_ATTR __ALIGNED(64) ATTR_NOCACHE_NOINIT_RAM_SECTION
#endif

/*****************************************************************************
 * Function Declarations
 *****************************************************************************/

/**
 * @brief Initialize speed test framework
 * @param msg_ctrl Message control instance
 * @retval None
 * @note This function must be called before any speed test operations
 */
void speed_test_init(mr_msg_ctrl_priv_t *msg_ctrl);

#ifdef __cplusplus
}
#endif

#endif /* __MR_SPEED_TEST_START_H__ */