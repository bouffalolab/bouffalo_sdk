/*----------------------------------------------*/
/* TJpgDec System Configurations R0.03          */
/*----------------------------------------------*/

#if __has_include("tjpgdcnf_user.h")

/* User external configuration, User need to use this file as a template */
#include "tjpgdcnf_user.h"

#else

#warning "There is no user TJpgDec conf file and the default conf will be used"

#define TJPGDEC_ATTRIBUTE_CODE 
#define TJPGDEC_ATTRIBUTE_DATA 

#define	JD_SZBUF		1024
/* Specifies size of stream input buffer */

#define JD_YUV444_MODE  1
/* YUV444 output
/  JD_FORMAT   : 0
/  JD_USE_SCALE: 0
*/

#define JD_FORMAT		0
/* Specifies output pixel format.
/  0: RGB888 (24-bit/pix)
/  1: RGB565 (16-bit/pix)
/  2: Grayscale (8-bit/pix)
*/

#define	JD_USE_SCALE	0
/* Switches output descaling feature.
/  0: Disable
/  1: Enable
*/

#define JD_TBLCLIP		1
/* Use table conversion for saturation arithmetic. A bit faster, but increases 1 KB of code size.
/  0: Disable
/  1: Enable
*/

#define JD_FASTDECODE	2
/* Optimization level
/  0: Basic optimization. Suitable for 8/16-bit MCUs.
/  1: + 32-bit barrel shifter. Suitable for 32-bit MCUs.
/  2: + Table conversion for huffman decoding (wants 6 << HUFF_BIT bytes of RAM)
*/

#endif
