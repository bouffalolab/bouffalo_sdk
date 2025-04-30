/*----------------------------------------------*/
/* TJpgDec System Configurations R0.03          */
/*----------------------------------------------*/
#include "bflb_core.h"

#define TJPGDEC_ATTRIBUTE_CODE ATTR_PSRAM_CODE_SECTION
#define TJPGDEC_ATTRIBUTE_DATA ATTR_PSRAM_DATA_SECTION

#define JD_SZBUF               2048
/* Specifies size of stream input buffer */

#if defined(BL616)
#define JD_YUV444_MODE 1 /* support dbi_yuv444 mode */
#else
#define JD_YUV444_MODE 0
#endif
/* YUV444 output
/  JD_FORMAT   : 0
/  JD_USE_SCALE: 0
*/

#if JD_YUV444_MODE
#define JD_FORMAT 0 /* yuv mode must be rgb888 */
#else
#define JD_FORMAT 1 /* rgb565 mode */
#endif
/* Specifies output pixel format.
/  0: RGB888 (24-bit/pix)
/  1: RGB565 (16-bit/pix)
/  2: Grayscale (8-bit/pix)
*/

#define JD_USE_SCALE  0
/* Switches output descaling feature.
/  0: Disable
/  1: Enable
*/

#define JD_TBLCLIP    1
/* Use table conversion for saturation arithmetic. A bit faster, but increases 1 KB of code size.
/  0: Disable
/  1: Enable
*/

#define JD_FASTDECODE 2
/* Optimization level
/  0: Basic optimization. Suitable for 8/16-bit MCUs.
/  1: + 32-bit barrel shifter. Suitable for 32-bit MCUs.
/  2: + Table conversion for huffman decoding (wants 6 << HUFF_BIT bytes of RAM)
*/
