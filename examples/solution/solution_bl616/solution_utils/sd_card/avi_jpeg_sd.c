#include <string.h>
#include <stdlib.h>

#include "bflb_l1c.h"
#include "bflb_mtimer.h"
#include "bflb_core.h"

#include <FreeRTOS.h>
#include <task.h>

#include "board.h"

#include "fatfs_diskio_register.h"
#include "ff.h"
#include "diskio.h"

#include "jpeg_sd.h"
#include "avi_jpeg_sd.h"

#if (0)
#define SD_AVI_DBG(a, ...) printf("[SD_AVI dbg]:" a, ##__VA_ARGS__)
#else
#define SD_AVI_DBG(a, ...)
#endif

#if (1)
#define SD_AVI_INFO(a, ...) printf("[SD_AVI info]:" a, ##__VA_ARGS__)
#else
#define SD_AVI_INFO(a, ...)
#endif

#define SD_AVI_ERR(a, ...) printf("[SD_AVI err]:" a, ##__VA_ARGS__)

#if (PSRAM_COPY_MODE)

static ATTR_DTCM_SECTION __ALIGNED(64) uint8_t psram_copy_buff[PSRAM_COPY_BUFF_SIZE];

FRESULT psram_f_write(FIL *fp, const void *buff, UINT btw, UINT *bw)
{
    FRESULT ret;
    UINT btw_c, bw_c = 0;

    *bw = 0;

    for (;;) {
        btw_c = (sizeof(psram_copy_buff) < btw) ? sizeof(psram_copy_buff) : btw;

        memcpy(psram_copy_buff, buff + *bw, btw_c);
        bw_c = 0;

        ret = f_write(fp, psram_copy_buff, btw_c, &bw_c);
        *bw += bw_c;
        btw -= bw_c;

        if (ret != FR_OK || btw_c != bw_c) {
            return ret;
        }

        if (btw == 0) {
            return ret;
        }
    }
}

#else

FRESULT psram_f_write(FIL *fp, const void *buff, UINT btw, UINT *bw)
{
    return f_write(fp, buff, btw, bw);
}

#endif

/* file format */
/*
    RIFF riff AVI;

        LIST hdrl;
            MainAVIHeader avih;

            LIST vids_strl;
                AVIStreamHeader vids_strh;
                EXBMINFOHEADER vids_strf;
                AVISUPERINDEXCHUNK vids_sup_idx;

            LIST auds_strl;
                AVIStreamHeader auds_strh;
                EXBMINFOHEADER auds_strf;
                AVISUPERINDEXCHUNK auds_sup_idx;

        LIST odml;
            ODMLExtendedAVIheader dmlh;

        LIST movi;
            CHUNK junk_align (align to 512)
    
    RIFF riff AVIX;
        LIST movi;
            AVISTDINDEXCHUNK vids_sub_idx;
            AVISTDINDEXCHUNK auds_sub_idx;
            CHUNK junk_align (align to 512)
            CHUNK movi_data
            ......

    RIFF riff AVIX;
        LIST movi;
            AVISTDINDEXCHUNK vids_sub_idx;
            AVISTDINDEXCHUNK auds_sub_idx;
            CHUNK junk_align (align to 512)
            CHUNK movi_data
            ......
    
    ......

*/

/* TODO:
    1. AVI 文件结构使用 RIFF 拓展模式，第一个 RIFF 块小于1G，拓展 RIFF 块小于2G，
       这里实际第一个 RIFF 块只用来保存头部结构，包括超级索引，并不保存数据，所以大小固定。
    2. AVI 索引结构使用 Open-DML 类型，并使用 Super Index 二级索引。
    3. 为了便于管理，每个 AVIX RIFF 拓展块只有开头有一组子索引(STD Index)，后面是音视频数据块。
    4. 为了避免文件 sleek，RIFF 块的头部和索引建立了数据镜像，以扇区对齐，写入时记录物理扇区号，后续更新直接将镜像写入。
*/

/* build RIFF header
*/
static int avi_build_header(avi_file_t *avi_hd)
{
    /* 
        TODO:   初始化 RIFF AIV 头部镜像，其中包含了 vids 与 auds 的一级索引, 最大索引条数由宏定义的，
            这是一个独立 RIFF 结构镜像，长度固定，后续的子索引和数据是通过 AVIX RIFF 拓展来增长的
            已经将头部总结构对齐到 512 字节，并记录了需要用的扇区数量，
            使用的时候直接写入文件开头，并把物理扇区地址记录下来，
            更新的时候就可以绕过文件系统,直接将整体写入记录物理扇区，达到性能最优。
     */

    /************* Establish a basic data mirror structure *****************/
    /* clang-format off */

    RIFF *riff = NULL;
    
        LIST *hdrl = NULL;
            MainAVIHeader *avih = NULL;

            LIST *vids_strl = NULL;
                AVIStreamHeader *vids_strh = NULL;
                EXBMINFOHEADER *vids_strf = NULL;
                AVISUPERINDEXCHUNK *vids_super_idx = NULL;

            LIST *auds_strl = NULL;
                AVIStreamHeader *auds_strh = NULL;
                WAVEFORMAT *auds_strf = NULL;
                AVISUPERINDEXCHUNK *auds_super_idx = NULL;

        LIST *odml = NULL;
            ODMLExtendedAVIheader *dmlh = NULL;

        LIST *movi = NULL;

    /* clang-format on */

    /* riff */
    riff = (void *)avi_hd->head_buff + avi_hd->head_data_size;
    avi_hd->p_riff = riff;
    avi_hd->riff_offset = avi_hd->head_data_size;
    avi_hd->head_data_size += sizeof(RIFF);

    /* list hdrl */
    hdrl = (void *)avi_hd->head_buff + avi_hd->head_data_size;
    avi_hd->head_data_size += sizeof(LIST);

    /* avih */
    avih = (void *)avi_hd->head_buff + avi_hd->head_data_size;
    avi_hd->p_main_header = avih;
    avi_hd->main_header_offset = avi_hd->head_data_size;
    avi_hd->head_data_size += sizeof(MainAVIHeader);

    if (avi_hd->avi_param.vids_en) {
        /* list vids_strl */
        vids_strl = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->head_data_size += sizeof(LIST);

        /* vids_strh */
        vids_strh = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->p_vids_strh = vids_strh;
        avi_hd->vids_strh_offset = avi_hd->head_data_size;
        avi_hd->head_data_size += sizeof(AVIStreamHeader);

        /* vids_strf */
        vids_strf = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->p_vids_strf = vids_strf;
        avi_hd->vids_strf_offset = avi_hd->head_data_size;
        avi_hd->head_data_size += sizeof(EXBMINFOHEADER);

        /* vids_super_idx */
        vids_super_idx = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->p_vids_super_idx = vids_super_idx;
        avi_hd->vids_super_idx_offset = avi_hd->head_data_size;
        avi_hd->vids_super_idx_total = AVI_VIDS_SUP_IDX_NUM;
        avi_hd->head_data_size += (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_VIDS_SUP_IDX_NUM);
    }

    if (avi_hd->avi_param.auds_en) {
        /* list auds_strl */
        auds_strl = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->head_data_size += sizeof(LIST);

        /* auds_strh */
        auds_strh = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->p_auds_strh = auds_strh;
        avi_hd->auds_strh_offset = avi_hd->head_data_size;
        avi_hd->head_data_size += sizeof(AVIStreamHeader);

        /* auds_strf */
        auds_strf = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->p_auds_strf = auds_strf;
        avi_hd->auds_strf_offset = avi_hd->head_data_size;
        avi_hd->head_data_size += sizeof(WAVEFORMAT);

        /* auds_index */
        auds_super_idx = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->p_auds_super_idx = auds_super_idx;
        avi_hd->auds_super_idx_offset = avi_hd->head_data_size;
        avi_hd->auds_super_idx_total = AVI_AUDS_SUP_IDX_NUM;
        avi_hd->head_data_size += (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_AUDS_SUP_IDX_NUM);
    }

    /* list odml */
    odml = (void *)avi_hd->head_buff + avi_hd->head_data_size;
    avi_hd->head_data_size += sizeof(LIST);

    /* dmlh */
    dmlh = (void *)avi_hd->head_buff + avi_hd->head_data_size;
    avi_hd->p_dmlh = dmlh;
    avi_hd->dmlh_offset = avi_hd->head_data_size;
    avi_hd->head_data_size += sizeof(ODMLExtendedAVIheader);

    /* list movi */
    movi = (void *)avi_hd->head_buff + avi_hd->head_data_size;
    avi_hd->p_movi_list = movi;
    avi_hd->movi_list_offset = avi_hd->head_data_size;
    avi_hd->head_data_size += sizeof(LIST);

    /* junk chunk, keep align to 512 (sd_card sector) */
    int junk_size = avi_hd->head_data_size % 512;
    if (junk_size) {
        /*  */
        junk_size = 512 - junk_size;
        if (junk_size < sizeof(CHUNK)) {
            junk_size += 512;
        }

        CHUNK *junk_chunk = (void *)avi_hd->head_buff + avi_hd->head_data_size;
        avi_hd->head_data_size += junk_size;

        if (avi_hd->head_data_size > avi_hd->head_buff_size) {
            SD_AVI_ERR("header junk size over %d->%d\r\n", avi_hd->head_data_size, avi_hd->head_buff_size);
            return -3;
        }

        /* fill */
        junk_chunk->dwFourCC = *(uint32_t *)"JUNK";
        junk_chunk->dwSize = junk_size - sizeof(CHUNK);
    }

    /************** Fill in basic internal data ****************/

    SD_AVI_INFO("header data size %d\r\n", avi_hd->head_data_size);
    SD_AVI_INFO("header junk size %d\r\n", junk_size);
    SD_AVI_INFO("header sector num %d\r\n", avi_hd->head_data_size / 512);

    /* save the sector_num */
    avi_hd->disk_head_sector_num = avi_hd->head_data_size / 512;

    if (avi_hd->head_data_size > avi_hd->head_buff_size) {
        SD_AVI_ERR("head size size over: %d -> %d\r\n", avi_hd->head_data_size, avi_hd->head_buff_size);
        return -1;
    }

    /* riff fill */
    riff->dwRIFF = *(uint32_t *)"RIFF";
    riff->dwSize = sizeof(RIFF) - 8 +
                   sizeof(LIST) + sizeof(MainAVIHeader) +
                   sizeof(LIST) + sizeof(ODMLExtendedAVIheader) +
                   sizeof(LIST) + junk_size;
    if (avi_hd->avi_param.vids_en) {
        riff->dwSize += sizeof(LIST) + sizeof(AVIStreamHeader) + sizeof(EXBMINFOHEADER) + (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_VIDS_SUP_IDX_NUM);
    }
    if (avi_hd->avi_param.auds_en) {
        riff->dwSize += sizeof(LIST) + sizeof(AVIStreamHeader) + sizeof(WAVEFORMAT) + (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_AUDS_SUP_IDX_NUM);
    }
    riff->dwFourCC = *(uint32_t *)"AVI ";

    /* list hdrl fill */
    hdrl->dwList = *(uint32_t *)"LIST";
    hdrl->dwSize = sizeof(LIST) - 8 + sizeof(MainAVIHeader);
    if (avi_hd->avi_param.vids_en) {
        hdrl->dwSize += sizeof(LIST) + sizeof(AVIStreamHeader) + sizeof(EXBMINFOHEADER) + (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_VIDS_SUP_IDX_NUM);
    }
    if (avi_hd->avi_param.auds_en) {
        hdrl->dwSize += sizeof(LIST) + sizeof(AVIStreamHeader) + sizeof(WAVEFORMAT) + (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_AUDS_SUP_IDX_NUM);
    }
    hdrl->dwFourCC = *(uint32_t *)"hdrl";

    /* avih fill  */
    avih->dwFourCC = *(uint32_t *)"avih";
    avih->dwSize = sizeof(MainAVIHeader) - 8;
    /* TODO: MainAVIHeader fill */

    /* vids */
    if (avi_hd->avi_param.vids_en) {
        /* vids list hdrl fill */
        vids_strl->dwList = *(uint32_t *)"LIST";
        vids_strl->dwSize = sizeof(LIST) - 8 +
                            sizeof(AVIStreamHeader) + sizeof(EXBMINFOHEADER) + (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_VIDS_SUP_IDX_NUM);
        vids_strl->dwFourCC = *(uint32_t *)"strl";

        {
            /* vids strh fill */
            vids_strh->dwFourCC = *(uint32_t *)"strh";
            vids_strh->dwSize = sizeof(AVIStreamHeader) - 8;
            /* TODO: AVIStreamHeader fill */

            /* vids strf fill */
            vids_strf->dwFourCC = *(uint32_t *)"strf";
            vids_strf->dwSize = sizeof(EXBMINFOHEADER) - 8;
            vids_strf->biSize = sizeof(EXBMINFOHEADER) - 8;
            /* TODO: EXBMINFOHEADER fill */

            /* vids super index fill */
            vids_super_idx->fcc = *(uint32_t *)"indx";
            vids_super_idx->cd = sizeof(AVISUPERINDEXCHUNK) - 8 +
                                 sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_VIDS_SUP_IDX_NUM;
            vids_super_idx->wLongsPerEntry = 4;
            /* TODO: AVISUPERINDEXCHUNK fill */
        }
    }

    /* auds */
    if (avi_hd->avi_param.auds_en) {
        /* auds list hdrl fill */
        auds_strl->dwList = *(uint32_t *)"LIST";
        auds_strl->dwSize = sizeof(LIST) - 8 +
                            sizeof(AVIStreamHeader) + sizeof(WAVEFORMAT) + (sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_AUDS_SUP_IDX_NUM);
        auds_strl->dwFourCC = *(uint32_t *)"strl";

        {
            /* auds strh fill */
            auds_strh->dwFourCC = *(uint32_t *)"strh";
            auds_strh->dwSize = sizeof(AVIStreamHeader) - 8;
            /* TODO: AVIStreamHeader fill */

            /* auds srtf fill */
            auds_strf->dwFourCC = *(uint32_t *)"strf";
            auds_strf->dwSize = sizeof(WAVEFORMAT) - 8;
            /* TODO: WAVEFORMAT fill */

            /* auds super index fill */
            auds_super_idx->fcc = *(uint32_t *)"indx";
            auds_super_idx->cd = sizeof(AVISUPERINDEXCHUNK) - 8 +
                                 sizeof(AVISUPERINDEXCHUNK) + sizeof(struct avisuperindex_entry) * AVI_AUDS_SUP_IDX_NUM;
            auds_super_idx->wLongsPerEntry = 4;
            /* TODO: AVISUPERINDEXCHUNK fill */
        }
    }

    /* odml list fill */
    odml->dwList = *(uint32_t *)"LIST";
    odml->dwSize = sizeof(LIST) - 8 +
                   sizeof(ODMLExtendedAVIheader);
    odml->dwFourCC = *(uint32_t *)"odml";

    {
        /* dmlh fill */
        dmlh->name = *(uint32_t *)"dmlh";
        dmlh->dwSize = sizeof(ODMLExtendedAVIheader) - 8;
        dmlh->dwTotalFrames = 0;
    }

    /* list movi fill */
    movi->dwList = *(uint32_t *)"LIST";
    movi->dwSize = sizeof(LIST) - 8;
    movi->dwFourCC = *(uint32_t *)"movi";

    return 0;
}

/*
    avi_build_subindex
*/
static int avi_build_subindex(avi_file_t *avi_hd)
{
    /* 
        TODO:   初始化这个子索引的数据镜像，同时也是一个 AVIX 拓展结构，开头有 RIFF 和 movi_list，
            RIFF 和 movi_list 记录的长度需要随着数据增加而修改，
            包含了 vids 与 auds, 最大索引条数由宏定义的，
            已经将整体结构对齐到 512 字节，并记录了需要用的扇区数量，
            使用的之前先对齐到簇，或者的数量足够的连续扇区，然后直接写入，并把物理扇区地址记录下来，
            更新的时候就可以绕过文件系统,直接将整体写入记录物理扇区，达到性能最优。
     */

    if (avi_hd->sub_idx_buff == 0) {
        return -1;
    }

    avi_hd->sub_idx_data_size = 0;

    /* AVIX riff */
    RIFF *aiv_sub_avix = (void *)avi_hd->sub_idx_buff + avi_hd->sub_idx_data_size;
    avi_hd->p_avix_riff = aiv_sub_avix;
    avi_hd->avix_riff_offset = avi_hd->sub_idx_data_size;
    avi_hd->sub_idx_data_size += sizeof(RIFF);

    /* AVIX movi LIST */
    LIST *avix_movi_list = (void *)avi_hd->sub_idx_buff + avi_hd->sub_idx_data_size;
    avi_hd->p_avix_movi_list = avix_movi_list;
    avi_hd->avix_movi_list_offset = avi_hd->sub_idx_data_size;
    avi_hd->sub_idx_data_size += sizeof(LIST);

    /* vids */
    if (avi_hd->avi_param.vids_en) {
        /* vids_std_idx */
        AVISTDINDEXCHUNK *vids_std_idx = (void *)avi_hd->sub_idx_buff + avi_hd->sub_idx_data_size;
        avi_hd->p_vids_std_idx = vids_std_idx;
        avi_hd->vids_sub_idx_offset = avi_hd->sub_idx_data_size;
        avi_hd->vids_sub_idx_total = AVI_VIDS_SUB_IDX_NUM;
        avi_hd->sub_idx_data_size += (sizeof(AVISTDINDEXCHUNK) + sizeof(struct avistdindex_entry) * AVI_VIDS_SUB_IDX_NUM);

        if (avi_hd->sub_idx_data_size > avi_hd->sub_idx_buff_size) {
            SD_AVI_ERR("sub_idx vids size over %d->%d\r\n", avi_hd->sub_idx_data_size, avi_hd->sub_idx_buff_size);
            return -2;
        }

        /* vids_std_idx fill */
        vids_std_idx->fcc = *(uint32_t *)"ix00";
        vids_std_idx->cd = sizeof(AVISTDINDEXCHUNK) - 8 + sizeof(struct avistdindex_entry) * AVI_VIDS_SUB_IDX_NUM;
        vids_std_idx->wLongsPerEntry = 2;
        vids_std_idx->bIndexSubType = 0;
        vids_std_idx->bIndexType = AVI_INDEX_OF_CHUNKS;
        vids_std_idx->nEntriesInUse = 0;
        vids_std_idx->dwChunkId = *(uint32_t *)"00dc";
        vids_std_idx->qwBaseOffset = 0;

        SD_AVI_INFO("sub vids offset %d\r\n", avi_hd->vids_sub_idx_offset);
    }

    if (avi_hd->avi_param.auds_en) {
        /* auds_std_idx */
        AVISTDINDEXCHUNK *auds_std_idx = (void *)avi_hd->sub_idx_buff + avi_hd->sub_idx_data_size;
        avi_hd->p_auds_std_idx = auds_std_idx;
        avi_hd->auds_sub_idx_offset = avi_hd->sub_idx_data_size;
        avi_hd->auds_sub_idx_total = AVI_AUDS_SUB_IDX_NUM;
        avi_hd->sub_idx_data_size += (sizeof(AVISTDINDEXCHUNK) + sizeof(struct avistdindex_entry) * AVI_AUDS_SUB_IDX_NUM);

        if (avi_hd->sub_idx_data_size > avi_hd->sub_idx_buff_size) {
            SD_AVI_ERR("sub_idx auds size over %d->%d\r\n", avi_hd->sub_idx_data_size, avi_hd->sub_idx_buff_size);
            return -3;
        }

        /* auds_std_idx fill */
        auds_std_idx->fcc = *(uint32_t *)"ix01";
        auds_std_idx->cd = sizeof(AVISTDINDEXCHUNK) - 8 + sizeof(struct avistdindex_entry) * AVI_AUDS_SUB_IDX_NUM;
        auds_std_idx->wLongsPerEntry = 2;
        auds_std_idx->bIndexSubType = 0;
        auds_std_idx->bIndexType = AVI_INDEX_OF_CHUNKS;
        auds_std_idx->nEntriesInUse = 0;
        auds_std_idx->dwChunkId = *(uint32_t *)"01wb";
        auds_std_idx->qwBaseOffset = 0;

        SD_AVI_INFO("sub auds offset %d\r\n", avi_hd->auds_sub_idx_offset );
    }

    /* junk chunk, keep align to 512 (sd_card block) */
    int junk_size = avi_hd->sub_idx_data_size % 512;
    junk_size = 512 - junk_size;
    if (junk_size < sizeof(CHUNK)) {
        junk_size += 512;
    }

    CHUNK *junk_chunk = (void *)avi_hd->sub_idx_buff + avi_hd->sub_idx_data_size;
    avi_hd->avi_sub_junk_offset = avi_hd->sub_idx_data_size;
    avi_hd->avi_sub_junk_size = junk_size - sizeof(CHUNK);
    avi_hd->sub_idx_data_size += junk_size;
    if (avi_hd->sub_idx_data_size > avi_hd->sub_idx_buff_size) {
        SD_AVI_ERR("sub_idx junk size over %d->%d\r\n", avi_hd->sub_idx_data_size, avi_hd->sub_idx_buff_size);
        return -4;
    }

    /* junk_chunk fill */
    junk_chunk->dwFourCC = *(uint32_t *)"JUNK";
    junk_chunk->dwSize = junk_size - sizeof(CHUNK);

    /* AVIX riff fill */
    aiv_sub_avix->dwRIFF = *(uint32_t *)"RIFF";
    aiv_sub_avix->dwSize = avi_hd->sub_idx_data_size - 8;
    aiv_sub_avix->dwFourCC = *(uint32_t *)"AVIX";

    /* AVIX movi list fill */
    avix_movi_list->dwList = *(uint32_t *)"LIST";
    avix_movi_list->dwSize = avi_hd->sub_idx_data_size - sizeof(RIFF) - 8;
    avix_movi_list->dwFourCC = *(uint32_t *)"movi";

    /* save the sector_num */
    avi_hd->disk_sub_idx_sector_num = avi_hd->sub_idx_data_size / 512;

    SD_AVI_INFO("sub idx data size %d\r\n", avi_hd->sub_idx_data_size);
    SD_AVI_INFO("sub idx junk size %d\r\n", junk_size);
    SD_AVI_INFO("sub idx sector num %d\r\n", avi_hd->disk_sub_idx_sector_num);


    return 0;
}

/*
*/
int avi_jpeg_sub_idx_create(avi_file_t *avi_hd)
{
    FRESULT ret;
    UINT bw;
    uint64_t avix_riff_fptr;

    /* TODO:
        将正在使用的子索引镜像写入扇区，然后清理索引内容，准备作为新的子索引使用，
        将文件位置对齐到簇大小，以保证后面扇区是连续的，(如果簇剩余大小足够，其实对齐到扇区就够了)
        然后将子索引缓存处理后，写入到文件，并记录下物理扇区，
        这里将子索引的 base_offset 设置为索引自身位置，同时这个位置被记录在总索引里，
        写入新的帧时，需要把帧的绝对位置减去这个 base_offset，目的是为了可以突破文件 4G 大小限制。
    */

    /* align to cluster size */
    int junk_size = avi_hd->new_chunk_offset % avi_hd->fs_clus_size;
    if (junk_size) {
        /* */
        junk_size = avi_hd->fs_clus_size - junk_size;
        if (junk_size < sizeof(CHUNK)) {
            junk_size += avi_hd->fs_clus_size;
        }

        CHUNK junk_chunk;
        junk_chunk.dwFourCC = *(uint32_t *)"JUNK";
        junk_chunk.dwSize = junk_size - sizeof(CHUNK);

        /* write junk_chunk header */
        ret = f_write(avi_hd->avi_file, &junk_chunk, sizeof(CHUNK), &bw);
        if (ret != FR_OK) {
            SD_AVI_ERR("sub_idx create error, f_write junk, code %d\r\n", ret);
            return -2;
        }

        /* lseek junk_chunk data size */
        f_lseek(avi_hd->avi_file, avi_hd->avi_file->fptr + junk_size - sizeof(CHUNK));

        avi_hd->new_chunk_offset += junk_size;
    }
    /* check file fptr */
    if (avi_hd->avi_file->fptr != avi_hd->new_chunk_offset) {
        SD_AVI_ERR("sub_idx_create junk fprt error: %lld->%lld\r\n", avi_hd->avi_file->fptr, avi_hd->new_chunk_offset);
        return -3;
    }
    /* sync the old index (if need) */
    if (avi_hd->disk_sub_idx_sector_offset) {
        avi_hd->p_avix_riff->dwSize += junk_size;
        avi_jpeg_file_sync(avi_hd, 0x02);
    } else {
        avi_hd->p_riff->dwSize += junk_size;
        avi_jpeg_file_sync(avi_hd, 0x01);
    }

    /* clean avix_riff size */
    avi_hd->p_avix_riff->dwSize = avi_hd->sub_idx_data_size - sizeof(CHUNK);
    avi_hd->p_avix_movi_list->dwSize = avi_hd->sub_idx_data_size - sizeof(RIFF) - sizeof(CHUNK);

    avix_riff_fptr = avi_hd->avi_file->fptr;

    SD_AVI_INFO("new avix offset %lld\r\n", avix_riff_fptr);

    /* vids */
    if (avi_hd->avi_param.vids_en) {
        if (avi_hd->p_vids_super_idx->nEntriesInUse >= avi_hd->vids_super_idx_total) {
            SD_AVI_ERR("vids super_idx over %d\r\n", avi_hd->p_vids_super_idx->nEntriesInUse);
        }

        /* clean the indx */
        avi_hd->p_vids_std_idx->nEntriesInUse = 0;
        memset(avi_hd->p_vids_std_idx->stdindex_entry, 0, sizeof(struct avistdindex_entry) * avi_hd->vids_sub_idx_total);

        /* update sub_idx base offset */
        avi_hd->p_vids_std_idx->qwBaseOffset = avix_riff_fptr;

        /* update new sub_idx to super_idx in header */
        struct avisuperindex_entry vids_sub_idx_entry;
        vids_sub_idx_entry.qwOffset = avix_riff_fptr + avi_hd->vids_sub_idx_offset;
        vids_sub_idx_entry.dwSize = sizeof(AVISTDINDEXCHUNK) + sizeof(struct avistdindex_entry) * avi_hd->vids_sub_idx_total;
        vids_sub_idx_entry.dwDuration = 0;
        avi_hd->p_vids_super_idx->superindex_entry[avi_hd->p_vids_super_idx->nEntriesInUse] = vids_sub_idx_entry;
        avi_hd->p_vids_super_idx->nEntriesInUse += 1;

        avi_hd->vids_last_chunk_ptr = avi_hd->avi_sub_junk_offset;
        avi_hd->vids_last_chunk_size = WAVE_NOT_KEY_FRAME;
    }

    /* auds */
    if (avi_hd->avi_param.auds_en) {
        if (avi_hd->p_auds_super_idx->nEntriesInUse >= avi_hd->auds_super_idx_total) {
            SD_AVI_ERR("auds super_idx over %d\r\n", avi_hd->p_auds_super_idx->nEntriesInUse);
        }

        /* clean the indx */
        avi_hd->p_auds_std_idx->nEntriesInUse = 0;
        memset(avi_hd->p_auds_std_idx->stdindex_entry, 0, sizeof(struct avistdindex_entry) * avi_hd->auds_sub_idx_total);

        /* update sub_idx base offset */
        avi_hd->p_auds_std_idx->qwBaseOffset = avix_riff_fptr;

        /* update new sub_idx to super_idx in header */
        struct avisuperindex_entry auds_sub_idx_entry;
        auds_sub_idx_entry.qwOffset = avix_riff_fptr + avi_hd->auds_sub_idx_offset;
        auds_sub_idx_entry.dwSize = sizeof(AVISTDINDEXCHUNK) + sizeof(struct avistdindex_entry) * avi_hd->auds_sub_idx_total;
        auds_sub_idx_entry.dwDuration = 0;
        avi_hd->p_auds_super_idx->superindex_entry[avi_hd->p_auds_super_idx->nEntriesInUse] = auds_sub_idx_entry;
        avi_hd->p_auds_super_idx->nEntriesInUse += 1;

        avi_hd->auds_last_chunk_ptr = avi_hd->avi_sub_junk_offset;
        avi_hd->auds_last_chunk_size = 0;
    }

    /* write new sub_idx */
    ret = psram_f_write(avi_hd->avi_file, avi_hd->sub_idx_buff, avi_hd->sub_idx_data_size, &bw);
    if (ret != FR_OK) {
        SD_AVI_ERR("sub_idx create error, f_write sub_idx, code %d\r\n", ret);
        return -2;
    }
    /* save offset */
    avi_hd->new_chunk_offset += avi_hd->sub_idx_data_size;

    /* sync header */
    avi_jpeg_file_sync(avi_hd, 0x01);

    /* TODO:
        将新的子索引的位置，记录到总索引中，同时这个位置也是子索引的 base_offset
    */
    /* save physical sector */
    avi_hd->disk_sub_idx_sector_offset = avi_hd->avi_file->obj.fs->database +
                                         avi_hd->avi_file->obj.fs->csize * (avi_hd->avi_file->clust - 2);

    SD_AVI_DBG("sub idx sector offset %d\r\n", avi_hd->disk_sub_idx_sector_offset);

    return 0;
}

/*
    vids_fps, 0:no video_stream; other:video_fps (auds_fps = vids_fps)
    auds_frame_size, 0:no audio_steam; other:audio_frame_size (16KHz*16bit)
*/
avi_file_t *avi_file_create(FIL *p_file, avi_param_t *avi_param)
{
    FRESULT ret;
    UINT bw;

    avi_file_t *avi_hd;
    uint8_t *head_buff;
    uint8_t *sub_idx_buff;

    if (p_file == NULL) {
        return NULL;
    }

    if (avi_param->vids_en == 0 && avi_param->auds_en == 0) {
        return NULL;
    }

    if (g_fatfs_fs.csize * 512 < AVI_HEAD_BUFF) {
        SD_AVI_ERR("avi create error, fs->csize");
        return NULL;
    }

    /* clear file data */
    f_lseek(p_file, 0);
    f_truncate(p_file);
    f_sync(p_file);

    /* alloc cluster */
    f_expand(p_file, AVI_HEAD_BUFF, 1);
    f_sync(p_file);

    /* head buff, ocram */
    head_buff = malloc(AVI_HEAD_BUFF);
    if (head_buff == NULL) {
        return NULL;
    }
    memset(head_buff, 0, AVI_HEAD_BUFF);

    /* sub idx buff, ocram */
    sub_idx_buff = malloc(AVI_SUB_IDX_BUFF);
    if (sub_idx_buff == NULL) {
        return NULL;
    }
    memset(sub_idx_buff, 0, AVI_SUB_IDX_BUFF);

    /* header buff */
    avi_hd = malloc(sizeof(avi_file_t));
    if (avi_hd == NULL) {
        return NULL;
    }
    memset(avi_hd, 0, sizeof(avi_file_t));

    /*  */
    avi_hd->avi_param = *avi_param;
    avi_hd->avi_file = p_file;
    avi_hd->fs_clus_size = g_fatfs_fs.csize * 512;

    avi_hd->head_buff = head_buff;
    avi_hd->head_buff_size = AVI_HEAD_BUFF;

    avi_hd->sub_idx_buff = sub_idx_buff;
    avi_hd->sub_idx_buff_size = AVI_SUB_IDX_BUFF;

    /* build header format */
    if (avi_build_header(avi_hd) < 0) {
        free(head_buff);
        free(sub_idx_buff);
        free(avi_hd);
        SD_AVI_ERR("build header error\r\n");
        return NULL;
    }

    /* build and init sub_idx */
    if (avi_build_subindex(avi_hd) < 0) {
        free(head_buff);
        free(sub_idx_buff);
        free(avi_hd);
        SD_AVI_ERR("build sub_idx error\r\n");
        return NULL;
    }

    /* header fill */

    /* fill para */
    /* main header */
    avi_hd->p_main_header->dwMicroSecPerFrame = 1000 * 1000 / avi_param->avi_fps;
    avi_hd->p_main_header->dwMaxBytesPerSec = avi_param->avi_fps * 512 * 1024;
    avi_hd->p_main_header->dwPaddingGranularity = 2;
    avi_hd->p_main_header->dwFlags = AVIF_HASINDEX | AVIF_MUTUSEINDEX | AVIF_TRUSTCKTYPE;         /* flag */
    avi_hd->p_main_header->dwTotalFrames = 0;                                                     /* untrustworthy */
    avi_hd->p_main_header->dwInitialFrames = 0;                                                   /* frames num */
    avi_hd->p_main_header->dwStreams = avi_param->vids_en + avi_param->auds_en;                   /* strems num */
    avi_hd->p_main_header->dwSuggestedBufferSize = avi_param->width * avi_param->hight * 4 + 512; /* untrustworthy */
    avi_hd->p_main_header->dwWidth = avi_param->width;
    avi_hd->p_main_header->dwHeight = avi_param->hight;

    if (avi_param->vids_en) {
        /* video strh */
        avi_hd->p_vids_strh->fccType = *(uint32_t *)"vids";
        avi_hd->p_vids_strh->fccHandler = *(uint32_t *)"MJPG";
        avi_hd->p_vids_strh->dwFlags = 0;
        avi_hd->p_vids_strh->wPriority = 0;
        avi_hd->p_vids_strh->wLanguage = 0;
        avi_hd->p_vids_strh->dwInitialFrames = 0;
        avi_hd->p_vids_strh->dwScale = 1;
        avi_hd->p_vids_strh->dwRate = avi_param->avi_fps;
        avi_hd->p_vids_strh->dwStart = 0;
        avi_hd->p_vids_strh->dwLength = 0; /* frames num */
        avi_hd->p_vids_strh->dwSuggestedBufferSize = avi_param->width * avi_param->hight * 4 + 512;
        avi_hd->p_vids_strh->dwQuality = 0;
        avi_hd->p_vids_strh->dwSampleSize = 0;
        avi_hd->p_vids_strh->rcFrame.left = 0;
        avi_hd->p_vids_strh->rcFrame.top = 0;
        avi_hd->p_vids_strh->rcFrame.right = avi_param->width;
        avi_hd->p_vids_strh->rcFrame.bottom = avi_param->hight;

        /* video strf */
        avi_hd->p_vids_strf->biWidth = avi_param->width;
        avi_hd->p_vids_strf->biHeight = avi_param->hight;
        avi_hd->p_vids_strf->biPlanes = 1;
        avi_hd->p_vids_strf->biBitCount = 24;
        avi_hd->p_vids_strf->biCompression = *(uint32_t *)"MJPG";
        avi_hd->p_vids_strf->biSizeImage = 3 * avi_param->width * avi_param->hight;
        avi_hd->p_vids_strf->biXPelsPerMeter = 0;
        avi_hd->p_vids_strf->biYPelsPerMeter = 0;
        avi_hd->p_vids_strf->biClrUsed = 0;
        avi_hd->p_vids_strf->biClrImportant = 0;

        /* video super index */
        avi_hd->p_vids_super_idx->bIndexSubType = 0;
        avi_hd->p_vids_super_idx->bIndexType = AVI_INDEX_OF_INDEXES;
        avi_hd->p_vids_super_idx->nEntriesInUse = 0;
        avi_hd->p_vids_super_idx->dwChunkId = *(uint32_t *)"00dc";
    }

    if (avi_param->auds_en) {
        /* audio strh */
        avi_hd->p_auds_strh->fccType = *(uint32_t *)"auds";
        avi_hd->p_auds_strh->fccHandler = *(uint32_t *)"PCM ";
        avi_hd->p_auds_strh->dwFlags = 0;
        avi_hd->p_auds_strh->wPriority = 0;
        avi_hd->p_auds_strh->wLanguage = 0;
        avi_hd->p_auds_strh->dwInitialFrames = 0;
        avi_hd->p_auds_strh->dwScale = 1;
        avi_hd->p_auds_strh->dwRate = avi_param->sample_rate;
        avi_hd->p_auds_strh->dwStart = 0;
        avi_hd->p_auds_strh->dwLength = 0; /* samples num */
        avi_hd->p_auds_strh->dwSuggestedBufferSize = 16000 * 2;
        avi_hd->p_auds_strh->dwQuality = 0;
        avi_hd->p_auds_strh->dwSampleSize = avi_param->chanenl_num * avi_param->sample_rate * avi_param->sample_bit_size;
        avi_hd->p_auds_strh->rcFrame.left = 0;
        avi_hd->p_auds_strh->rcFrame.top = 0;
        avi_hd->p_auds_strh->rcFrame.right = 0;
        avi_hd->p_auds_strh->rcFrame.bottom = 0;

        /* audio strf */
        avi_hd->p_auds_strf->wFormatTag = 1;
        avi_hd->p_auds_strf->wChannels = avi_param->chanenl_num;
        avi_hd->p_auds_strf->dwSamplesPerSec = avi_param->sample_rate;
        avi_hd->p_auds_strf->dwAvgBytesPerSec = avi_param->chanenl_num * avi_param->sample_rate * avi_param->sample_bit_size / 8;
        avi_hd->p_auds_strf->wBlockAlign = avi_param->chanenl_num * avi_param->sample_bit_size / 8;
        avi_hd->p_auds_strf->wBitsPerSample = avi_param->sample_bit_size;
        avi_hd->p_auds_strf->wSize = 0;

        /* audio super index */
        avi_hd->p_auds_super_idx->bIndexSubType = 0;
        avi_hd->p_auds_super_idx->bIndexType = AVI_INDEX_OF_INDEXES;
        avi_hd->p_auds_super_idx->nEntriesInUse = 0;
        avi_hd->p_auds_super_idx->dwChunkId = *(uint32_t *)"01wb";

        avi_hd->auds_frame_size = avi_param->auds_frame_size;
        avi_hd->auds_scale_size = avi_param->chanenl_num * avi_param->sample_bit_size / 8; /* chanenl_num * sample_size*/
    }

    /* save header to sd_card */
    /* write avi_header to SD card */
    ret = psram_f_write(avi_hd->avi_file, avi_hd->head_buff, avi_hd->head_data_size, &bw);
    if (ret != FR_OK) {
        SD_AVI_ERR("avi create error, f_write, code %d\r\n", ret);
        return NULL;
    }
    /* update ptr */
    avi_hd->new_chunk_offset = avi_hd->head_data_size;

    /* save head physical sector (file top, first sclust) */
    avi_hd->disk_head_sector_offset = p_file->obj.fs->database + p_file->obj.fs->csize * (p_file->obj.sclust - 2);

    SD_AVI_DBG("header sector offset %d\r\n", avi_hd->disk_head_sector_offset);

    /* create the avix_riff and sub_idx */
    avi_jpeg_sub_idx_create(avi_hd);

    return avi_hd;
}

/*
    mode: bit0: header sync, bit1: sub_idx sync
 */
int avi_jpeg_file_sync(avi_file_t *avi_hd, uint8_t mode)
{
    FRESULT ret;

    if (avi_hd == NULL || avi_hd->head_buff == NULL || avi_hd->sub_idx_buff == NULL) {
        return -1;
    }

    /* sync file */
    f_sync(avi_hd->avi_file);

    /* get physical ID */
    BYTE pdrv = avi_hd->avi_file->obj.fs->pdrv;

    if (mode & 0x01) {
        /* check header */
        if (avi_hd->disk_head_sector_offset == 0 || avi_hd->disk_head_sector_num == 0) {
            return -2;
        }
        if (avi_hd->disk_head_sector_num * 512 != avi_hd->head_data_size) {
            SD_AVI_ERR("header_sync, size error %d\r\n", avi_hd->head_data_size);
            return -3;
        }

        /* TODO:
            此前已经让 header 对齐到扇区，并保留了文件top物理扇区号，
            这里就可以直接将更新后头写入到物理扇区
        */
        ret = disk_write(pdrv, avi_hd->head_buff, avi_hd->disk_head_sector_offset, avi_hd->disk_head_sector_num);
        if (ret != FR_OK) {
            SD_AVI_ERR("header_sync, disk_write error %d\r\n", ret);
            return -4;
        }
    }

    if (mode & 0x02) {
        /* check sub_indx */
        if (avi_hd->disk_sub_idx_sector_offset == 0 || avi_hd->disk_sub_idx_sector_num == 0) {
            return -5;
        }
        if (avi_hd->disk_sub_idx_sector_num * 512 != avi_hd->sub_idx_data_size) {
            SD_AVI_ERR("sub_idx_sync, size error %d\r\n", avi_hd->sub_idx_data_size);
            return -6;
        }

        /* TODO:
            子索引是随着文件增长而不停创建的，但只需要更新最后一个子索引就行了,
            此前每次写入新子索引时，已经将位置对齐到单个簇内的扇区，并记录了物理扇区号和数量，
            这里就可以直接将子索引数据写入到物理扇区
        */
        ret = disk_write(pdrv, avi_hd->sub_idx_buff, avi_hd->disk_sub_idx_sector_offset, avi_hd->disk_sub_idx_sector_num);
        if (ret != FR_OK) {
            SD_AVI_ERR("sub_idx_sync, disk_write error %d\r\n", ret);
            return -7;
        }
    }

    return 0;
}

/* 
    jpeg_frame, NULL:Insert old frame, other:new frame.
    pcm_frame,  NULL:Insert old frame, other:new frame,size:  (channel_num * sampling_rate * sampling_size) / vids_fps
 */
int avi_write_frame(avi_file_t *avi_hd, void *jpeg_frame, uint32_t frame_size, void *pcm_frame)
{
    FRESULT ret;
    UINT bw;

    /* movi chunk head */
    CHUNK movi_data;

    /* check file fptr */
    if (avi_hd->avi_file->fptr != avi_hd->new_chunk_offset) {
        SD_AVI_ERR("write_frame fprt error: %lld->%lld\r\n", avi_hd->avi_file->fptr, avi_hd->new_chunk_offset);
        return -1;
    }

    /******** vids ********/
    if (avi_hd->avi_param.vids_en) {
        /* sub idx elem */
        struct avistdindex_entry vids_sub_idx_entry;

        if (jpeg_frame) {
            frame_size = (frame_size + 31) & 0xffffffe0;

            SD_AVI_DBG("write vids new frame, size %d\r\n", frame_size);

            /* chunk header */
            movi_data.dwFourCC = *(uint32_t *)"00dc";
            movi_data.dwSize = frame_size;

            /* write vids chunk header */
            ret = f_write(avi_hd->avi_file, &movi_data, sizeof(CHUNK), &bw);
            if (ret != FR_OK || bw != sizeof(CHUNK)) {
                SD_AVI_ERR("avi_write vids head error, f_write, code %d\r\n", ret);
                return -4;
            }

            /* write vids chunk data */
            ret = psram_f_write(avi_hd->avi_file, jpeg_frame, frame_size, &bw);
            if (ret != FR_OK || bw != frame_size) {
                SD_AVI_ERR("avi_write vids data error, f_write, code %d\r\n", ret);
                return -6;
            }

            /* update vids sub_idx */
            vids_sub_idx_entry.dwOffset = avi_hd->new_chunk_offset + sizeof(CHUNK) - avi_hd->p_vids_std_idx->qwBaseOffset;
            vids_sub_idx_entry.dwSize = frame_size;

            /* update avix riff size */
            avi_hd->p_avix_riff->dwSize += (sizeof(CHUNK) + frame_size);
            /* update movi list size */
            avi_hd->p_avix_movi_list->dwSize += (sizeof(CHUNK) + frame_size);

            avi_hd->vids_last_chunk_ptr = vids_sub_idx_entry.dwOffset;
            avi_hd->vids_last_chunk_size = vids_sub_idx_entry.dwSize;

            /* update chunk_offset */
            avi_hd->new_chunk_offset += (sizeof(CHUNK) + frame_size);
        } else {
            SD_AVI_DBG("write vids old frame\r\n");

            /* update vids sub_idx */
            vids_sub_idx_entry.dwOffset = avi_hd->vids_last_chunk_ptr;
            vids_sub_idx_entry.dwSize = avi_hd->vids_last_chunk_size;
        }
        avi_hd->p_vids_std_idx->stdindex_entry[avi_hd->p_vids_std_idx->nEntriesInUse] = vids_sub_idx_entry;
        avi_hd->p_vids_std_idx->nEntriesInUse++;

        /* update avi super idx */
        avi_hd->p_vids_super_idx->superindex_entry[avi_hd->p_vids_super_idx->nEntriesInUse - 1].dwDuration += 1;
        /* update avi header */
        avi_hd->p_vids_strh->dwLength++;
        avi_hd->p_dmlh->dwTotalFrames++;
    }

    /******** auds ********/
    if (avi_hd->avi_param.auds_en) {
        /* sub idx elem */
        struct avistdindex_entry auds_sub_idx_entry;

        if (pcm_frame) {
            SD_AVI_DBG("write auds new frame, size %d\r\n", avi_hd->auds_frame_size);

            /* chunk header */
            movi_data.dwFourCC = *(uint32_t *)"01wb";
            movi_data.dwSize = avi_hd->auds_frame_size;

            /* write auds chunk header */
            ret = f_write(avi_hd->avi_file, &movi_data, sizeof(CHUNK), &bw);
            if (ret != FR_OK || bw != sizeof(CHUNK)) {
                SD_AVI_ERR("avi_write auds head error, f_write, code %d\r\n", ret);
                return -4;
            }

            /* write auds chunk data */
            ret = psram_f_write(avi_hd->avi_file, pcm_frame, avi_hd->auds_frame_size, &bw);
            if (ret != FR_OK || bw != avi_hd->auds_frame_size) {
                SD_AVI_ERR("avi_write auds data error, f_write, code %d\r\n", ret);
                return -6;
            }

            /* update auds sub_idx */
            auds_sub_idx_entry.dwOffset = avi_hd->new_chunk_offset + sizeof(CHUNK) - avi_hd->p_auds_std_idx->qwBaseOffset;
            auds_sub_idx_entry.dwSize = avi_hd->auds_frame_size;

            /* update avix riff size */
            avi_hd->p_avix_riff->dwSize += (sizeof(CHUNK) + avi_hd->auds_frame_size);
            /* update movi list size */
            avi_hd->p_avix_movi_list->dwSize += (sizeof(CHUNK) + avi_hd->auds_frame_size);

            avi_hd->auds_last_chunk_ptr = auds_sub_idx_entry.dwOffset;
            avi_hd->auds_last_chunk_size = auds_sub_idx_entry.dwSize;

            /* update chunk_offset */
            avi_hd->new_chunk_offset += (sizeof(CHUNK) + avi_hd->auds_frame_size);
        } else {
            SD_AVI_DBG("write auds old frame\r\n");

            /* update auds sub_idx */
            auds_sub_idx_entry.dwOffset = avi_hd->auds_last_chunk_ptr;
            auds_sub_idx_entry.dwSize = avi_hd->auds_last_chunk_size;
        }
        avi_hd->p_auds_std_idx->stdindex_entry[avi_hd->p_auds_std_idx->nEntriesInUse] = auds_sub_idx_entry;
        avi_hd->p_auds_std_idx->nEntriesInUse++;

        /* update avi super idx */
        avi_hd->p_auds_super_idx->superindex_entry[avi_hd->p_auds_super_idx->nEntriesInUse - 1].dwDuration += avi_hd->auds_frame_size / avi_hd->auds_scale_size;
        /* update avi header */
        avi_hd->p_auds_strh->dwLength += avi_hd->auds_frame_size / avi_hd->auds_scale_size;
    }

    if ((avi_hd->avi_param.vids_en && avi_hd->p_vids_std_idx->nEntriesInUse >= AVI_VIDS_SUB_IDX_NUM) || 
        (avi_hd->avi_param.auds_en && avi_hd->p_auds_std_idx->nEntriesInUse >= AVI_AUDS_SUB_IDX_NUM)) {
        /* create the new avix_riff and sub_idx */
        avi_jpeg_sub_idx_create(avi_hd);
    }

    return 0;
}

/* free the mm */
int avi_jpeg_close(avi_file_t *avi_hd)
{
    if (avi_hd == NULL) {
        return -1;
    }

    avi_jpeg_file_sync(avi_hd, (0x01 | 0x02));

    free(avi_hd->head_buff);
    free(avi_hd->sub_idx_buff);
    free(avi_hd);

    return 0;
}

/**************************************   *****************************************/

#include "frame_queue_ctrl.h"
#include "frame_queue.h"

static FIL fnew = { 0 };
static TaskHandle_t save_avi_init_task_hd;

/* auds and vids(selectable) */
#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS)

static uint16_t jpeg_out_queue_avi_id;
static uint16_t audio_out_queue_avi_id;

static TaskHandle_t save_avi_jpeg_pcm_task_hd;

/* TODO:
    当使用 auds 流时，将使用 auds 流作为时间主线，因为音频时间是稳定的。
    如果有 vids 流，可以在每次写入 auds 流时，附带一个 auds 帧，不附带(NULL)时使用上次帧。
    注意第一帧必须同时有 auds 和 vids，否则可能会导致 音轨和视频轨 偏移。

    按照 16K*16bit 速率，和 50-fps 帧率，每次应该写入 20ms 的 pcm 数据 640 Byte。
    但是 uav 默认一帧是 4ms, 这里临时用一个简单的 ringbuff 来转化。
*/

/* auds ring buffer */
#define AUDS_BUFF_BLOCK_SIZE (640)
#define AUDS_BUFF_BLOCK_NUM  (2)
static ATTR_DTCM_SECTION __ALIGNED(64) uint8_t avi_auds_ringbuff[AUDS_BUFF_BLOCK_SIZE * AUDS_BUFF_BLOCK_NUM];
static uint32_t avi_auds_rb_wptr = 0;
static uint32_t avi_auds_rb_block_w = 0;
static uint32_t avi_auds_rb_block_r = 0;

static void save_sdcard_avi_jpeg_pcm_task(void *pvParameters)
{
    FRESULT f_ret;
    uint32_t save_cnt = 0, error_cnt = 0;
    uint32_t file_num = 0;

    avi_file_t *avi_hd;

    audio_out_frame_t audio_out_frame;

    int ret;
    char str_buff[128];

    avi_param_t avi_param = {
        .avi_fps = 50,
#if(CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
        .vids_en = 1,
        .width = CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH,
        .hight = CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT,
#else
        .vids_en = 0,
#endif
        .auds_en = 1,
        .auds_frame_size = 640, /* frame_size = chanenl_num * sample_size * sample_rate / avi_fps */
        .chanenl_num = 1,
        .sample_rate = 16000,
        .sample_bit_size = 16,
    };

create_new_file:

    sprintf(str_buff, "/sd/avi_test/avi_jpeg_pcm_test_%d.avi", file_num);
    file_num++;

    f_ret = f_open(&fnew, str_buff, FA_CREATE_NEW | FA_WRITE | FA_READ);
    if (f_ret != FR_OK) {
        SD_AVI_ERR("avi open failed, num %d, code %d\r\n", file_num, f_ret);
        vTaskSuspend(NULL);
    }

    avi_hd = avi_file_create(&fnew, &avi_param);
    if (avi_hd == NULL) {
        SD_AVI_ERR("avi_file_create failed\r\n");
        vTaskSuspend(NULL);
    }

    save_cnt = 0;

    while (1) {
        /****** copy to ring buff ******/
        /* pop auds frame */
        ret = frame_queue_output_pop(g_audio_out_frame_ctrl, (frame_elem_t *)&audio_out_frame, audio_out_queue_avi_id, 1000);
        if (ret < 0) {
            /* audac pool empty */
            SD_AVI_INFO("audio_out pop timeout %d, continue wait... \r\n", ret);
            continue;
        }

        /* copy to ringbuff */
        if (audio_out_frame.data_size + avi_auds_rb_wptr < AUDS_BUFF_BLOCK_SIZE * AUDS_BUFF_BLOCK_NUM) {
            /*  */
            memcpy(avi_auds_ringbuff + avi_auds_rb_wptr, audio_out_frame.elem_base.frame_addr, audio_out_frame.data_size);
            avi_auds_rb_wptr += audio_out_frame.data_size;

        } else {
            uint32_t size_1 = AUDS_BUFF_BLOCK_SIZE * AUDS_BUFF_BLOCK_NUM - avi_auds_rb_wptr;
            memcpy(avi_auds_ringbuff + avi_auds_rb_wptr, audio_out_frame.elem_base.frame_addr, size_1);

            uint32_t size_2 = audio_out_frame.data_size - size_1;
            memcpy(avi_auds_ringbuff, audio_out_frame.elem_base.frame_addr + size_1, size_2);

            avi_auds_rb_wptr = size_2;
        }

        /* free the audio frame */
        frame_queue_output_free(g_audio_out_frame_ctrl, (frame_elem_t *)&audio_out_frame);

        avi_auds_rb_block_w = avi_auds_rb_wptr / AUDS_BUFF_BLOCK_SIZE;
        if (avi_auds_rb_block_r == avi_auds_rb_block_w) {
            continue;
        }
        /****** ring buff end ******/

#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
        jpeg_frame_t jpeg_frame;
        if (save_cnt > 0) {
            /* pop vids frame */
            ret = frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, jpeg_out_queue_avi_id, 0);
        } else {
            ret = frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, jpeg_out_queue_avi_id, portMAX_DELAY);
        }
#endif
        save_cnt++;

        if (ret < 0 || CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS == 0) {
            /* write null frame */
            ret = avi_write_frame(avi_hd, NULL, 0, ((void *)avi_auds_ringbuff + AUDS_BUFF_BLOCK_SIZE * avi_auds_rb_block_r));
            if (ret < 0) {
                error_cnt++;
                SD_AVI_ERR("avi write error, sd cnt:%d, err_cnt:%d, err_code:%d, offset %d\r\n", save_cnt, error_cnt, ret, avi_hd->new_chunk_offset);
            }
        }
#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
        else {
            /* align size to 32Byte */
            uint32_t frame_size = (jpeg_frame.data_size + 31) & 0xffffffe0;
            memset((jpeg_frame.elem_base.frame_addr + jpeg_frame.data_size), 0, frame_size - jpeg_frame.data_size);
            /* write to avi file */
            ret = avi_write_frame(avi_hd, jpeg_frame.elem_base.frame_addr, frame_size, ((void *)avi_auds_ringbuff + AUDS_BUFF_BLOCK_SIZE * avi_auds_rb_block_r));
            if (ret < 0) {
                error_cnt++;
                SD_AVI_ERR("avi write error, sd cnt:%d, err_cnt:%d, err_code:%d, offset %d\r\n", save_cnt, error_cnt, ret, avi_hd->new_chunk_offset);
            }

            /* free frame */
            ret = frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame);
            if (ret < 0) {
                SD_AVI_ERR("sd avi free %d, saved:%d, continue wait... \r\n", ret, save_cnt);
            }
        }
#endif

        avi_auds_rb_block_r = avi_auds_rb_block_w;

        /* sync to disk */
        if (save_cnt % 100 == 0) {
            SD_AVI_INFO("sd cnt:%d, err_cnt:%d, file:%d\r\n", save_cnt, error_cnt, file_num);
            ret = avi_jpeg_file_sync(avi_hd, (0x01 | 0x02));
            if (ret < 0) {
                SD_AVI_ERR("avi_jpeg_file_sync error\r\n");
            }
        }

        /* new file, max: (AVI_VIDS_SUP_IDX_NUM * AVI_VIDS_SUB_IDX_NUM = 225000)*/
        if (save_cnt >= (60 * 60 * 50)) {
            save_cnt = 0;
            /* close file */
            avi_jpeg_close(avi_hd);
            f_ret = f_close(&fnew);
            if (f_ret != FR_OK) {
                SD_AVI_ERR("avi close failed, num %d, code %d\r\n", file_num, f_ret);
            }
            goto create_new_file;
        }

        vTaskDelay(1);
    }
}

#elif (CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)

static uint16_t jpeg_out_queue_avi_id;
static TaskHandle_t save_avi_jpeg_task_hd;

#define AVI_VIDS_LOSS_CNT (20)

/*
    如果仅仅使用 vids 流时，视频流时间不稳定，因为帧率会变化，为了保证播放速度正常，
    这里每 20ms 尝试获取一帧，如果获取到则写入新帧，如果没有获取到，利用 avi 的 index 插入上次帧。
    当然如果连续长时间获取不到新帧，需要考虑 jpeg 的生产者出现异常，需要挂起等待。
 */
static void save_sdcard_avi_jpeg(void *pvParameters)
{
    FRESULT f_ret;
    uint32_t save_cnt = 0, null_cnt = 0, error_cnt = 0;
    uint32_t file_num = 0;

    avi_file_t *avi_hd;
    jpeg_frame_t jpeg_frame;

    TickType_t xLastWakeTime;

    int ret;
    char str_buff[128];

    avi_param_t avi_param = {
        .avi_fps = 50,

        .vids_en = 1,
        .width = CONFIG_SOLUTION_VIDEO_DEFAULT_WIDTH,
        .hight = CONFIG_SOLUTION_VIDEO_DEFAULT_HIGHT,

        .auds_en = 0,
    };

create_new_file:

    sprintf(str_buff, "/sd/avi_test/avi_jpeg_test_%d.avi", file_num);
    file_num++;
    null_cnt = 0;

    f_ret = f_open(&fnew, str_buff, FA_CREATE_NEW | FA_WRITE | FA_READ);
    if (f_ret != FR_OK) {
        SD_AVI_ERR("avi open failed, num %d, code %d\r\n", file_num, f_ret);
        vTaskSuspend(NULL);
    }

    avi_hd = avi_file_create(&fnew, &avi_param);
    if (avi_hd == NULL) {
        SD_AVI_ERR("avi_file_create failed\r\n");
        vTaskSuspend(NULL);
    }

    xLastWakeTime = xTaskGetTickCount();

    while (1) {
        /* pop vids frame */
        if (null_cnt == 0) {
            ret = frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, jpeg_out_queue_avi_id, 1000);
            if (ret < 0) {
                SD_AVI_INFO("[sd_avi] jpeg pop timeout: %d, saved:%d, continue wait... \r\n", ret, save_cnt);
                continue;
            }
            null_cnt = AVI_VIDS_LOSS_CNT;
        } else {
            ret = frame_queue_output_pop(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame, jpeg_out_queue_avi_id, 0);
        }
        save_cnt++;

        if (ret < 0) {
            /* write null frame */
            ret = avi_write_frame(avi_hd, NULL, 0, NULL);
            if (ret < 0) {
                error_cnt++;
                SD_AVI_ERR("avi write error, sd cnt:%d, err_cnt:%d, err_code:%d, offset %d\r\n", save_cnt, error_cnt, ret, avi_hd->new_chunk_offset);
            }
            null_cnt -= 1;

        } else {
            /* align size to 32Byte */
            uint32_t frame_size = (jpeg_frame.data_size + 31) & 0xffffffe0;
            memset((jpeg_frame.elem_base.frame_addr + jpeg_frame.data_size), 0, frame_size - jpeg_frame.data_size);
            /* write to avi file */
            ret = avi_write_frame(avi_hd, jpeg_frame.elem_base.frame_addr, frame_size, NULL);
            if (ret < 0) {
                error_cnt++;
                SD_AVI_ERR("avi write error, sd cnt:%d, err_cnt:%d, err_code:%d, offset %d\r\n", save_cnt, error_cnt, ret, avi_hd->new_chunk_offset);
            }

            /* free frame */
            ret = frame_queue_output_free(g_jpeg_frame_ctrl, (frame_elem_t *)&jpeg_frame);
            if (ret < 0) {
                SD_AVI_ERR("sd avi free %d, saved:%d, continue wait... \r\n", ret, save_cnt);
            }
        }

        /* sync to disk */
        if (save_cnt % 100 == 0) {
            SD_AVI_INFO("sd cnt:%d, err_cnt:%d, file:%d\r\n", save_cnt, error_cnt, file_num);
            ret = avi_jpeg_file_sync(avi_hd, (0x01 | 0x02));
            if (ret < 0) {
                SD_AVI_ERR("avi_jpeg_file_sync error\r\n");
            }
        }

        /* new file, max: (AVI_VIDS_SUP_IDX_NUM * AVI_VIDS_SUB_IDX_NUM = 225000)*/
        if (save_cnt >= (60 * 60 * 50)) {
            save_cnt = 0;
            /* close file */
            avi_jpeg_close(avi_hd);
            f_ret = f_close(&fnew);
            if (f_ret != FR_OK) {
                SD_AVI_ERR("avi close failed, num %d, code %d\r\n", file_num, f_ret);
            }

            goto create_new_file;
        }

        /* 20ms (50-fps) */
        vTaskDelayUntil(&xLastWakeTime, 20);
    }
}
#endif

static void sdcard_avi_init_task(void *pvParameters)
{
    if (fatfs_sd_init() < 0) {
        SD_AVI_ERR("sd_card init failed\r\n");
        vTaskDelete(NULL);
    }

    if (g_fatfs_fs.csize * 512 < (16 * 512)) {
        SD_AVI_ERR("fatfs cluster size %d\r\n", g_fatfs_fs.csize * 512);
        SD_AVI_ERR("The cluster size must exceed %d\r\n", g_fatfs_fs.csize * 512);
        vTaskDelete(NULL);
    }

    if (g_fatfs_fs.fs_type != FS_EXFAT) {
        SD_AVI_ERR("the Filesystem type must be exFat (>4G)\r\n");
        vTaskDelete(NULL);
    }

    SD_AVI_INFO("save_avi_to_sdcard_init\r\n");
    SD_AVI_INFO("rm -rf /sd/avi_test ...");

    /* delete dir */
    filedir_delete("/sd/avi_test");
    SD_AVI_INFO("rm OK!\r\n");
    /* create dir */
    f_mkdir("/sd/avi_test");

#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS)
    /* create audio_out output queue */
    audio_out_queue_avi_id = AUDIO_OUT_FRAME_STREAM_SD_AVI_ID;
    if (frame_queue_output_create(g_audio_out_frame_ctrl, &audio_out_queue_avi_id, AUDIO_OUT_FRAME_STREAM_AUDAC_DEPTH) < 0) {
        SD_AVI_ERR("audio_out frame sd_avi out queue create failed\r\n");
        vTaskDelete(NULL);
    } else {
        SD_AVI_INFO("audio_out frame sd_avi out queue ID: %d\r\n", audio_out_queue_avi_id);
    }

#endif

#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    /* create jpeg output queue */
    jpeg_out_queue_avi_id = MJPEG_FRAME_STREAM_SD_AVI_ID;
    if (frame_queue_output_create(g_jpeg_frame_ctrl, &jpeg_out_queue_avi_id, MJPEG_FRAME_STREAM_SD_AVI_DEPTH) < 0) {
        SD_AVI_ERR("jpeg frame sd_avi out queue create failed\r\n");
        vTaskDelete(NULL);
    } else {
        SD_AVI_INFO("jpeg frame sd_avi out queue ID: %d\r\n", jpeg_out_queue_avi_id);
    }
#endif

#if (CONFIG_SOLUTION_FUNC_SDCARD_AVI_AUDS)
    /* auds or auds_vids */
    /* create jpeg_pcm save task */
    xTaskCreate(save_sdcard_avi_jpeg_pcm_task, (char *)"save_sdcard_avi_jpeg_pcm_task", 1024, NULL, 15, &save_avi_jpeg_pcm_task_hd);

#elif (CONFIG_SOLUTION_FUNC_SDCARD_AVI_VIDS)
    /* onyl vids */
    /* create vids save task */
    xTaskCreate(save_sdcard_avi_jpeg, (char *)"save_sdcard_avi_jpeg", 1024, NULL, 15, &save_avi_jpeg_task_hd);
#endif

    /* delete init task */
    vTaskDelete(NULL);
}

int save_avi_to_sdcard_init(void)
{
    SD_AVI_INFO("sdcard_avi_init_task_init\r\n");

    xTaskCreate(sdcard_avi_init_task, (char *)"sdcard_avi_init_task", 1024, NULL, 16, &save_avi_init_task_hd);

    return 0;
}
