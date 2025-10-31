#ifndef __PARSER_H__
#define __PARSER_H__


enum
{
    START_MARKER = 0xFF,
    SOI_MARKER   = 0xD8,
    JFIF_MARKER  = 0xE0,
    CMT_MARKER   = 0xFE,
    DQT_MARKER   = 0xDB,
    SOF_MARKER   = 0xC0,
    DHT_MARKER   = 0xC4,
    SOS_MARKER   = 0xDA,
    EOI_MARKER   = 0xD9,
    DRI_MARKER   = 0xDD
};
 
typedef struct
{
    unsigned char id;
    unsigned char samp;
    unsigned char qt;
} CompInfo;





#endif /* __PARSER_H__ */
