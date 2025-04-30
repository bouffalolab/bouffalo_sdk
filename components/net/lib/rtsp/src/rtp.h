#ifndef __RTP_H__
#define __RTP_H__


/* NALU types */
enum {
    NALU_TYPE_SPS = 7,          /* sequence parameter set */
    NALU_TYPE_PPS = 8,          /* picture parameter set */
};

/* Media type. */
enum media_type {
    MEDIA_TYPE_VIDEO,
    MEDIA_TYPE_AUDIO,
};

/* RTP payload type. */
enum {
    RTP_PT_JPEG = 26,
    RTP_PT_H264 = 96,
    RTP_PT_PCMU = 0,
    RTP_PT_PCMA = 8,
    RTP_PT_AAC  = 97,
    RTP_PT_OPUS = 101,
};

/* NALU payload type. */
enum {
//  NALU_PT_SINGLE = 1...23,
    NALU_PT_FU_A = 28,
    NALU_PT_FU_B = 29,
};

/* Interleaved header for transport(RTP over RTSP). */
struct intlvd {
    unsigned char dollar;
    unsigned char chn;
    unsigned short sz;
};

/* RTP header. */
struct rtp_hdr {
#ifdef BIGENDIAN
	unsigned char v:2;          /* protocol version */
	unsigned char p:1;         	/* padding flag */
	unsigned char x:1;         	/* header extension flag */
	unsigned char cc:4;       	/* CSRC count */
	unsigned char m:1;         	/* marker bit */
	unsigned char pt:7;        	/* payload type */
#else
	unsigned char cc:4;
	unsigned char x:1;
	unsigned char p:1;
	unsigned char v:2;
	unsigned char pt:7;
	unsigned char m:1;
#endif
	unsigned short seq;			/* sequence number */
	unsigned int   ts;          /* timestamp */
	unsigned int   ssrc;        /* synchronization source */
#if 0
	unsigned int   csrc[2];		/* optional CSRC list */
#endif
};

/* RTP payload format for single NALU. */
struct single {
    char hdr;
    char *pl;
    unsigned int pl_sz;
};

/* RTP payload format for FU-A. */
struct fu_a {
    char idr;
    char hdr;
    char *pl;                   /* FU payload. */
    unsigned int pl_sz;
};

/* RTP payload format for FU-B. */
struct fu_b {
    char idr;
    char hdr;
    unsigned short don;         /* Decoding order number(in network byte order). */
    char *pl;                   /* FU payload. */
    unsigned int pl_sz;
};

/* RTP payload format for Audio. */
struct audio {
    char *pl;
    unsigned int pl_sz;
};

struct jpeghdr {
    unsigned int tspec:8;   /* type-specific field */
    unsigned int off:24;    /* fragment byte offset */
    unsigned char type;            /* id of jpeg decoder params */
    unsigned char q;               /* quantization factor (or table id) */
    unsigned char width;           /* frame width in 8 pixel blocks */
    unsigned char height;          /* frame height in 8 pixel blocks */
};
struct jpeghdr_rst {
    unsigned short dri;
    unsigned int f:1;
    unsigned int l:1;
    unsigned int count:14;
};


struct jpegframe_parser {
    unsigned char width;
    unsigned char height;
    unsigned char type;
    unsigned char precision;
    unsigned char qFactor;
    unsigned char driFound;
    unsigned int  jpegheaderLength;
    unsigned char qTables[2*128];
    unsigned short qTablesLength;
    unsigned short restartInterval;
    unsigned char* scandata;
    unsigned int   scandataLength;
};

struct jpeghdr_qtable {
    unsigned char  mbz;
    unsigned char  precision;
    unsigned short length;
};

struct jpeg_frame {
    unsigned char *jpeg_data;
    int data_len;
};

/* RTP payload format for Jpeg. */
struct jpeg {
    struct jpeghdr          jpghdr;
    struct jpeghdr_rst      rsthdr;
    struct jpeghdr_qtable   qtblhdr;
    struct jpeg_frame       jpgfrm;
    struct jpegframe_parser parser;
};


/* RTP payload. */
struct rtp_pl {
    union {
        struct single single;
        struct fu_a fu_a;
        struct fu_b fu_b;
        struct audio audio;
        struct jpeg jpeg;
    }
};

#define MAX_RTP_PKT_SZ  1440
#define MAX_RTP_PL_SZ   (MAX_RTP_PKT_SZ - sizeof(struct rtp_hdr) - sizeof(struct intlvd))


#define RTP_JPEG_RESTART           0x40

struct rtsp_sess;
int create_rtp_sess(struct rtsp_sess *sessp);
void destroy_rtp_sess(struct rtsp_sess *sessp);
int prefetch_frm(struct rtsp_sess *sessp);


#endif /* __RTP_H__ */
