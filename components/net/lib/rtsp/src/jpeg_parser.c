#include "jpeg_parser.h"
#include "rtp.h"

#define NDEBUG
#ifndef NDEBUG
    #include <stdio.h>
    #define LOGGY(format, ...) printf (format, ##__VA_ARGS__)
#else
    #define LOGGY(format, ...)
#endif /* NDEBUG */

 
static unsigned int parse_jpeg_scanMarker(const unsigned char* data,
                                             unsigned int size,
                                             unsigned int* offset)
{
    while ((data[(*offset)++] != START_MARKER) && ((*offset) < size));
 
    if ((*offset) >= size) {
        return EOI_MARKER;
    } else {
        unsigned int marker;
 
        marker = data[*offset];
        (*offset)++;
 
        return marker;
    }
}
 
static unsigned int _jpegPieceHeaderSize(const unsigned char* data, unsigned int offset)
{
    return data[offset] << 8 | data[offset + 1];
}
 
static int parse_jpeg_readSOF(const unsigned char* data, unsigned int size,
                             unsigned int* offset, struct jpegframe_parser* jpeg_parser)
{
    int i, j;
    CompInfo elem;
    CompInfo info[3] = { {0,}, };
    unsigned int sof_size, off;
    unsigned int width, height, infolen;
 
    off = *offset;
 
    /* we need at least 17 bytes for the SOF */
    if (off + 17 > size) goto wrong_size;
 
    sof_size = _jpegPieceHeaderSize(data, off);
    if (sof_size < 17) goto wrong_length;
 
    *offset += sof_size;
 
    /* skip size */
    off += 2;
 
    /* precision should be 8 */
    if (data[off++] != 8) goto bad_precision;
 
    /* read dimensions */
    height = data[off] << 8 | data[off + 1];
    width = data[off + 2] << 8 | data[off + 3];
    off += 4;
 
    if (height == 0 || height > 2040) goto invalid_dimension;
    if (width == 0 || width > 2040) goto invalid_dimension;
 
    jpeg_parser->width = width / 8;
    jpeg_parser->height = height / 8;
 
    /* we only support 3 components */
    if (data[off++] != 3) goto bad_components;
 
    infolen = 0;
    for (i = 0; i < 3; i++) {
        elem.id = data[off++];
        elem.samp = data[off++];
        elem.qt = data[off++];
 
        /* insertion sort from the last element to the first */
        for (j = infolen; j > 1; j--) {
            if (info[j - 1].id < elem.id) break;
            info[j] = info[j - 1];
        }
        info[j] = elem;
        infolen++;
    }
 
    /* see that the components are supported */
    if (info[0].samp == 0x21) {
        jpeg_parser->type = 0;
    } else if (info[0].samp == 0x22) {
        jpeg_parser->type = 1;
    } else {
        goto invalid_comp;
    }
 
    if (!(info[1].samp == 0x11)) goto invalid_comp;
    if (!(info[2].samp == 0x11)) goto invalid_comp;
    if (info[1].qt != info[2].qt) goto invalid_comp;
 
    return 0;
 
    /* ERRORS */
wrong_size:
    LOGGY("Wrong SOF size\n");
    return -1;
 
wrong_length:
    LOGGY("Wrong SOF length\n");
    return -1;
 
bad_precision:
    LOGGY("Bad precision\n");
    return -1;
 
invalid_dimension:
    LOGGY("Invalid dimension\n");
    return -1;
 
bad_components:
    LOGGY("Bad component\n");
    return -1;
 
invalid_comp:
    LOGGY("Invalid component\n");
    return -1;
}
 
static unsigned int parse_jpeg_readDQT(const unsigned char* data,
                                      unsigned int size,
                                      unsigned int offset,
                                      struct jpegframe_parser* jpeg_parser)
{
    unsigned int quant_size, tab_size;
    unsigned char prec;
    unsigned char id;
 
    if (offset + 2 > size) goto too_small;
 
    quant_size = _jpegPieceHeaderSize(data, offset);
    if (quant_size < 2) goto small_quant_size;
 
    /* clamp to available data */
    if (offset + quant_size > size) {
        quant_size = size - offset;
    }
 
    offset += 2;
    quant_size -= 2;
 
    while (quant_size > 0) {
        /* not enough to read the id */
        if (offset + 1 > size) break;
 
        id = data[offset] & 0x0f;
        if (id == 15) goto invalid_id;
 
        prec = (data[offset] & 0xf0) >> 4;
        if (prec) {
            tab_size = 128;
            jpeg_parser->qTablesLength = 128 * 2;
        } else {
            tab_size = 64;
            jpeg_parser->qTablesLength = 64 * 2;
        }
 
        /* there is not enough for the table */
        if (quant_size < tab_size + 1) goto no_table;
 
        //LOGGY("Copy quantization table: %u\n", id);
        memcpy(&jpeg_parser->qTables[id * tab_size], &data[offset + 1], tab_size);
        // for(int i = 0; i < tab_size; i++){
        // LOGGY("%02x ", data[offset + 1+i]);
        // if((i+1)%16 == 0){
        //     LOGGY("\r\n");
        // }
        // }
        // LOGGY("tab_size = %d \r\n",tab_size);
        tab_size += 1;
        quant_size -= tab_size;
        offset += tab_size;
    }
 
done:
    return offset + quant_size;
 
    /* ERRORS */
too_small:
    LOGGY("DQT is too small\n");
    return size;
 
small_quant_size:
    LOGGY("Quantization table is too small\n");
    return size;
 
invalid_id:
    LOGGY("Invalid table ID\n");
    goto done;
 
no_table:
    LOGGY("table doesn't exist\n");
    goto done;
}
 
static int parse_jpeg_readDRI(const unsigned char* data,
                             unsigned int size, unsigned int* offset, struct jpegframe_parser* jpeg_parser)
{
    unsigned int dri_size, off;
 
    off = *offset;
 
    /* we need at least 4 bytes for the DRI */
    if (off + 4 > size) goto wrong_size;
 
    dri_size = _jpegPieceHeaderSize(data, off);
    if (dri_size < 4) goto wrong_length;
 
    *offset += dri_size;
    off += 2;
 
    jpeg_parser->restartInterval = (data[off] << 8) | data[off + 1];
 
    return 0;
 
wrong_size:
    return -1;
 
wrong_length:
    *offset += dri_size;
    return -1;
}
 
int parse_jpeg_req(unsigned char* data, unsigned int size, struct jpegframe_parser* jpeg_parser)
{
    unsigned int offset = 0;
    unsigned int dqtFound = 0;
    unsigned int sosFound = 0;
    unsigned int sofFound = 0;
    unsigned int driFound = 0;
    unsigned int jpeg_header_size = 0;
 
    while ((sosFound == 0) && (offset < size)) {
        switch (parse_jpeg_scanMarker(data, size, &offset)) {
        case JFIF_MARKER:
            LOGGY("JFIF pos = %x\r\n", offset - 2);
            break;
        case CMT_MARKER:
            LOGGY("CMT pos = %x\r\n", offset - 2);
            break;
        case DHT_MARKER:
            LOGGY("DHT pos = %x\r\n", offset - 2);
            offset += _jpegPieceHeaderSize(data, offset);
            break;
        case SOF_MARKER:
            LOGGY("SOF pos = %x\r\n", offset - 2);
            if (parse_jpeg_readSOF(data, size, &offset,jpeg_parser) != 0) {
                goto invalid_format;
            }
            sofFound = 1;
            break;
        case DQT_MARKER:
            LOGGY("DQT pos = %x\r\n", offset - 2);
            offset = parse_jpeg_readDQT(data, size, offset,jpeg_parser);
            dqtFound = 1;
            break;
        case SOS_MARKER:
            LOGGY("SOS pos = %x\r\n", offset - 2);
            sosFound = 1;
            jpeg_header_size = offset + _jpegPieceHeaderSize(data, offset);
            //jpeg_header_size = offset - 2;
            jpeg_parser->jpegheaderLength = jpeg_header_size;
            break;
        case EOI_MARKER:
            LOGGY("EOI pos = %x\r\n", offset - 2);
            /* EOI reached before SOS!? */
            LOGGY("EOI reached before SOS!?\r\n");
            break;
        case SOI_MARKER:
            LOGGY("SOI pos = %x\r\n", offset - 2);
            LOGGY("SOI found\r\n");
            break;
        case DRI_MARKER:
            LOGGY("DRI pos = %x\r\n", offset - 2);
            LOGGY("DRI found\r\n");
            if (parse_jpeg_readDRI(data, size, &offset,jpeg_parser) == 0) {
                driFound = 1;
                jpeg_parser->driFound = 1;
            }
            break;
        default:
            break;
        }
    }
    if ((dqtFound == 0) || (sofFound == 0)) {
        goto unsupported_jpeg;
    }
 
    if (jpeg_parser->width == 0 || jpeg_parser->height == 0) {
        goto no_dimension;
    }
   
    jpeg_parser->scandata = data + jpeg_header_size;
    jpeg_parser->scandataLength = size - jpeg_header_size;
 
    if (driFound == 1) {
        jpeg_parser->type += 64;
    }
 
    return 0;
 
    /* ERRORS */
unsupported_jpeg:
    return -1;
 
no_dimension:
    return -1;
 
invalid_format:
    return -1;
}
