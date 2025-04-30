#ifndef __AVI_JPEG_SD_H_
#define __AVI_JPEG_SD_H_

#include <stdio.h>

#include "ff.h"

/* ------------------------------------- avi typedef start ------------------------------------- */

#pragma pack(1)

typedef uint64_t QWORD;
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef uint16_t WORD;
typedef uint8_t BYTE;

/* riff */
typedef struct {
    DWORD dwRIFF;
    DWORD dwSize;
    DWORD dwFourCC;
    BYTE data[0]; /* dwSize in length */
} RIFF;

/* chunk */
typedef struct {
    DWORD dwFourCC;
    DWORD dwSize;
    BYTE data[0]; /* dwSize in length */
} CHUNK;

/* list */
typedef struct {
    DWORD dwList;
    DWORD dwSize;
    DWORD dwFourCC;
    BYTE data[0]; /* dwSize - 4 in length */
} LIST;

/* MainAVIHeader chunk */
typedef struct {
    DWORD dwFourCC;
    DWORD dwSize;

    DWORD dwMicroSecPerFrame;
    DWORD dwMaxBytesPerSec;
    DWORD dwPaddingGranularity;

    DWORD dwFlags;
    DWORD dwTotalFrames;
    DWORD dwInitialFrames;
    DWORD dwStreams;
    DWORD dwSuggestedBufferSize;

    DWORD dwWidth;
    DWORD dwHeight;

    DWORD dwReserved[4];
} MainAVIHeader;

typedef struct {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT;

/* vids/auds strh chunk */
typedef struct {
    DWORD dwFourCC;
    DWORD dwSize;

    DWORD fccType;
    DWORD fccHandler;
    DWORD dwFlags;
    WORD wPriority;
    WORD wLanguage;
    DWORD dwInitialFrames;
    DWORD dwScale;
    DWORD dwRate;
    DWORD dwStart;
    DWORD dwLength;
    DWORD dwSuggestedBufferSize;
    DWORD dwQuality;
    DWORD dwSampleSize;
    RECT rcFrame;
} AVIStreamHeader;

/* auds strh chunk */
typedef struct {
    DWORD dwFourCC;
    DWORD dwSize;

    WORD wFormatTag;
    WORD wChannels;
    DWORD dwSamplesPerSec;
    DWORD dwAvgBytesPerSec;
    WORD wBlockAlign;
    WORD wBitsPerSample;
    WORD wSize;

} WAVEFORMAT;

/* vids strf chunk */
typedef struct {
    DWORD dwFourCC;
    DWORD dwSize;

    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;

} EXBMINFOHEADER;

/* super index array */
struct avisuperindex_entry {
    QWORD qwOffset;
    DWORD dwSize;
    DWORD dwDuration;
};
/* Open-DML super indxl chunk */
typedef struct {
    DWORD fcc;           /* chunk fourCC */
    DWORD cd;            /* chunk size */
    WORD wLongsPerEntry; /* sizeof(struct avisuperindex_entry) / sizeof(DWORD), must be 4 */
    BYTE bIndexSubType;  /*  */
    BYTE bIndexType;     /*  */
    DWORD nEntriesInUse; /* num of (superindex_entry) */
    DWORD dwChunkId;     /* strame fourCC, ex: 00dc, 01wb */
    DWORD dwReserved[3];
    struct avisuperindex_entry superindex_entry[0];
} AVISUPERINDEXCHUNK;

/* std index array */
struct avistdindex_entry {
    DWORD dwOffset;
    DWORD dwSize;   /* bit 31 is set if this is NOT a keyframe */ 
};
/* Open-DML std indxl chunk */
typedef struct {
    DWORD fcc;           /* chunk fourCC */
    DWORD cd;            /* chunk size */
    WORD wLongsPerEntry; /* sizeof(struct avistdindex_entry) / sizeof(DWORD), must be 2 */
    BYTE bIndexSubType;  /*  */
    BYTE bIndexType;     /*  */
    DWORD nEntriesInUse; /* num of (stdindex_entry) */
    DWORD dwChunkId;     /* strame fourCC, exp: 00dc, 01wb */
    QWORD qwBaseOffset;  /*  */
    DWORD dwReserved3;
    struct avistdindex_entry stdindex_entry[0];
} AVISTDINDEXCHUNK;

typedef struct {
    DWORD name;
    DWORD dwSize;
    DWORD dwTotalFrames;
} ODMLExtendedAVIheader;

/* Create the avi file header, Cannot be used for file parsing */
/* clang-format off */
typedef struct {
    RIFF riff_avi;

    /* Information block */
    LIST hdrl;
        MainAVIHeader avih;

        LIST vids_strl;
            AVIStreamHeader vids_strh;
            EXBMINFOHEADER vids_strf;
            AVISUPERINDEXCHUNK vids_sup_idx;

        LIST auds_strl;
            AVIStreamHeader auds_strh;
            WAVEFORMAT auds_strf;
            AVISUPERINDEXCHUNK auds_sup_idx;

    LIST odml;
        ODMLExtendedAVIheader dmlh;

    /* data block and index */
    LIST movi;
        AVISTDINDEXCHUNK vids_std_idx_0;
        AVISTDINDEXCHUNK auds_std_idx_0;
        CHUNK movi_data;
        /* ...... */
        AVISTDINDEXCHUNK vids_std_idx_1;
        AVISTDINDEXCHUNK auds_std_idx_1;
        CHUNK movi_data_;
        /* ...... */
        /* ...... */

} avi_file_head;

/* clang-format on */

#pragma pack()

/* MainAVIHeader dwFlages define */
#define AVIF_HASINDEX       0x00000010
#define AVIF_MUTUSEINDEX    0x00000020
#define AVIF_ISINTERLEAVED  0x00000100
#define AVIF_WASCAPTUREFILE 0x00010000
#define AVIF_COPYRIGHTED    0x00020000
#define AVIF_TRUSTCKTYPE    0x00000800

/* WAVEFORMAT wFormatTag define */
#define WAVE_FORMAT_PCM
#define WAVE_FORMAT_ADPCM

#define WAVE_NOT_KEY_FRAME ((uint32_t)1<<31)

/* AVIINDEXCHUNK bIndexSubType/bIndexType define */
#define AVI_INDEX_OF_INDEXES    0x00    /* when each entry in aIndex array points to an index chunk */
#define AVI_INDEX_OF_CHUNKS     0x01    /* when each entry in aIndex array points to a chunk in the file */  
#define AVI_INDEX_IS_DATA       0x80    /* when each entry is aIndex is really the data */
#define AVI_INDEX_2FIELD        0x01    /* when fields within frames are also indexed */

/* ------------------------------------- avi typedef end ------------------------------------- */

#define PSRAM_COPY_MODE (0)
#if(PSRAM_COPY_MODE)
#define PSRAM_COPY_BUFF_SIZE    (16 * 1024)
#endif

#define AVI_HEAD_BUFF               (16 * 1024) /* Set the value to a multiple of 512 */

#define AVI_SUB_IDX_BUFF            (8 * 1024) /* Set the value to a multiple of 512 */

/* pre-allocated file size, individual file size cannot exceed this value */
#define AVI_FILE_ALLOC_SIZE         (2 * 1024 * 1024 * 1024)  /* 2G */

/* super index num */
#define AVI_VIDS_SUP_IDX_NUM        (450)
#define AVI_AUDS_SUP_IDX_NUM        (450)
/* std index num */
#define AVI_VIDS_SUB_IDX_NUM        (500)
#define AVI_AUDS_SUB_IDX_NUM        (500)

/* avi init param */
typedef struct {
    /* frame fps auds/vids */
    uint32_t avi_fps;

    /* vids */
    uint8_t vids_en;
    uint16_t width;
    uint32_t hight;

    /* auds */
    uint8_t auds_en;
    uint32_t auds_frame_size;
    uint32_t chanenl_num;
    uint32_t sample_rate;
    uint32_t sample_bit_size;

} avi_param_t;

/* avi total struct */
typedef struct {
    /* fatfs file header */
    FIL *avi_file;
    /* fs cluster size, Used for alignment and optimized IO performance */
    uint32_t fs_clus_size;

    avi_param_t avi_param;

    /* audio frame size */
    uint16_t auds_frame_size;   /* avi_frame_fps * auds_frame_size = samples_rate * channel * samples_size */
    /* audio frame */
    uint16_t auds_scale_size;   /* auds_scale_size = channel * samples_size */
    
    /*************** header ***************/
    /* head_buff */
    uint8_t *head_buff;
    /* buff and data size */
    uint32_t head_buff_size;
    uint32_t head_data_size;
    /* disk info */
    uint32_t disk_head_sector_offset;
    uint32_t disk_head_sector_num;

    /* the location of important data */
    /* avi riff */
    RIFF *p_riff;
    uint32_t riff_offset;

    /* avi main header */
    MainAVIHeader *p_main_header;
    uint32_t main_header_offset;

    /* avi video strh */
    AVIStreamHeader *p_vids_strh;
    uint32_t vids_strh_offset;
    /* avi video strf */
    EXBMINFOHEADER *p_vids_strf;
    uint32_t vids_strf_offset;
    /* avi video upper_level_index (Super Index) */
    AVISUPERINDEXCHUNK *p_vids_super_idx;
    uint32_t vids_super_idx_offset;
    uint32_t vids_super_idx_total;

    /* avi audio strh */
    AVIStreamHeader *p_auds_strh;
    uint32_t auds_strh_offset;
    /* avi audio strf */
    WAVEFORMAT *p_auds_strf;
    uint32_t auds_strf_offset;
    /* avi audio upper_level_index (Super Index) */
    AVISUPERINDEXCHUNK *p_auds_super_idx;
    uint32_t auds_super_idx_offset;
    uint32_t auds_super_idx_total;

    /* dmlh */
    ODMLExtendedAVIheader *p_dmlh;
    uint32_t dmlh_offset;

    /* avi movi list */
    LIST *p_movi_list;
    uint32_t movi_list_offset;

    /***************  avix riff ***************/

    /* sub_idx_buff */
    uint8_t *sub_idx_buff;
    uint32_t sub_idx_buff_size;
    uint32_t sub_idx_data_size;
    /* disk info */
    uint32_t disk_sub_idx_sector_offset;
    uint32_t disk_sub_idx_sector_num;

    /* avi RIFF avi extended (AVIX) */
    RIFF *p_avix_riff;
    uint32_t avix_riff_offset;

    /* AVIX movi list */
    LIST *p_avix_movi_list;
    uint32_t avix_movi_list_offset;

    /*****  vids idx *****/
    /* avi vids sub idx (Standard Index) */
    AVISTDINDEXCHUNK *p_vids_std_idx;
    uint32_t vids_sub_idx_offset;
    uint32_t vids_sub_idx_total;

    /*****  auds idx *****/
    /* avi auds sub idx (Standard Index) */
    AVISTDINDEXCHUNK *p_auds_std_idx;
    uint32_t auds_sub_idx_offset;
    uint32_t auds_sub_idx_total;

    /* std sub idx junk_chunk offset */
    uint32_t avi_sub_junk_offset;
    uint32_t avi_sub_junk_size;

    /* avi vids last chunk ptr */
    uint64_t vids_last_chunk_ptr;
    uint64_t vids_last_chunk_size;

    /* avi auds last chunk ptr */
    uint64_t auds_last_chunk_ptr;
    uint64_t auds_last_chunk_size;

    /* new chunk piont */
    uint64_t new_chunk_offset;

} avi_file_t;

avi_file_t *avi_file_create(FIL *p_file, avi_param_t *avi_param);
int avi_write_frame(avi_file_t *avi_hd, void *jpeg_frame, uint32_t frame_size, void *pcm_frame);
int avi_jpeg_file_sync(avi_file_t *avi_hd, uint8_t mode);
int avi_jpeg_close(avi_file_t *avi_hd);

int save_avi_to_sdcard_init(void);

#endif /* __AVI_JPEG_SD_H_ */