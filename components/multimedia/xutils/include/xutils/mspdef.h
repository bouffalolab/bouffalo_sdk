/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

#ifndef __MSPDEF_H__
#define __MSPDEF_H__

#define MSP_ALIGN_SIZE                   8           /**< todo: mv to config*/

/* boolean type definitions */
#define MSP_TRUE                         1           /**< boolean true  */
#define MSP_FALSE                        0           /**< boolean fails */

/* null pointer definition */
#define MSP_NULL                         0

/**
 * @ingroup BasicDef
 *
 * @def MSP_ALIGN(size, align)
 * Return the most contiguous size aligned at specified width. MSP_ALIGN(13, 4)
 * would return 16.
 */
#define MSP_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

/**
 * @ingroup BasicDef
 *
 * @def MSP_ALIGN_DOWN(size, align)
 * Return the down number of aligned at specified width. MSP_ALIGN_DOWN(13, 4)
 * would return 12.
 */
#define MSP_ALIGN_DOWN(size, align)      ((size) & ~((align) - 1))

#endif
