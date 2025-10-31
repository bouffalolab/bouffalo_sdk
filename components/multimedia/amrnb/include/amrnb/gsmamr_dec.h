/* ------------------------------------------------------------------
 * Copyright (C) 1998-2009 PacketVideo
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the License for the specific language governing permissions
 * and limitations under the License.
 * -------------------------------------------------------------------
 */
/****************************************************************************************
Portions of this file are derived from the following 3GPP standard:

    3GPP TS 26.073
    ANSI-C code for the Adaptive Multi-Rate (AMR) speech codec
    Available from http://www.3gpp.org

(C) 2004, 3GPP Organizational Partners (ARIB, ATIS, CCSA, ETSI, TTA, TTC)
Permission to distribute, modify and use this file under the standard license
terms listed above has been obtained from the copyright holder.
****************************************************************************************/
/*
------------------------------------------------------------------------------



 Filename: /audio/gsm-amr/c/include/gsmamr_dec.h

     Date: 09/10/2001

------------------------------------------------------------------------------
 REVISION HISTORY

 Description: Adding comments and removing some tables as per review comments.

 Description: Replace enum Mode with enum Frame_Type_3GPP and updated function
              prototype of AMRDecode().

 Description: Added back the enum Mode type definition, removed RXFrameType
              type definition, and updated AMRDecode and GSMInitDecode function
              prototypes.

 Description: Added #defines for WMF and IF2. Updated AMRDecode function
              prototype.

 Description: Removed enum Mode type definition and updated AMRDecode function
              prototype.

 Description: Renamed WMF and IF2 to AMR_WMF and AMR_IF2, respectively. Added
              #define for AMR_ETS format.

 Description: Rename input format defines to make it unique to the decoder.

 Description: Added comment to describe L_FRAME.

 Description: Moved _cplusplus #ifdef after Include section.

 Description: Included file "typedefs.h" to avoid re-declaring similar typedef
              this for OSCL-ed compatibility

 Description: Included file "gsm_amr_typedefs.h" and eliminated re-definition
              of types UWord8, Word8, Word16

 Description:

------------------------------------------------------------------------------
 INCLUDE DESCRIPTION

 This header contains all the necessary information needed to allow the gsm amr
 decoder library to be used properly upon release.

------------------------------------------------------------------------------
*/
#ifndef _GSMAMR_DEC_H_
#define _GSMAMR_DEC_H_

/*----------------------------------------------------------------------------
; INCLUDES
----------------------------------------------------------------------------*/

#include "gsm_amr_typedefs.h"
#include "frame_type_3gpp.h"
#include "amrdecode.h"

/*--------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif

    /*----------------------------------------------------------------------------
    ; MACROS
    ----------------------------------------------------------------------------*/


    /*----------------------------------------------------------------------------
    ; DEFINES
    ----------------------------------------------------------------------------*/
    /* Number of 13-bit linear PCM samples per 20 ms frame */
    /* L_FRAME = (8 kHz) * (20 msec) = 160 samples         */
#define L_FRAME     160

    /*----------------------------------------------------------------------------
    ; EXTERNAL VARIABLES REFERENCES
    ----------------------------------------------------------------------------*/


    /*----------------------------------------------------------------------------
    ; SIMPLE TYPEDEF'S
    ----------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------------
    ; ENUMERATED TYPEDEF'S
    ----------------------------------------------------------------------------*/

    /*----------------------------------------------------------------------------
    ; STRUCTURES TYPEDEF'S
    ----------------------------------------------------------------------------*/


    /*----------------------------------------------------------------------------
    ; GLOBAL FUNCTION DEFINITIONS
    ----------------------------------------------------------------------------*/
    /*
     * This function allocates memory for filter structure and initializes state
     * memory used by the GSM AMR decoder. This function returns zero. It will
     * return negative one if there is an error.
     */
    Word16 GSMInitDecode(void **state_data,
                         Word8 *id);

    /*
     * This function resets the state memory used by the GSM AMR decoder. This
     * function returns zero. It will return negative one if there is an error.
     */
    Word16 Speech_Decode_Frame_reset(void *state_data);

    /*
     * This function frees up the memory used for the state memory of the
     * GSM AMR decoder.
     */
    void GSMDecodeFrameExit(void **state_data);


#ifdef __cplusplus
}
#endif

#endif  /* _GSMAMR_DEC_H_ */

