/*
 * Copyright (C) 2017-2022 Bouffalolab Group Holding Limited
 * Change Logs:
 *   Date        Author       Notes
 *   2019-03-25  MSP          the first version
 */

//noise_suppression
#ifndef __NS_H__
#define __NS_H__

#include "typedefs.h"

typedef struct NsHandleT NsHandle;

#ifdef __cplusplus
extern "C" {
#endif

/*
 * This function creates an instance to the noise suppression structure
 *
 * Input:
 *      - NS_inst       : Pointer to noise suppression instance that should be
 *                        created
 *
 * Output:
 *      - NS_inst       : Pointer to created noise suppression instance
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
int msp_ns_create(NsHandle** NS_inst);


/*
 * This function frees the dynamic memory of a specified noise suppression
 * instance.
 *
 * Input:
 *      - NS_inst       : Pointer to NS instance that should be freed
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
int msp_ns_free(NsHandle* NS_inst);


/*
 * This function initializes a NS instance and has to be called before any other
 * processing is made.
 *
 * Input:
 *      - NS_inst       : Instance that should be initialized
 *      - fs            : sampling frequency
 *
 * Output:
 *      - NS_inst       : Initialized instance
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
int msp_ns_init(NsHandle* NS_inst, uint32_t fs);

/*
 * This changes the aggressiveness of the noise suppression method.
 *
 * Input:
 *      - NS_inst       : Noise suppression instance.
 *      - mode          : 0: Mild, 1: Medium , 2: Aggressive
 *
 * Output:
 *      - NS_inst       : Updated instance.
 *
 * Return value         :  0 - Ok
 *                        -1 - Error
 */
int msp_ns_set_policy(NsHandle* NS_inst, int mode);


/*
 * This functions does Noise Suppression for the inserted speech frame. The
 * input and output signals should always be 10ms (80 or 160 samples).
 *
 * Input
 *      - NS_inst       : Noise suppression instance.
 *      - spframe       : Pointer to speech frame buffer for L band
 *      - spframe_H     : Pointer to speech frame buffer for H band
 *      - fs            : sampling frequency
 *
 * Output:
 *      - NS_inst       : Updated NS instance
 *      - outframe      : Pointer to output frame for L band
 *      - outframe_H    : Pointer to output frame for H band
 *
 * Return value         :  0 - OK
 *                        -1 - Error
 */
int msp_ns_process(NsHandle* NS_inst,
                     short* spframe,
                     short* spframe_H,
                     short* outframe,
                     short* outframe_H);

/* Returns the internally used prior speech probability of the current frame.
 * There is a frequency bin based one as well, with which this should not be
 * confused.
 *
 * Input
 *      - handle        : Noise suppression instance.
 *
 * Return value         : Prior speech probability in interval [0.0, 1.0].
 *                        -1 - NULL pointer or uninitialized instance.
 */
float msp_ns_prior_speech_probability(NsHandle* handle);

#ifdef __cplusplus
}
#endif

#endif  // __NS_H__
